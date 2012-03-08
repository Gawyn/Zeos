#include <libc.h>
#include <jp.h>

#define PERROR()	perror();
#define FINJP	while(1)
#define CERROR(x)	if (x<0){ PERROR();FINJP;}
#define PRINTF(x)	CERROR(write(1,x,strlen(x)))

int strlen( char *buffer ) {
    int i=0;
    for(;buffer[i];i++);
    return i;
}

#define CHECK( c, res, str ) do { \
    int k; \
    PRINTF( str ) ;\
    PRINTF( ":" ) ;\
    k = (c); \
    if ( k==(res) )  { PRINTF(": OK " );}else{ PRINTF(": ERROR " );} \
    if (( k==-1)&&(res!=-1)) {PRINTF(": "); PERROR();} \
    else PRINTF(" \n"); \
} while(0);

int jpWrite(void)
{
	int mypid,pid,ret;
	char buffer[16];
	int i;
	int j;

        for(j=0; j<80*35; j++) write(1," ",1);
	PRINTF("\n****JP: Write***********************\n");
	PRINTF("\nSALUDAMOS:(deberia salir HOLA):");
	write(1,"HOLA\n",5);

#ifdef ENTREGA1
    CHECK( write(2, "HOLA", 4),    -1, "Probando canal incorrecto(2)");
    CHECK( write(1, "HOLA", -3),   -1, "Probamos size negativo (-3)");
    CHECK( write(1, "HOLA\0HOLA",9), 9, "Probamos a poner un \0");
    CHECK( write(1, "TESTING...", 0),   0, "Probamos valor de retorno OK(size=0)");
    CHECK( write(1, "TESTING...", 5),   5, "Probamos valor de retorno OK(size<size cadena)");
    CHECK( write(1, "TESTING...", 10), 10, "Probamos valor de retorno OK(size>0)");
    CHECK( write(1, "Esto no deberia salir\n", 0),   0, "Probamos con size=0");
    CHECK( write(1, "Esta es una prueba de una cadena larga que no la pongo toda igual para asegurarme que lo que estoy copiando esta bien , no se cuanto ocupa pero me da igual\n", 100), 100, "Probamos size cadena > size");
//    CHECK( write(1, "A VER SI FALLA", 4096), 4096, "Probamos con size>KERNEL_STACK_SIZE");

    CHECK( write(1, (char*)0, 100),-1, "Probamos @buffer=0");
#endif

#ifdef ENTREGA2
    CHECK( write(1, (char*)1000, 100), -1, "Probamos @buffer=1000");
#endif

	PRINTF("\n**************FIN JP****************\n");
  	while(1); 
}
