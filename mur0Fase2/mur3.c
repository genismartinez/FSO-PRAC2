/*****************************************************************************/
/*                                                                           */
/*                           mur0.c                                          */
/*                                                                           */
/*  Programa inicial d'exemple per a les practiques 2 de FSO.                */
/*                                                                           */
/*  Compilar i executar:                                                     */
/*     El programa invoca les funcions definides a "winsuport.c", les        */
/*     quals proporcionen una interficie senzilla per crear una finestra     */
/*     de text on es poden escriure caracters en posicions especifiques de   */
/*     la pantalla (basada en CURSES); per tant, el programa necessita ser   */
/*     compilat amb la llibreria 'curses':                                   */
/*                                                                           */
/*       $ gcc -Wall -c winsuport.c -o winsuport.o                           */
/*       $ gcc -Wall mur0.c winsuport.o -o mur0 -lcurses                     */
/*                                                                           */
/*  Al tenir una orientació vertical cal tenir un terminal amb prou files.   */
/*  Per exemple:                                                             */
/*               xterm -geometry 80x50                                       */
/*               gnome-terminal --geometry 80x50                             */
/*                                                                           */
/*****************************************************************************/

//#include <stdint.h>		/* intptr_t for 64bits machines */
#define _REENTRANT
#include <pthread.h>
#include <stdio.h>		/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include "winsuport2.h"		/* incloure definicions de funcions propies */
#include <stdint.h>
#include <time.h>
#include "memoria.h"
#include <unistd.h>
#include "missatge.h"
#include <sys/wait.h>
/* definicio de constants */
#define MAX_THREADS	10
#define LENGTH 1
#define MAXBALLS 9
#define MIN_FIL	10		/* dimensions del camp. Depenen del terminal ! */
#define MAX_FIL	50
#define MIN_COL	10
#define MAX_COL	80
#define BLKSIZE	3
#define BLKGAP	2
#define BLKCHAR 'B'
#define WLLCHAR '#'
#define FRNTCHAR 'A'
#define LONGMISS	65
#define MAX_THREADS 10
pthread_t tid[MAX_THREADS];
pthread_t idpa;
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pid_t tpid[MAXBALLS];
			/* variables globals */
char *descripcio[] = {
	"\n",
	"Aquest programa implementa una versio basica del joc Arkanoid o Breakout:\n",
	"generar un camp de joc rectangular amb una porteria, una paleta que s\'ha\n",
	"de moure amb el teclat per a cobrir la porteria, i una pilota que rebota\n",
	"contra les parets del camp, a la paleta i els blocs. El programa acaba si\n",
	"la pilota surt per la porteria o no queden mes blocs. Tambe es pot acabar\n",
	"amb la tecla RETURN.\n",
	"\n",
	"  Arguments del programa:\n",
	"\n",
	"       $ ./mur0 fitxer_config [retard]\n",
	"\n",
	"     El primer argument ha de ser el nom d\'un fitxer de text amb la\n",
	"     configuracio de la partida, on la primera fila inclou informacio\n",
	"     del camp de joc (valors enters), i la segona fila indica posicio\n",
	"     i velocitat de la pilota (valors reals):\n",
	"          num_files  num_columnes  mida_porteria\n",
    "          pos_fil_pal pos_col_pal mida_pal\n"
	"          pos_fila   pos_columna   vel_fila  vel_columna\n",
	"\n",
	"     on els valors minims i maxims admesos son els seguents:\n",
	"          MIN_FIL <= num_files     <= MAX_FIL\n",
	"          MIN_COL <= num_columnes  <= MAX_COL\n",
	"          0 < mida_porteria < num_files-2\n",
	"        1.0 <= pos_fila     <= num_files-3\n",
	"        1.0 <= pos_columna  <= num_columnes\n",
	"       -1.0 <= vel_fila     <= 1.0\n",
	"       -1.0 <= vel_columna  <= 1.0\n",
	"\n",
	"     Alternativament, es pot donar el valor 0 a num_files i num_columnes\n",
	"     per especificar que es vol que el tauler ocupi tota la pantalla. Si\n",
	"     tambe fixem mida_porteria a 0, el programa ajustara la mida d\'aquesta\n",
	"     a 3/4 de l\'altura del camp de joc.\n",
	"\n",
	"     A mes, es pot afegir un segon argument opcional per indicar el\n",
	"     retard de moviment del joc en mil.lisegons; el valor minim es 10,\n",
	"     el valor maxim es 1000, i el valor per defecte d'aquest parametre\n",
	"     es 100 (1 decima de segon).\n",
	"\n",
	"  Codis de retorn:\n",
	"     El programa retorna algun dels seguents codis:\n",
	"	0  ==>  funcionament normal\n",
	"	1  ==>  numero d'arguments incorrecte\n",
	"	2  ==>  no s\'ha pogut obrir el fitxer de configuracio\n",
	"	3  ==>  algun parametre del fitxer de configuracio es erroni\n",
	"	4  ==>  no s\'ha pogut crear el camp de joc (no pot iniciar CURSES)\n",
	"\n",
	"   Per a que pugui funcionar aquest programa cal tenir instal.lada la\n",
	"   llibreria de CURSES (qualsevol versio).\n",
	"\n",
	"*"
};				/* final de la descripcio */

int n_fil, n_col;		/* numero de files i columnes del taulell */
int m_por;			/* mida de la porteria (en caracters) */
int f_pal, c_pal;		/* posicio del primer caracter de la paleta */
int m_pal;				/* mida de la paleta */
int f_pil[MAXBALLS], c_pil[MAXBALLS];		/* posicio de la pilota, en valor enter */
float pos_f[MAXBALLS], pos_c[MAXBALLS];		/* posicio de la pilota, en valor real */
float vel_f[MAXBALLS], vel_c[MAXBALLS];		/* velocitat de la pilota, en valor real */
int nblocs = 0;
int dirPaleta = 0;
int retard;			/* valor del retard de moviment, en mil.lisegons */
//int fi1,fi2;
char strin[LONGMISS];			/* variable per a generar missatges de text */
int n_pil;
int *fi1;
int ind=0;

time_t temps_actual,temps_init;
char Aid_mis[MAXBALLS][10];
int id_mis[MAXBALLS];
int id_mis[MAXBALLS];
char Ac_pal[20],Am_pal[20];
char Anum_pil[20], Aretard[20], Afin2[20], Arebotes[20], An_col[20], An_fil[20], Aid_win[20], Asem_fi2[20], Asem_rebots[20], Anum_pilotes[20];
int Fi1id,final2, *fi2;
int *p_win;
int id_win;
char Aid_mis1[20], Aid_mis2[20], Aid_mis3[20], Aid_mis4[20], Aid_mis5[20], Aid_mis6[20], Aid_mis7[20], Aid_mis8[20], Aid_mis9[20];
/* funcio per carregar i interpretar el fitxer de configuracio de la partida */
/* el parametre ha de ser un punter a fitxer de text, posicionat al principi */
/* la funcio tanca el fitxer, i retorna diferent de zero si hi ha problemes  */
int carrega_configuracio(FILE * fit)
{
	fprintf(stderr,"Entro\n");
	int ret = 0;
	int i=0;//j=0;
	fscanf(fit, "%d %d %d\n", &n_fil, &n_col, &m_por);	/* camp de joc */
	fscanf(fit, "%d %d %d\n", &f_pal, &c_pal, &m_pal);	/* paleta */
	fprintf(stderr,"Pilotes =%d\n",n_pil);
	while(!feof(fit)&&i<MAXBALLS){//||(ret<=5&&ret>=0)){
		fprintf(stderr,"Entro bucle %d\n",i);
		fscanf(fit, "%f %f %f %f\n", &pos_f[i], &pos_c[i], &vel_f[i], &vel_c[i]);	/* pilota */
		
		//i++;
	
	if ((n_fil != 0) || (n_col != 0)) {	/* si no dimensions maximes */
		if ((n_fil < MIN_FIL) || (n_fil > MAX_FIL) || (n_col < MIN_COL) || (n_col > MAX_COL))
			ret = 1;
		else 
		if (m_por > n_col - 3)
			ret = 2;
        else if ((m_pal > n_col - 3) || (m_pal < 1) || (f_pal > n_fil-1) || (f_pal < 1) || (c_pal + m_pal > n_col -1 || c_pal < 1 )){
            ret = 3;
			fprintf(stderr,"Traza paso 3:\n");}
		//}
		//while(i<n_pil && ret != 3){
		else if ((pos_f[i] < 1) || (pos_f[i] >= n_fil - 3) || (pos_c[i] < 1)
			 || (pos_c[i] > n_col - 1))	/* tres files especials: línia d'estat, porteria i paleta */
			ret = 4;
	//}
	}
	fprintf(stderr,"Mig\n");
	if ((vel_f[i] < -1.0) || (vel_f[i] > 1.0) || (vel_c[i] < -1.0) || (vel_c[i] > 1.0))
		ret = 5;
	
	//i++;
	n_pil++;i++;
	}
	fprintf(stderr,"N_pil= %d\n",n_pil);
	
	if (ret != 0) {		/* si ha detectat algun error */
		fprintf(stderr, "Error en fitxer de configuracio:\n");
		switch (ret) {
		case 1:
			fprintf(stderr,
				"\tdimensions del camp de joc incorrectes:\n");
			fprintf(stderr, "\tn_fil= %d \tn_col= %d\n", n_fil,
				n_col);
			break;
		case 2:
			fprintf(stderr, "\tmida de la porteria incorrecta:\n");
			fprintf(stderr, "\tm_por= %d\n", m_por);
			break;
        case 3:
            fprintf(stderr,"\tmida de la paleta incorrecta:\n");
            fprintf(stderr, "\tf_pal= %d \tc_pal= %d \t m_pal=%d\n", f_pal,c_pal,m_pal);
            break;
		case 4:
			fprintf(stderr, "\tposicio de la pilota incorrecta:\n");
			fprintf(stderr, "\tpos_f= %.2f \tpos_c= %.2f\n", pos_f[i], pos_c[i]);
			break;
		case 5:
			fprintf(stderr,
				"\tvelocitat de la pilota incorrecta:\n");
			fprintf(stderr, "\tvel_f= %.2f \tvel_c= %.2f\n", vel_f[i],
				vel_c[i]);
			break;
		}
	}
	fclose(fit);
	fprintf(stderr,"Surto\n");
	return (ret);
}

/* funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
/* retorna diferent de zero si hi ha algun problema */
int inicialitza_joc(void)
{
	int i, retwin;
	int i_port, f_port;	/* inici i final de porteria */
	int c, nb, offset;
	FILE*fit=fopen("traza.txt","a");

	retwin = win_ini(&n_fil, &n_col, '+', INVERS);	/* intenta crear taulell */

	if (retwin < 0) {	/* si no pot crear l'entorn de joc amb les curses */
		fprintf(stderr, "Error en la creacio del taulell de joc:\t");
		switch (retwin) {
		case -1:
			fprintf(stderr, "camp de joc ja creat!\n");
			break;
		case -2:
			fprintf(stderr,
				"no s'ha pogut inicialitzar l'entorn de curses!\n");
			break;
		case -3:
			fprintf(stderr,
				"les mides del camp demanades son massa grans!\n");
			break;
		case -4:
			fprintf(stderr, "no s'ha pogut crear la finestra!\n");
			break;
		}
		return (retwin);
	}
	fprintf(stderr,"Inicialitza joc: \n");
	// Fi1id=ini_mem(sizeof(int));
	// fi1=(int *)map_mem(Fi1id);
	// *fi1=0;
	final2 = ini_mem(retwin);	/* Inicialitzem memoria compartida */
	fprintf(stderr,"\tfinal2= %d\n\t",final2);
	fi2 = (int *)map_mem(final2);			/* obtenir adres. de mem. compartida */
	fprintf(stderr,"\tfi2 mur3= %d\n",*fi2);
	*fi2 = 0;						/* inicialitza variable compartida */
	sprintf (Afin2, "%i", final2);	
	

	id_win = ini_mem(retwin);		/* crear zona mem. compartida */
	fprintf(stderr,"\tid_win= %d\n\t",id_win);
	p_win = (int *) map_mem(id_win);		/* obtenir adres. de mem. compartida */
	
	win_set(p_win,n_fil,n_col);		/* crea acces a finestra oberta */


	for (i = 0; i < n_pil ; i++)
	{
		id_mis[i] = ini_mis();	    /* crear una bustia IPC per cada pilota*/
		fprintf(stderr,"id_mis= %i\n",id_mis[i]);
	}

	sprintf (Aid_mis1, "%i", id_mis[0]);
	sprintf (Aid_mis2, "%i", id_mis[1]);
	sprintf (Aid_mis3, "%i", id_mis[2]);
	sprintf (Aid_mis4, "%i", id_mis[3]);
	sprintf (Aid_mis5, "%i", id_mis[4]);
	sprintf (Aid_mis6, "%i", id_mis[5]);
	sprintf (Aid_mis7, "%i", id_mis[6]);
	sprintf (Aid_mis8, "%i", id_mis[7]);
	sprintf (Aid_mis9, "%i", id_mis[8]);


	if (m_por > n_col - 2)
		m_por = n_col - 2;	/* limita valor de la porteria */
	if (m_por == 0)
		m_por = 3 * (n_col - 2) / 4;	/* valor porteria per defecte */

	i_port = n_col / 2 - m_por / 2 - 1;	/* crea el forat de la porteria */
	f_port = i_port + m_por - 1;
	for (i = i_port; i <= f_port; i++){
		win_escricar(n_fil - 2, i, ' ', NO_INV);
		
	}

	n_fil = n_fil - 1;	/* descompta la fila de missatges */

	for (i = 0; i < m_pal; i++)	/* dibuixar paleta inicial */{
		win_escricar(f_pal, c_pal + i, '0', INVERS);
		
	}
	for(i=0;i<n_pil;i++){
	/* generar la pilota */
	if (pos_f[i] > n_fil - 1)
		pos_f[i] = n_fil - 1;	/* limita posicio inicial de la pilota */
	if (pos_c[i] > n_col - 1)
		pos_c[i] = n_col - 1;
	f_pil[i] = pos_f[i];
	c_pil[i] = pos_c[i];		/* dibuixar la pilota inicialment */
	//win_escricar(f_pil[i], c_pil[i], '1'+i, INVERS);
	
	}
	// generar els blocs 
	nb = 0;
	nblocs = n_col / (BLKSIZE + BLKGAP) - 1;
	offset = (n_col - nblocs * (BLKSIZE + BLKGAP) + BLKGAP) / 2;
	for (i = 0; i < nblocs; i++) {
		for (c = 0; c < BLKSIZE; c++) {
			win_escricar(3, offset + c, FRNTCHAR, INVERS);
			nb++;	
			win_escricar(4, offset + c, BLKCHAR, NO_INV);		
			nb++;	
			win_escricar(5, offset + c, FRNTCHAR, INVERS);
			nb++;
		}
		offset += BLKSIZE + BLKGAP;
	}
	nblocs = nb / BLKSIZE;
	// generar les defenses
	nb = n_col / (BLKSIZE + 2 * BLKGAP) - 2;
	offset = (n_col - nb * (BLKSIZE + 2 * BLKGAP) + BLKGAP) / 2;
	for (i = 0; i < nb; i++) {
		for (c = 0; c < BLKSIZE + BLKGAP; c++) {
			win_escricar(6, offset + c, WLLCHAR, NO_INV);
		}
		offset += BLKSIZE + 2 * BLKGAP;
	}

	sprintf(strin,
		"Tecles: \'%c\'-> Esquerra, \'%c\'-> Dreta, RETURN-> sortir\n",
		TEC_ESQUER, TEC_DRETA);
	win_escristr(strin);
	win_update();
	fclose(fit);
	return (0);
}

/* funcio que escriu un missatge a la línia d'estat i tanca les curses */
void mostra_final(char *miss)
{	int lmarge;
	char marge[LONGMISS];

	/* centrar el misssatge */
	lmarge=(n_col+strlen(miss))/2;
	sprintf(marge,"%%%ds",lmarge);

	sprintf(strin, marge,miss);
	win_escristr(strin);

	/* espera tecla per a que es pugui veure el missatge */
	getchar();
}

/* funcio per moure la paleta segons la tecla premuda */
/* retorna un boolea indicant si l'usuari vol acabar */

void * mou_paleta(void * nul)
{
	//int tecla, result;
	int tecla;
	
	fprintf(stderr,"Paleta:\n");
do{
	//result = 0;
	tecla = win_gettec();
	if (tecla != 0) {
		if ((tecla == TEC_DRETA) && ((c_pal + m_pal) < n_col - 1)) {
				win_escricar(f_pal, c_pal, ' ', NO_INV);	/* esborra primer bloc */
				//pthread_mutex_unlock(&mutex);
				c_pal++;	/* actualitza posicio */
				//pthread_mutex_lock(&mutex);
				win_escricar(f_pal, c_pal + m_pal - 1, '0', INVERS);	/*esc. ultim bloc */
		}
		if ((tecla == TEC_ESQUER) && (c_pal > 1)) {
				win_escricar(f_pal, c_pal + m_pal - 1, ' ', NO_INV);	/*esborra ultim bloc */
				c_pal--;	/* actualitza posicio */
				win_escricar(f_pal, c_pal, '0', INVERS);	/* escriure primer bloc */
		}
		if (tecla == TEC_RETURN){
			(*fi1) = 1;
			}
				/* final per pulsacio RETURN */
		dirPaleta = tecla;	/* per a afectar al moviment de les pilotes */
	}
	//win_retard(retard);
	/*
	time(&temps_actual);
	win_retard(retard);
	double diferencia=difftime(temps_actual,temps_init);
	int minute=(int)diferencia/60;
	int seconds=(int)diferencia-minute*60;
	char temps[20];
	sprintf(temps,"%d : %d",minute,seconds);
	pthread_mutex_lock(&mutex);
	win_escristr(temps);
	pthread_mutex_unlock(&mutex);*/
	}while (!*fi1 && !*fi2);
	(*fi1)=1;
	return ((void*) 0);
}

/* programa principal */
int main(int n_args, char *ll_args[])
{
	time(&temps_init);
	FILE *file;
	file=fopen("traza.txt","w");
	
	int i;
	//n_pil=1;
	FILE *fit_conf;

	char avel_f[20], avel_c[20], af_pil[20], ac_pil[20], apos_f[20], apos_c[20];
	
	fprintf(stderr,"Traza 1\n");

	if ((n_args != 2) && (n_args != 3)) {	/* si numero d'arguments incorrecte */
		i = 0;	
		do
			fprintf(stderr, "%s", descripcio[i++]);	/* imprimeix descripcio */
		while (descripcio[i][0] != '*');	/* mentre no arribi al final */
		exit(1);
	}

	fit_conf = fopen(ll_args[1], "rt");	/* intenta obrir el fitxer */
	
	if (!fit_conf) {
		fprintf(stderr, "Error: no s'ha pogut obrir el fitxer \'%s\'\n",
			ll_args[1]);
		exit(2);
	}

	fprintf(stderr,"Traza 2\n");

	if (carrega_configuracio(fit_conf) != 0){	/* llegir dades del fitxer  */
		//fprintf(file,"Traza 3\n");
		fprintf(stderr,"Traza 3\n");
		exit(3);}	/* aborta si hi ha algun problema en el fitxer */
	
	fprintf(stderr,"Traza 4\n");

	fprintf(stderr,"Traza 5\n");

	if (n_args == 3) {	/* si s'ha especificat parametre de retard */
		retard = atoi(ll_args[2]);	/* convertir-lo a enter */
		if (retard < 10)
			retard = 10;	/* verificar limits */
		if (retard > 1000)
			retard = 1000;
	} else
		retard = 100;	/* altrament, fixar retard per defecte */

	printf("Joc del Mur: prem RETURN per continuar:\n");

	//getchar();
	int index = 0;

	if (inicialitza_joc() != 0)	/* intenta crear el taulell de joc */
		{
		fprintf(stderr,"Joc iniciat\n");
		exit(4);
		}	/* aborta si hi ha algun problema amb taulell */

	pthread_create(&idpa,NULL,mou_paleta,(void *) (intptr_t)1);
	sprintf(Aid_win,"%i", id_win);
	
	pthread_join(tpid[0],NULL);

	//pthread_create(&tid[1],NULL,mou_pilota,(void *) (intptr_t)index);
	for(int j=0;j<n_pil;j++)
	{
	    tpid[j] = fork(); 
		if (tpid[j] == 0)		/* Procés fill */
		{			/* Variables enviades per valor*/
			//Passem variables enlloc de llegirles un altre cop dins de la pilota
			snprintf (Anum_pil,sizeof(Anum_pil), "%i", (j+1));	
			snprintf (avel_f, sizeof(avel_f),"%f", vel_f[j]);			
			snprintf (avel_c,sizeof(avel_c), "%f", vel_c[j]);
			snprintf (af_pil,sizeof(af_pil), "%i", f_pil[j]);
			snprintf (ac_pil,sizeof(ac_pil), "%i", c_pil[j]);
			snprintf (apos_f,sizeof(apos_f), "%f", pos_f[j]);
			snprintf (apos_c,sizeof(apos_c), "%f", pos_c[j]);
			snprintf (An_col, sizeof(An_col),"%i", n_col);
			snprintf (An_fil, sizeof(An_fil),"%i", n_fil);
			snprintf(Asem_fi2,sizeof(Asem_fi2),"%i",*fi2);
			snprintf (Aretard, sizeof(Aretard),"%i", retard);
			snprintf (Anum_pilotes, sizeof(Anum_pilotes),"%i", n_pil);
			snprintf(Ac_pal,sizeof(Ac_pal),"%i",c_pal);
			snprintf(Am_pal,sizeof(Am_pal),"%i",m_pal);
            //fprintf(stderr, "\nPilota %s (abans de crear procés fill):\n%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s", Anum_pil, Avel_f, Avel_c, Af_pil, Ac_pil, Apos_f, Apos_c, An_col, An_fil, Aretard, Arebotes, Afin2, Aid_win);
			//fprintf(stderr,"Fi2= %d\n",*fi2);
			execlp("./pilota3", "pilota3", Anum_pil, avel_f, avel_c, af_pil, ac_pil, apos_f, apos_c, An_col, An_fil,
			 Aretard, Ac_pal, Afin2, Aid_win,Am_pal, Asem_fi2, Anum_pilotes, Aid_mis1, Aid_mis2, Aid_mis3, 
			 Aid_mis4, Aid_mis5, Aid_mis6, Aid_mis7, Aid_mis8, Aid_mis9, (char *)0);
			fprintf(stderr,"error: No s'ha pogut executar el process fill %i \n", j);
			exit(1);	/* Retornem error */
			/*codi fill */
		}
		else if (tpid[j] <  0 )			/* ERROR*/
		{
			fprintf(stderr, "Error en la creació de processos");
		}

	}
	for(i=0; i<n_pil; i++){                    //esperar finalización de los hijos
    //waitpid(tpid[i], NULL, 0);
    //elim_mis(bustia[i]);
  }
/********** bucle principal del joc **********/
	do {
	win_retard(retard); //retard del joc
	win_update();		//actualitza visuaització de curses
	} while (!*fi1 && !*fi2);
	fprintf(stderr,"Traza 6\n");
	elim_mem(final2);
	win_fi();		/* tanca les curses */
	fprintf(stderr,"Traza Final\n");

	if (nblocs == 0)
		mostra_final("YOU WIN !");
	else 
		mostra_final("GAME OVER");

	//pthread_mutex_destroy(&mutex);
fclose(stderr);
	return (0);		/* retorna sense errors d'execucio */
}
