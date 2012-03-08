#include <libc.h>
#include <jp.h>

int proso_strlen( char *buffer ) {
    int i=0;
    for(;buffer[i];i++);
    return i;
}

int proso_itoa(int num, char*buffer){
    int a;
    if (num > 9){
        a=proso_itoa(num/10,buffer);
    }
    else{
        a=0;
    }
    buffer[a]=num%10+48;
    return a+1;
}

//int  __attribute__((__section__(".text.main")))
int jpProcessos(void)
{
	int mypid,pid,ret;
	char buffer[16];
	int i;
	int j;
	int n;
	int tics;
	for(j=0; j<80*35; j++) write(1," ",1);
	PRINTF("\n****JP: Procesos********************\n");
	PRINTF("Probamos getpid\n");
	mypid=getpid();
	proso_itoa(mypid,buffer);
	PRINTF("PID PADRE: ");
	PRINTF(buffer);
	PRINTF("\nCreacion primer proceso \n");
	pid=fork();
	if (pid==0){
		PRINTF("HOLA SOY EL HIJO\n");
		proso_itoa(getpid(),buffer);
		PRINTF("El pid del hijo es ");
		PRINTF(buffer);
		PRINTF("\n");
		PRINTF("Soy el hijo y ME SUICIDO\n");
		exit();
	}else if (pid==-1){
		PERROR();
		FINJP;
	}else{
		PRINTF("SOY EL PADRE: Creacion primer proceso OK\n");
	}
	PRINTF("Creamos una jerarquia: un hijo y un nieto\n");
	switch(fork()){
		case 0:switch(fork()){
			       case 0:PRINTF("SOY EL NIETO: ");
				      proso_itoa(getpid(),buffer);
				      PRINTF(buffer);
				      PRINTF("\n");
			       	      PRINTF("SOY EL NIETO y me suicido ");
				      exit();
			       case -1:FINJP;
			       default:PRINTF("SOY EL HIJO: ");
				       proso_itoa(getpid(),buffer);
				       PRINTF(buffer);
				       PRINTF("\n");
			       	       PRINTF("SOY EL HIJO y me suicido ");
				       exit();
				       break;
		       }
		case -1:FINJP;break;
		default:break;
	}
	PRINTF("Creo N procesos concurrentes\n");
	for (i=0;i<4;i++){
		if (getpid()==mypid)	CERROR(fork());
	}
	for(i=0;i<100;i++){
		buffer[0]='A'+(getpid()%4);
		CERROR(write(1,buffer,1));
	}
	PRINTF("Dejo solo uno\n");
	if (getpid()!=mypid){
		PRINTF("SOY ");proso_itoa(getpid(),buffer);
		PRINTF(buffer);PRINTF(" Y Me suicido\n");
		exit();
	}
	PRINTF("Solo queda el proceso ");proso_itoa(getpid(),buffer);
	PRINTF(buffer);

	PRINTF(" y Deberia ser 0\n");
	CHECK( nice(-1),    -1, "Probando nice incorrecto(-1)");
	CHECK( nice(0),     -1, "Probando nice incorrecto(0)");
	PRINTF("Probando nice correcto(10):");
	n = nice(10);
	if (n < 0) {PRINTF("ERROR"); PERROR();}
	else {
		CHECK( nice(5),    10, "Probando nice correcto(5)");
		CHECK( get_stats(-1, &tics),    -1, "Probando get_stats pid incorrecto(-1)");
		CHECK( get_stats(mypid, (int*)0),    -1, "Probando get_stats @tics=0");
		CHECK( get_stats(mypid, (int*)10009999999999999999999), -1, "Probando get_stats @tics=1000");
		CHECK( get_stats(mypid, &tics), 0, "Probando get_stats OK");

	}

	PRINTF("Creamos N procesos\n");
	i=0;
#if 1
	while(((ret=fork())>0)&& (i<100)){
		PRINTF("Creo 1 proceso\n");
		i++;
	}
#endif
	PRINTF("Matamos todos los procesos excepto el primero\n");
	if (getpid()!=mypid)	exit();

	proso_itoa(i,buffer);
	PRINTF("Hemos creado ");PRINTF(buffer);PRINTF("procesos\n");

	PRINTF("\n**************FIN JP**************** PID: ");
				       proso_itoa(getpid(),buffer);
				       PRINTF(buffer);
				       PRINTF("\n");
	while(1); 
}
