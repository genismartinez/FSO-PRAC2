#include <stdio.h>	/* incloure definicions de funcions estandard */
#include <stdlib.h>
#include <string.h>
#include "winsuport2.h"	/* incloure definicions de funcions propies */
#include "semafor.h"
#include "memoria.h"
#include <time.h>
#include "missatge.h"

#define MIDA_PALETA 4	/* definicions constants del programa */

char strin[65];
void *dirCampo;
int  *fi1, *fi2, *fi3, *min, *seg;
int *bus, *cb;

/* funcio per moure la Npil[0]: retorna un 1 si la Npil[0] surt per la porteria,*/
/* altrament retorna un 0 */
int main(int n_args, char * ll_args[]){    
  
  int retard = atoi(ll_args[1]);
  int f_pil = atoi(ll_args[2]);
  int c_pil = atoi(ll_args[3]);
  float pos_f = atof(ll_args[4]);
  float pos_c = atof(ll_args[5]);
  float vel_f = atof(ll_args[6]);
  float vel_c = atof(ll_args[7]);
  int index = atoi(ll_args[8]); 
  int idCampo = atoi(ll_args[9]);
  int idFI1 = atoi(ll_args[10]);
  int idFI2 = atoi(ll_args[11]);
  int idFI3 = atoi(ll_args[12]);
  int idMin = atoi(ll_args[13]);
  int idSeg = atoi(ll_args[14]);
  int rebots = atoi(ll_args[15]);              /*Passar argumentos als semafors*/
  int mutexpant = atoi(ll_args[16]);
  int mutexfi = atoi(ll_args[17]);
  int n_fil = atoi(ll_args[18]) +1;
  int n_col = atoi(ll_args[19]);
  int idBustia = atoi(ll_args[20]);
  int idCb = atoi(ll_args[21]);
    
  
  int i = (int) index; //Para cuando utilicemos el puntero bien
  //int i = index;
  int f_h, c_h, fip=0;
  char rh,rv,rd,a,b;
  
  int inici, final, aux;
  float m[4], n[3];
  
  //------------------------------------------------------------> Inicialización de la otra memoria compartida || Mapeo de las direcciones
  dirCampo = map_mem(idCampo); 
  win_set(dirCampo, n_fil, n_col);

  fi1 = map_mem(idFI1);
  fi2 = map_mem(idFI2);
  fi3 = map_mem(idFI3);
  min = map_mem(idMin);
  seg = map_mem(idSeg);
  bus = map_mem(idBustia);
  cb = map_mem(idCb);
  //-------------------------------------------------------------
  
  
  inici=time(NULL);
  waitS(mutexfi);//A
  do{
    if (cb[i] != 0){
      receiveM(bus[i],&m);
      fprintf(stderr,"%d\n1: %f \n2: %f", i,m[1], vel_f);
      if (m[0] == 1 || m[0] == 3){
        n[1] = vel_f;
        vel_f = -m[1];
        m[1] = 0;
      }
      if (m[0] == 2 || m[0] == 3){
        n[2] = vel_c;
        vel_c = -m[2];
        m[2] = 0;
      }
      n[0] = m[0];
      cb[i] = 0;
      aux = m[3];
      sendM(bus[aux],&n,sizeof(float[3]));
      n[1] = 0;
      n[2] = 0;
    }
    
    signalS(mutexfi);//A
    
    f_h = pos_f+vel_f;		/* posicio hipotetica de la Npil[0] (entera) */
    c_h = pos_c+vel_c;
   // result = 0;			/* inicialment suposem que la Npil[0] no surt */
    rh = rv = rd = ' ';
    if ((f_h != f_pil) || (c_h != c_pil))
    {		/* si posicio hipotetica no coincideix amb la posicio actual */
      if (f_h != f_pil) 		/* provar rebot vertical */
      {	

        waitS(mutexpant);//A       
        rv = win_quincar(f_h,c_pil);	/* veure si hi ha algun obstacle */
  	    
  	    if (rv != ' ')			/* si hi ha alguna cosa */
  	    {   vel_f = -vel_f;		/* canvia sentit velocitat vertical */
  	        f_h = pos_f+vel_f;		/* actualitza posicio hipotetica */ //Mensaje control de choque con pelota.
  	        if (rv == '0'){
  	          waitS(mutexfi);
  	          (*fi3)--;
  	          final=time(NULL);
              *seg=difftime(final,inici);
              *min=*seg/60;
              *seg=*seg%60;
              sprintf(strin,"Rt: %d, Rr: %d, t: %d:%d",rebots,*fi3,*min,*seg);
              signalS(mutexfi);
  	          win_escristr(strin);
  	         }
  	         if (rv != '0' && rv != '+'){
  	           m[0] = 1;
  	           m[1] = vel_f;
  	           m[3] = i;
  	           sendM(bus[((int)rv)-49],&m,sizeof(float[4]));
  	           cb[((int)rv)-49] = 1;
  	           
  	           signalS(mutexpant);
  	           receiveM(bus[i],&m);
  	           waitS(mutexpant);
  	           if (m[0] == 1 || m[0] == 3){
                vel_f = m[1];
                m[1] = 0;
               }
               if (m[0] == 2 || m[0] == 3){
                vel_c = m[2];
                m[2] = 0;
               }
  	         }
  	    }
  	    signalS(mutexpant);//A
      }
      
      if (c_h != c_pil) 		/* provar rebot horitzontal */
      {	
        waitS(mutexpant);//B
        rh = win_quincar(f_pil,c_h);	/* veure si hi ha algun obstacle */

      	if (rh != ' ')			/* si hi ha algun obstacle */
  	    {    vel_c = -vel_c;		/* canvia sentit vel. horitzontal */
  	         c_h = pos_c+vel_c;		/* actualitza posicio hipotetica */  //Mensaje control de choque con pelota.
  	         if (rh == '0'){
  	           waitS(mutexfi);
  	          (*fi3)--;
  	          final=time(NULL);
              *seg=difftime(final,inici);
              *min=*seg/60;
              *seg=*seg%60;
              sprintf(strin,"Rt: %d, Rr: %d, t: %d:%d",rebots,*fi3,*min,*seg);
              signalS(mutexfi);
  	          win_escristr(strin);
  	         }
  	         if (rh != '0' && rh != '+'){
  	           m[0] = 2;
  	           m[2] = vel_c;
  	           m[3] = i;
  	           sendM(bus[((int)rh)-49],&m,sizeof(float[4]));
  	           cb[((int)rh)-49] = 1;
  	           
  	           signalS(mutexpant);
  	           receiveM(bus[i],&m);
  	           waitS(mutexpant);
  	           if (m[0] == 1 || m[0] == 3){
                vel_f = m[1];
                m[1] = 0;
               }
               if (m[0] == 2 || m[0] == 3){
                vel_c = m[2];
                m[2] = 0;
               }
  	         }
  	    }
  	    signalS(mutexpant);//B
      }
      
      if ((f_h != f_pil) && (c_h != c_pil))	/* provar rebot diagonal */
      {	
        waitS(mutexpant);//C
        rd = win_quincar(f_h,c_h);

  	    if (rd != ' ')				/* si hi ha obstacle */
  	    {    vel_f = -vel_f; vel_c = -vel_c;	/* canvia sentit velocitats */
  	         f_h = pos_f+vel_f;
  	         c_h = pos_c+vel_c;		/* actualitza posicio entera */ //Mensaje control de choque con pelota.
  	         if (rd == '0'){
  	          waitS(mutexfi);
  	          (*fi3)--;
  	          final=time(NULL);
              *seg=difftime(final,inici);
              *min=*seg/60;
              *seg=*seg%60;
              sprintf(strin,"Rt: %d, Rr: %d, t: %d:%d",rebots,*fi3,*min,*seg);
              signalS(mutexfi);
  	          win_escristr(strin);
  	         }
  	         if (rd != '0' && rd != '+'){
  	           m[0] = 3;
  	           m[1] = vel_f;
  	           m[2] = vel_c;
  	           m[3] = i;
  	           sendM(bus[((int)rd)-49],&m,sizeof(float[4]));
  	           cb[((int)rd)-49] = 1;
  	           
  	           signalS(mutexpant);
  	           receiveM(bus[i],&m);
  	           waitS(mutexpant);
  	           if (m[0] == 1 || m[0] == 3){
                vel_f = m[1];
                m[1] = 0;
               }
               if (m[0] == 2 || m[0] == 3){
                vel_c = m[2];
                m[2] = 0;
               }
  	         }
  	       if ((vel_f > 0 && vel_c > 0) || (vel_f < 0 && vel_c < 0)){ //Comprobar rebot trible
    	       a = win_quincar(f_h+1,c_h+1);
    	       b = win_quincar(f_h-1,c_h-1);
  	       }
  	       else{
  	         a = win_quincar(f_h-1,c_h+1);
    	       b = win_quincar(f_h+1,c_h-1);
  	       }
  	       if ((a!='0' && a!='+' && a!=' ') && (b!='0' && b!='+' && b!=' ')){
  	         if (vel_f > vel_c){
  	           vel_c = 0;  
  	         }
  	         else{
  	           vel_f = 0;
  	         }

  	         b = ' ';
  	       }
  	    }
  	    signalS(mutexpant);//C
      }
      
      waitS(mutexpant);//D
      if (win_quincar(f_h,c_h) == ' ')	/* verificar posicio definitiva */
      {					/* si no hi ha obstacle */

  	      win_escricar(f_pil,c_pil,' ',NO_INV);  	/* esborra Npil[0] */

  	      pos_f += vel_f; pos_c += vel_c;
          f_pil = f_h; c_pil = c_h;		/* actualitza posicio actual */
  	     
  	      if (c_pil > 0){		 		/* si surt del taulell, */
  		      win_escricar(f_pil,c_pil,(i+'1'),INVERS); /* imprimeix Npil[0] */
  	      }
  	     
  	      else{
  		      waitS(mutexfi);//B
            (*fi2)--; 
            fip = 1;
            signalS(mutexfi);//B
  	      }
      }
      signalS(mutexpant);//D
    }
    else { pos_f += vel_f; pos_c += vel_c; }
     
     //printf("F: %f || C: %f",Npil[i].pos_f, Npil[i].pos_c); Prueba de posicion pelota

  	
  win_retard(retard);
  waitS(mutexpant);
  win_update();
  signalS(mutexpant);
  waitS(mutexfi);//A
  }while(*fi1==0 && fip==0 && *fi3!=0);
  signalS(mutexfi);//A
 
  return(0);
  
}
