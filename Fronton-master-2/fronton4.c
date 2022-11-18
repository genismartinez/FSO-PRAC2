/*****************************************************************************/
/*                                                                           */
/*                           Fronton0.c                                      */
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
/*	   $ gcc -c winsuport.c -o winsuport.o				     */
/*	   $ gcc fronton0.c winsuport.o -o fronton0 -lcurses		     */
/*                                                                           */
/*****************************************************************************/
//#FILA Y COLUMNA NO SE ACTUALIZAN pelota siempre valen 0
//#Que variables mas debemos sincronizar para que se vea bien por pantalla?
//#Condiciones de fin de juego
#include <stdio.h>	/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include "winsuport2.h"	/* incloure definicions de funcions propies */
#include <pthread.h>
#include "semafor.h"
#include "memoria.h"
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include "missatge.h"

#define MIDA_PALETA 4	/* definicions constants del programa */

			/* variables globals */
char *descripcio[]={
"\n",
"Aquest programa implementa una versio basica del joc del fronto:\n",
"generar un camp de joc rectangular amb una porteria, una paleta que s\'ha\n",
"de moure amb el teclat per a cobrir la porteria, i una Npil[0] que rebota\n",
"contra les parets del camp i contra la paleta. Quan la Npil[0] surt per la\n",
"porteria, el programa acaba la seva execucio. Tambe es pot acabar prement\n",
"la tecla RETURN. El joc consisteix en aguantar la Npil[0] el maxim temps.\n",
"\n",
"  Arguments del programa:\n",
"\n",
"       $ ./fronton0 fitxer_config [retard]\n",
"\n",
"     El primer argument ha de ser el nom d\'un fitxer de text amb la\n",
"     configuracio de la partida, on la primera fila inclou informacio\n",
"     del camp de joc (valors enters), i la segona fila indica posicio\n",
"     i velocitat de la Npil[0] (valors reals):\n", 
"          num_files  num_columnes  mida_porteria\n",
"          pos_fila   pos_columna   vel_fila  vel_columna\n",
"\n",
"     on els valors minims i maxims admesos son els seguents:\n",
"          6 < num_files     < 26\n",
"          9 < num_columnes  < 81\n",
"          0 < mida_porteria < num_files-2\n",
"        1.0 <= pos_fila     <= num_files-2\n",
"        1.0 <= pos_columna  <= num_columnes-1\n",
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
"*"};		/* final de la descripcio */

int MAXp = 0;

int n_fil, n_col;       /* numero de files i columnes del taulell */
int m_por;		/* mida de la porteria (en caracters) */
int f_pal, c_pal;       /* posicio del primer caracter de la paleta */
int retard=1000;		/* valor del retard de moviment, en mil.lisegons */

char strin[65];		/* variable per a generar missatges de text */

typedef struct{
  int f_pil;
  int c_pil;	/* posicio de la Npil[0], en valor enter */
  float pos_f;
  float pos_c;	/* posicio de la Npil[0], en valor real */
  float vel_f;
  float vel_c;	/* velocitat de la Npil[0], en valor real */
}pilota;

pilota Npil[9];


pthread_t tid;
pid_t pelotas[9]; 

int rebots;
int idCampo, idFI1, idFI2 ,idMin, idSeg, idFI3, rebots;// idTiempo;
int  *fi1, *fi2, *fi3, *min, *seg; // ,*dirCont, ;

int mutexpant, mutexfi;
int *bustia, idBustia;
int *cb, idCb;
//pthread_mutex_t mutexpant= PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mutexfi= PTHREAD_MUTEX_INITIALIZER;

/* funcio per carregar i interpretar el fitxer de configuracio de la partida */
/* el parametre ha de ser un punter a fitxer de text, posicionat al principi */
/* la funcio tanca el fitxer, i retorna diferent de zero si hi ha problemes  */
int carrega_configuracio(FILE *fit){
  int ret=0;
  
  fscanf(fit,"%d %d %d\n",&n_fil,&n_col,&m_por);	   /* camp de joc */
  while((!feof(fit)) && (MAXp<9)){
  fscanf(fit,"%f %f %f %f\n",&Npil[MAXp].pos_f,&Npil[MAXp].pos_c,&Npil[MAXp].vel_f,&Npil[MAXp].vel_c); /* Npil[0] */
  //printf("%f %f %f %f\n",Npil[MAXp].pos_f,Npil[MAXp].pos_c,Npil[MAXp].vel_f,Npil[MAXp].vel_c); //prueba de lectura OK
  if ((n_fil!=0) || (n_col!=0))			/* si no dimensions maximes */
  {
    if ((n_fil < 7) || (n_fil > 25) || (n_col < 10) || (n_col > 80))
	ret=1;
    else
    if (m_por > n_fil-3)
	ret=2;
    else
    if ((Npil[MAXp].pos_f < 1) || (Npil[MAXp].pos_f > n_fil-2) || (Npil[MAXp].pos_c < 1) || (Npil[MAXp].pos_c > n_col-1))
	ret=3;
  }
  if ((Npil[MAXp].vel_f < -1.0) || (Npil[MAXp].vel_f > 1.0) || (Npil[MAXp].vel_c < -1.0) || (Npil[MAXp].vel_c > 1.0))
  	ret=4;
  
  if (ret!=0)		/* si ha detectat algun error */
  {
    fprintf(stderr,"Error en fitxer de configuracio:\n");
    switch (ret)
    {
      case 1:	fprintf(stderr,"\tdimensions del camp de joc incorrectes:\n");
		fprintf(stderr,"\tn_fil= %d \tn_col= %d\n",n_fil,n_col);
		break;
      case 2:	fprintf(stderr,"\tmida de la porteria incorrecta:\n");
		fprintf(stderr,"\tm_por= %d\n",m_por);
		break;
      case 3:	fprintf(stderr,"\tposicio de la Npil[0] incorrecta:\n");
		fprintf(stderr,"\tNpil[0].pos_f= %.2f \tpos_c= %.2f\n",Npil[MAXp].pos_f, Npil[MAXp].pos_c);
		break;
      case 4:	fprintf(stderr,"\tvelocitat de la Npil[0] incorrecta:\n");
		fprintf(stderr,"\tNpil[0].vel_f= %.2f \tNpil[0].vel_c= %.2f\n",Npil[MAXp].vel_f,Npil[MAXp].vel_c);
		break;
     }
  }
  MAXp++;
  }
  fclose(fit);
  return(ret);
}


/*  funcio per inicialitar les variables i visualitzar l'estat inicial del joc */
/* retorna diferent de zero si hi ha algun problema */
int inicialitza_joc(void)
{
  void *dirCampo;
  int i, retwin;
  int i_port, f_port;		/* inici i final de porteria */

  retwin = win_ini(&n_fil,&n_col,'+',INVERS);	/* intenta crear taulell */ 
  if (retwin < 0)	/* si no pot crear l'entorn de joc amb les curses */
  { fprintf(stderr,"Error en la creacio del taulell de joc:\t");
    switch (retwin)
    {	case -1: fprintf(stderr,"camp de joc ja creat!\n");
    		 break;
	case -2: fprintf(stderr,"no s'ha pogut inicialitzar l'entorn de curses!\n");
		 break;
	case -3: fprintf(stderr,"les mides del camp demanades son massa grans!\n");
		 break;
	case -4: fprintf(stderr,"no s'ha pogut crear la finestra!\n");
		 break;
     }
     return(retwin);
  }
  
  mutexfi = ini_sem(1);     /* crear semáforo inicialmente abierto(1). Devuelve el semáforo  */ 
  mutexpant = ini_sem(1);
  
  //----------------------------------------------------------------------------------------
  //Creación del campo.
  //obtener direccion del campo
  idCampo = ini_mem(retwin);
  //mappeo de la direccion del campo || DirComp => direcció de la memoria compartida
  dirCampo = map_mem(idCampo); 
  //le pasamos a win_set la direccion del mapa guardado
  win_set(dirCampo, n_fil, n_col);
  
  
  //------------------------------------------------------------> Inicialización de la otra memoria compartida || Mapeo de las direcciones
  
  idFI1 = ini_mem(sizeof (int));
  fi1 = map_mem(idFI1);
  *fi1 = 0;
  
  
  idFI2 = ini_mem(sizeof(int));
  fi2 = map_mem(idFI2);
  *fi2 = MAXp;
  
  idFI3 = ini_mem(sizeof(int));
  fi3 = map_mem(idFI3);
  *fi3 = rebots;
  
  /*
  idRebots = ini_mem(sizeof(int));
  rebots = map_mem(idRebots);
  */
  
  idMin = ini_mem(sizeof(int));
  min = map_mem(idMin);
  
  idSeg = ini_mem(sizeof(int));
  seg = map_mem(idSeg);
  
  idBustia = ini_mem(sizeof(int[MAXp]));
  bustia = map_mem(idBustia);
  
  idCb = ini_mem(sizeof(int[MAXp]));            //mensaje auxiliar
  cb = map_mem(idCb);
  
  for(i=0; i<MAXp; i++){
    bustia[i] = ini_mis();
    cb[i] = 0;
  }
  //-------------------------------------------------------------
  
  
  if (m_por > n_fil-2)
	m_por = n_fil-2;	/* limita valor de la porteria */
  if (m_por == 0)
  	m_por = 3*(n_fil-2)/4;		/* valor porteria per defecte */

  i_port = n_fil/2 - m_por/2 -1;	/* crea el forat de la porteria */
  f_port = i_port + m_por -1;
  for (i = i_port; i <= f_port; i++)
	win_escricar(i,0,' ',NO_INV);

  n_fil = n_fil-1;		/* descompta la fila de missatges */

  f_pal = 1;			/* posicio inicial de la paleta per defecte */
  c_pal = 3;
  for (i=0; i< MIDA_PALETA; i++)       /* dibuixar paleta inicialment */
	win_escricar(f_pal+i,c_pal,'0',INVERS);

  for (i=0;i<MAXp;i++){
    if (Npil[i].pos_f > n_fil-1)
  	  Npil[i].pos_f = n_fil-1;	/* limita posicio inicial de la Npil[0] */
    if (Npil[i].pos_c > n_col-1)
  	  Npil[i].pos_c = n_col-1;
    Npil[i].f_pil = Npil[i].pos_f;
    Npil[i].c_pil = Npil[i].pos_c;			 /* dibuixar la Npil[0] inicialment */
    win_escricar(Npil[i].f_pil,Npil[i].c_pil,(i+'1'),INVERS);
  }
  sprintf(strin,"Tecles: \'%c\'-> amunt, \'%c\'-> avall, RETURN-> sortir\n",
  							TEC_AMUNT,TEC_AVALL);
  win_escristr(strin);
  
  //pthread_mutex_init(&mutexpant, NULL);
  //pthread_mutex_init(&mutexfi, NULL);
  
   
  
  
  
  
  
  return(0);
}






/* funcio per moure la paleta en segons la tecla premuda */
void * mou_paleta(void * null)
{
  int tecla; //result;
 
  waitS(mutexfi);//A
  do{
    signalS(mutexfi);//A
     //pthread_mutex_lock(&mutexpant);//F
  tecla = win_gettec();
    //pthread_mutex_unlock(&mutexpant);//F
  if (tecla != 0)
  {
    if ((tecla == TEC_AVALL) && ((f_pal+MIDA_PALETA)< n_fil-1))
    {
      waitS(mutexpant);//A
      win_escricar(f_pal,c_pal,' ',NO_INV);	/* esborra primer bloc */
    	f_pal++;				/* actualitza posicio */
	    win_escricar(f_pal+MIDA_PALETA-1,c_pal,'0',INVERS); /*esc. ultim bloc*/
      signalS(mutexpant);//A
    }
    if ((tecla == TEC_AMUNT) && (f_pal> 1))
    {
      waitS(mutexpant);//B
      win_escricar(f_pal+MIDA_PALETA-1,c_pal,' ',NO_INV); /*esborra ultim bloc*/
	    f_pal--;				/* actualitza posicio */
	    win_escricar(f_pal,c_pal,'0',INVERS);	/* escriure primer bloc */
	    signalS(mutexpant);//B
	  }
    if (tecla == TEC_RETURN){
      waitS(mutexfi);//B
      *fi1=1;		/* final per pulsacio RETURN */
      signalS(mutexfi);//B
    }
  }
  
  win_retard(retard);
  waitS(mutexpant);
  win_update();
  signalS(mutexpant);
  
  waitS(mutexfi);//A
  }while(*fi1==0 && *fi2!=0 && *fi3!=0);
  signalS(mutexfi);//A
  pthread_exit(0);
  
  return(0);
}



/* programa principal                               */
int main(int n_args, char *ll_args[])
{
  char a1[32],a2[32],a3[32],a4[32],a5[32],a6[32],a7[32],a8[32],a9[32],a10[32],a11[32],a12[32],a13[32],a14[32],a15[32],a16[32],a17[32], a18[32],a19[32],a20[32],a21[32];


  //TODO control de no pasar rebotes!.
  int i, n=0;
  FILE *fit_conf;
  int inici, final;
  
  inici=time(NULL);

  if ((n_args != 2) && (n_args != 3) && (n_args !=4))	/* si numero d'arguments incorrecte */
  { i=0;
    do fprintf(stderr,"%s",descripcio[i++]);	/* imprimeix descripcio */
    while (descripcio[i][0] != '*');		/* mentre no arribi al final */
    exit(1);
  }

  fit_conf = fopen(ll_args[1],"rt");		/* intenta obrir el fitxer */
  if (!fit_conf)
  {  fprintf(stderr,"Error: no s'ha pogut obrir el fitxer \'%s\'\n",ll_args[1]);
     exit(2);
  }

  if (carrega_configuracio(fit_conf) !=0)	/* llegir dades del fitxer  */
     exit(3);	/* aborta si hi ha algun problema en el fitxer */

  if (n_args >= 3) rebots = atoi(ll_args[2]); //fer atoi.
  
  if (n_args == 4)		/* si s'ha especificat parametre de retard */
  {	retard = atoi(ll_args[3]);	/* convertir-lo a enter */
  	if (retard < 10) retard = 10;	/* verificar limits */
  	if (retard > 1000) retard = 1000;
  }
  else retard = 100;		/* altrament, fixar retard per defecte */

  printf("Joc del Fronto: prem RETURN per continuar:\n"); getchar();

  if (inicialitza_joc() !=0)	/* intenta crear el taulell de joc */
     exit(4);	/* aborta si hi ha algun problema amb taulell */

  //do			/********** bucle principal del joc **********/
  //{	fi1 = mou_paleta();
  //for(z=0;z<MAXp;z++){
	//fi2 = mou_pilota(z);
  //}
	//win_retard(retard);		/* retard del joc */
  //} while (!fi1 && !fi2);

  
  sprintf(a1,"%i",retard);
  sprintf(a9,"%i",idCampo);
  sprintf(a10,"%i",idFI1);
  sprintf(a11,"%i",idFI2);
  sprintf(a12,"%i",idFI3);
  sprintf(a13,"%i",idMin);
  sprintf(a14,"%i",idSeg);
  sprintf(a15,"%i",rebots);
  sprintf(a16,"%i",mutexpant);
  sprintf(a17,"%i",mutexfi);
  sprintf(a18,"%i",n_fil);
  sprintf(a19,"%i",n_col);
  sprintf(a20,"%i",idBustia);
  sprintf(a21,"%i",idCb);
  
  for(i=0; i<MAXp; i++){
    pelotas[n] = fork();
    if(pelotas[n] == (pid_t) 0){
      
      sprintf(a2, "%i", Npil[n].f_pil);
      sprintf(a3, "%i", Npil[n].c_pil);   //Arguments passats per orde!
      sprintf(a4, "%lf", Npil[n].pos_f);
      sprintf(a5, "%lf", Npil[n].pos_c);
      sprintf(a6, "%lf", Npil[n].vel_f);
      sprintf(a7, "%lf", Npil[n].vel_c);
      sprintf(a8, "%i",n);
      execlp("./pilota4", "pilota4", a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21,(char *) 0);
      fprintf(stderr,"error: no puc executar el process fill \'pilota4\'\n");
      exit(0);

    }else if (pelotas[n] > 0) n++; /* branca del pare */
            
  }
  /*
  for (i=0; i<MAXp; i++){
    pthread_create(&tid[i], NULL, mou_pilota, (void **) (i));  //Cuando utilicemos el puntero bien -> (void **) (i)
  }*/
  pthread_create(&tid, NULL, mou_paleta, NULL); 
  
  do{
    win_retard(retard);
    waitS(mutexpant);
    win_update();
    signalS(mutexpant);
    
    waitS(mutexfi);
    final=time(NULL);
    *seg=difftime(final,inici);
    *min=*seg/60;
    *seg=*seg%60;
    sprintf(strin,"Rt: %d, Rr: %d, t:  %d:%d",rebots,*fi3,*min,*seg);
  	win_escristr(strin);
    signalS(mutexfi);
  }while (*fi1==0 && *fi2!=0 && *fi3!=0);
  
  
  pthread_join(tid, NULL);
  
  for(i=0; i<MAXp; i++){                    //esperar finalización de los hijos
    waitpid(pelotas[i], NULL, 0);
    elim_mis(bustia[i]);
  }
  
  
  elim_mem(idCampo);
  elim_mem(idFI1);
  elim_mem(idFI2);
  elim_mem(idFI3);
  elim_mem(idMin);
  elim_mem(idSeg);
  elim_mem(idBustia);
  elim_mem(idCb);
  
  elim_sem(mutexfi);
  elim_sem(mutexpant);
  
  win_fi();				/* tanca les curses */
  if (*fi2==0) printf("Final joc perque les pilotes han sortit per la porteria!\n\n");
  else{
    if (*fi3==0) printf("Final de joc perque s'han acabat els moviments!\n\n");
    else printf("Final joc perque s'ha premut RETURN!\n\n");
  }  
  
  printf("Temps total de joc %d minuts : %d segons \n", *min, *seg);
  return(0);			/* retorna sense errors d'execucio */
}




