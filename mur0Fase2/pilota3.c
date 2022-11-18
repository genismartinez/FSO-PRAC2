/* He inclòs totes les llibreries presents en el fronton3.c */
#include <stdio.h>	/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include "winsuport2.h"	/* incloure definicions de funcions propies */
#include "memoria.h"
//#include "semafor.h"
#include "missatge.h"

#define MIDA_PALETA 4	/* definicions constants del programa */
#define MAX_PILOTES 9
#define MAX_REBOTS 5
#define BLKCHAR 'B'
#define WLLCHAR '#'
#define FRNTCHAR 'A'
#define MAX_THREADS	10
#define LENGTH 1
#define MAXBALLS 9
/* funcio per moure la pilota: retorna un 1 si la pilota surt per la porteria,*/
/* altrament retorna un 0 */

/* Necessita rebre per valor:
vel_f, vel_c,f_pil, c_pil, pos_f, pos_c
Necessita rebre per parámetre:
fi2, num_rebots
*/

int ind;
int f_pi;
int n_pil;
int num_pil;
//pthread_t tid[MAX_THREADS];
int nblocs = 0;
int f_pil, c_pil;		/* posicio de la pilota, en valor enter */
float pos_f, pos_c;		/* posicio de la pilota, en valor real */
float vel_f, vel_c;		/* velocitat de la pilota, en valor real */
int f_pal, c_pal;		/* posicio del primer caracter de la paleta */
int m_pal;				/* mida de la paleta */

void comprovar_bloc(int f, int c)
{
	int col;
	char quin = win_quincar(f, c);
	int pil= ind;
	if (quin == BLKCHAR || quin == FRNTCHAR) {
		col = c;
		while (win_quincar(f, col) != ' ') {
			win_escricar(f, col, ' ', NO_INV);
			col++;
		}
		col = c - 1;
		while (win_quincar(f, col) != ' ') {
			win_escricar(f, col, ' ', NO_INV);
			col--;
		}
		/* generar nova pilota ? */
		if(quin==BLKCHAR&&ind<=n_pil){
			fprintf(stderr,"Comprovar bloc, n_pil= %d, pil= %d\n",n_pil,pil);
        	(ind)++;
			win_escricar(f_pil,c_pil,'1',INVERS);
		}
		nblocs--;
	}
}
float control_impacte2(int c_pil, float velc0) {
	int distApal;
	float vel_c;
	
	distApal = c_pil - c_pal;
	if (distApal >= 2*m_pal/3)	/* costat dreta */
		vel_c = 0.5;
	else if (distApal <= m_pal/3)	/* costat esquerra */
		vel_c = -0.5;
	else if (distApal == m_pal/2)	/* al centre */
		vel_c = 0.0;
	else /*: rebot normal */
		vel_c = velc0;
	return vel_c;
}
int main(int n_args, char *ll_args[]){
	FILE*file=fopen("traza.txt","a");
	fprintf(stderr,"Entramos en el archivo .c\n");
	/* Variables locals */
	int f_h, c_h, i;
	char rh,rv,rd;
	/* Variables passades per valor */
	 num_pil = atoi(ll_args[1])-1;
	float vel_f = atof (ll_args[2]);
	float vel_c = atof (ll_args[3]);
	f_pil = atoi (ll_args[4]);
	c_pil = atoi (ll_args[5]);
	float pos_f = atof (ll_args[6]);
	float pos_c = atof (ll_args[7]);
	int n_col = atoi (ll_args[8]);
	int n_fil = atoi (ll_args[9]);
	int retard = atoi (ll_args[10]);	/* Variables passades per parámetre */
	fprintf(stderr,"Pilota 3\n");
	c_pal=atoi(ll_args[11]);

	int fin2 = atoi (ll_args[12]);
	int *fi2;
	fi2= (int *) map_mem(fin2);

	int id_win = atoi (ll_args[13]);
	int * p_win;
	p_win = (int*) map_mem (id_win);
	

	win_set(p_win,n_fil,n_col);	/* crea acces a finestra oberta pel proces pare */
	m_pal=atoi(ll_args[14]);
	int sem_fi2 = atoi(ll_args[15]);	    /* recupera id de semafor */

	n_pil= atoi (ll_args[16]);
	int id_mis[MAX_PILOTES];
	for (i = 0; i < n_pil; i++)
	{
		id_mis[i] = atoi (ll_args[i + 17]);	// Carreguem els ids de les busties en un array de caracters
		fprintf(stderr,"\tid_mis= %i\n",id_mis[i]);
	}
	
	do
	{
		
		f_h = pos_f+vel_f;		/* posicio hipotetica de la pilota (entera) */
		c_h = pos_c+vel_c;
		rh = rv = rd = ' ';
		if ((f_h != f_pil) || (c_h != c_pil))
        {
	if (f_h != f_pil) 		/* provar rebot vertical */
	{
                rv = win_quincar(f_h,c_pil);	/* veure si hi ha algun obstacle */
		if (rv != ' ')			/* si hi ha alguna cosa */
		{   
			comprovar_bloc(f_h,c_pil);
			if (rv == '0')
			{
				vel_c=control_impacte2(c_pil,vel_c);
                    	}
			vel_f = -vel_f;		/* canvia sentit velocitat vertical */
			f_h = pos_f+vel_f;		/* actualitza posicio hipotetica */
		}
	}
            if (c_h != c_pil) 		/* provar rebot horitzontal */
            {
            rh = win_quincar(f_pil,c_h);	/* veure si hi ha algun obstacle */
            if (rh != ' ')			/* si hi ha algun obstacle */
            {
				comprovar_bloc(f_pil,c_h);
                vel_c = -vel_c;		/* canvia sentit vel. horitzontal */
                c_h = pos_c+vel_c;		/* actualitza posicio hipotetica */
                if (rh == '0')
                {
			//waitS(sem_rebots);		/* tanca semafor */
			//(*num_rebots)--;
			//signalS(sem_rebots); 		/* obre semafor */
                }
            }
        }
        if ((f_h != f_pil) && (c_h != c_pil))	/* provar rebot diagonal */
        {
            rd = win_quincar(f_h,c_h);
            if (rd != ' ')				/* si hi ha obstacle */
            {
				comprovar_bloc(f_h,c_h);
                vel_f = -vel_f; vel_c = -vel_c;	/* canvia sentit velocitats */
                f_h = pos_f+vel_f;
                c_h = pos_c+vel_c;		/* actualitza posicio entera */
                if (rd == '0')
                {
			//waitS(sem_rebots);		/* tanca semafor */
			//(*num_rebots)--;
			//signalS(sem_rebots); 		/* obre semafor */
                }
            }
        }

	//waitS(sem_rebots);		/* tanca semafor */
        if (win_quincar(f_h,c_h) == ' ')	/* verificar posicio definitiva */
        {					/* si no hi ha obstacle */
		    win_escricar(f_pil,c_pil,' ',NO_INV);  	/* esborra pilota */
            pos_f += vel_f; pos_c += vel_c;
            f_pil = f_h; c_pil = c_h;		/* actualitza posicio actual */
            if (f_pil!=(n_fil-1)){	 		/* si ho surt del taulell, */
                //fprintf(stderr,"n_fil= %d, f_pil= %d\n",n_fil,f_pil);
				win_escricar(f_pil,c_pil, '1' ,INVERS); /* imprimeix pilota */
            }
            else
            {
				fprintf(stderr,"Done\n"); 
		//waitS(sem_fi2);
                (*fi2)=1;		/* final per sortir  */
		//signalS(sem_fi2);
            }
        }
	
    }
	
    else { pos_f += vel_f; pos_c += vel_c; }
	//signalS(sem_rebots);
	
    win_retard(retard);
	}while (!(*fi2));
			/* obre semafor */
	
	fclose(file);
	return(0);			/* retorna sense errors d'execucio */
}

