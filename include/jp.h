#ifndef __JP_H__
#define __JP_H__

#define ENTREGA1
#define ENTREGA2 
/*#define ENTREGA3*/


#define PERROR()	perror();
#define FINJP	    do {write(1,"FIN JP",6); while(1);} while(0)
#define CERROR(x)	if (x<0){ PERROR();FINJP;}
#define PRINTF(x)	CERROR(write(1,x,proso_strlen(x)))
#define CHECK( c, res, str ) do { \
    int k; \
    PRINTF( str ) ;\
    PRINTF( ":" ) ;\
    k = (c); \
    if ( k==(res) )  { PRINTF(": OK " );}else{ PRINTF(": ERROR " );} \
    if (( k==-1)&&(res!=-1)) {PRINTF(": "); PERROR();} \
    else PRINTF(" \n"); \
} while(0);


int jpWrite(void);

int jpProcessos(void);

void jpSemafors(void);

void joc_proves_sem_sincro();

void joc_proves_sem_mutex();

void jpRead();

void jpZeosFat();

#endif 
