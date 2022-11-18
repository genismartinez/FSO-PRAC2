/*****************************************************************************/
/*                                                                           */
/*                           Fronton3.c                                      */
/*                                                                           */
/*  Programa inicial d'exemple per a les practiques 2 i 3 d'ISO.	     */
/*                                                                           */
/*  Compilar i executar:					  	     */
/*     El programa invoca les funcions definides a "winsuport.c", les        */
/*     quals proporcionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':				     */
/*									     */
/*	   $ gcc -c winsuport2.c -o winsuport2.o				*/
/*	   $ gcc fronton0.c winsuport.o -o fronton0 -lcurses		     */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>	/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include "winsuport2.h"	/* incloure definicions de funcions propies */
#include "memoria.h"
#include "semafor.h"
#include "missatge.h"
#include <pthread.h>

pthread_t busons;					/*adreça id del thread encrregat del moviment de la paleta*/
int *id_bustia , id_sem;
float vel_f, vel_c; 
int pilota_rebot=0;
int tipus_xoc=0;
char missatge[4], miss[4];


/* tractament de les bústies*/
void * busons_thread(void * i)
{
int index = (int) i;
do{
	receiveM(id_bustia[index], miss);
	while(tipus_xoc==0){}
	if(tipus_xoc==1){
		vel_f = atof(miss);
		tipus_xoc=0;
	}
	if(tipus_xoc==2){
		vel_c = atof(miss);
		tipus_xoc=0;
	}
	if(tipus_xoc==3){
		vel_f = atof(miss);
		receiveM(id_bustia[index], miss);
		vel_c = atof(miss);
		tipus_xoc=0;
	}

}while(1);

}

/* programa principal*/
int main(int n_args, char *ll_args[])
{
	int f_h, c_h;
	char rh,rv,rd;
	int f_pil, c_pil;
	float pos_f, pos_c;
	void *p_win;
	int mida_camp, n_fil, n_col;
	int retard;							/* valor del retard de moviment, en mil.lisegons */
	int id_fi1, *fi1, id_fi2, *fi2, id_fiRebots, *fiRebots;		/* convertim les condicions de fi en globals*/
	int MAX_REBOTS= atoi(ll_args[15]);
	int id_bustia_mem;
	int index;

	pos_f= atof(ll_args[6]);  
	pos_c= atof(ll_args[7]);
	vel_f= atof(ll_args[8]);
	vel_c= atof(ll_args[9]);
	f_pil = atoi(ll_args[10]);
	c_pil = atoi(ll_args[11]);
	id_fi1 = atoi(ll_args[12]);
	id_fi2 = atoi(ll_args[13]);
	id_fiRebots = atoi(ll_args[14]);

	index = atoi(ll_args[1]);

	fi1 = map_mem(id_fi1);		/* obtenir adres. de mem. compartida */
	fi2 = map_mem(id_fi2);		/* obtenir adres. de mem. compartida */
	fiRebots = map_mem(id_fiRebots);		/* obtenir adres. de mem. compartida */

	mida_camp = atoi(ll_args[2]);
	p_win = map_mem(mida_camp);		/*direcció mem compartida*/

	n_fil = atoi(ll_args[3]);		/* obtenir dimensions del camp de joc */
	n_col = atoi(ll_args[4]);

	win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */

	retard = atoi(ll_args[5]);
	
	id_sem = atoi(ll_args[16]);	/*Obtenim l'identificador del semàfor*/
	
	id_bustia_mem = atoi(ll_args[17]);
	id_bustia = map_mem(id_bustia_mem);		/*direcció mem compartida*/

	pthread_create(&busons, NULL, busons_thread, (void *) index);

	do
	{
		f_h = pos_f+vel_f;		/* posicio hipotetica de la pilota (entera) */
		c_h = pos_c+vel_c;
		rh = rv = rd = ' ';
		if ((f_h != f_pil) || (c_h != c_pil))
		{		/* si posicio hipotetica no coincideix amb la posicio actual */
			if (f_h != f_pil) 		/* provar rebot vertical */
			{	
				waitS(id_sem);			/* tanquem el semàfor*/	
				rv = win_quincar(f_h,c_pil);	/* veure si hi ha algun obstacle */
				signalS(id_sem);		/* obrim el semàfor*/
				if (rv != ' ')			/* si hi ha alguna cosa */
				{   
					if(rv == '0' || rv =='+'){
						vel_f = -vel_f;		/* canvia sentit velocitat vertical */
					    	f_h = pos_f+vel_f;		/* actualitza posicio hipotetica */
					    	if (rv == '0')
					    	{ 
							waitS(id_sem);		/* tanquem el semàfor*/	
							(*fiRebots)++;
							signalS(id_sem);	/* obrim el semàfor*/
		    			    	}			
					}
					else{						
						tipus_xoc=1;
						pilota_rebot = rv - '0';
						sprintf(missatge, "%f", vel_f);
						//fprintf(stderr, "aqui %i %c %i %s\n", index, rv, pilota_rebot, missatge);
						sendM(id_bustia[pilota_rebot-1],missatge, 4);
					}
				}
    			}
    			if (c_h != c_pil) 			/* provar rebot horitzontal */
    			{	
				waitS(id_sem);			/* tanquem el semàfor*/	
				rh = win_quincar(f_pil,c_h);	/* veure si hi ha algun obstacle */
				signalS(id_sem);		/* obrim el semàfor*/	
				if (rh != ' ')			/* si hi ha alguna cosa */
				{   
					if(rh == '0' || rh =='+'){
						vel_c = -vel_c;		/* canvia sentit vel. horitzontal */
	     			     		c_h = pos_c+vel_c;		/* actualitza posicio hipotetica */
					    	if (rh == '0')
					    	{ 
							waitS(id_sem);		/* tanquem el semàfor*/	
							(*fiRebots)++;
							signalS(id_sem);	/* obrim el semàfor*/
		    			    	}			
					}
					else{						
						tipus_xoc=2;
						pilota_rebot = rh - '0';
						sprintf(missatge, "%f", vel_c);
						//fprintf(stderr, "aqui %i %c %i %s\n", index, rh, pilota_rebot, missatge);
						sendM(id_bustia[pilota_rebot-1],missatge, 4);
					}
				}	
    			}
    			if ((f_h != f_pil) && (c_h != c_pil))		/* provar rebot diagonal */
    			{	
				waitS(id_sem);				/* tanquem el semàfor*/	
				rd = win_quincar(f_h,c_h);
				signalS(id_sem);			/* obrim el semàfor*/	
				if (rd != ' ')			/* si hi ha alguna cosa */
				{   
					if(rd == '0' || rd =='+'){
						vel_c = -vel_c;		/* canvia sentit vel. horitzontal */
						vel_f = -vel_f;
	     			     		c_h = pos_c+vel_c;		/* actualitza posicio hipotetica */
						f_h = pos_f+vel_f;
					    	if (rd == '0')
					    	{ 
							waitS(id_sem);		/* tanquem el semàfor*/	
							(*fiRebots)++;
							signalS(id_sem);	/* obrim el semàfor*/
		    			    	}			
					}
					else{						
						tipus_xoc=3;
						pilota_rebot = rd - '0';
						sprintf(missatge, "%f", vel_c);
						sendM(id_bustia[pilota_rebot-1],missatge, 4);
						sprintf(missatge, "%f", vel_f);
						sendM(id_bustia[pilota_rebot-1],missatge, 4);
					}
				}
    			}
    			waitS(id_sem);				/* tanquem el semàfor*/	
    			if (win_quincar(f_h,c_h) == ' ')	/* verificar posicio definitiva */
   			{					/* si no hi ha obstacle */
				win_escricar(f_pil,c_pil,' ',NO_INV);  	/* esborra pilota */		
				pos_f += vel_f; pos_c += vel_c;
				f_pil = f_h; c_pil = c_h;		/* actualitza posicio actual */
				if (c_pil != 0){	 		/* si ho surt del taulell, */	
					win_escricar(f_pil,c_pil, '1'+index ,INVERS); /* imprimeix pilota */	
				}
				else
				{
					(*fi2)=1;		/* codi de finalitzacio de partida */
 			  	}
				signalS(id_sem);		/* obrim el semàfor*/	
 			}
			else	signalS(id_sem);		/* obrim el semàfor*/
  		}
  		else { pos_f += vel_f; pos_c += vel_c; }
  		win_retard(retard);
	}while (!(*fi1) && !(*fi2) && (*fiRebots)<MAX_REBOTS);

	//pthread_destroy(busons);

	return(0);			/* retorna sense errors d'execucio */
}
