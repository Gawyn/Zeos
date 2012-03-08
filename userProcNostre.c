#include <libc.h>
#include <jp.h>
#include <zeosFat.h>

int proso_strlen( char *buffer ) 
{
    int i=0;
    for(;buffer[i];i++);
    return i;
}

int proso_itoa(int num, char*buffer)
{
    int a;
    if (num > 9) a=proso_itoa(num/10,buffer);
    else a=0;
    buffer[a]=num%10+48;
    return a+1;
}

void jpSemafors()
{
	int x,y;
	PRINTF("\n ################## JOC DE PROVES DE SEMAFORS ################\n");
   	PRINTF("Prova del sem_init\n");
	CHECK(sem_init(-1, 1), -1, "Provem error d'inicialitzar semafor amb n_sem negatiu");
	CHECK(sem_init(0, 0), 0, "Prova d'inicialitzar semafor 0 a 0");
   	CHECK(sem_init(0, 1), -1, "Prova de doble inicialitzacio del semafor 0");
	PRINTF("Prova d'errors simples del sem_destroy\n");
	CHECK(sem_destroy(-1), -1, "Provem de destruir un semafor amb n_sem negatiu");
	CHECK(sem_destroy(3), -1, "Provem de destruir un semafor no inicialitzat");
 	CHECK(sem_destroy(0), 0, "Provem de destruir un semafor inicialitzat i sense processos pendents");
	PRINTF("Prova d'errors simples del sem_wait\n");
	CHECK(sem_wait(-1), -1, "Provem error al fer un wait sobre un semafor amb n_sem negatiu");
	CHECK(sem_wait(9), -1, "Provem de fer un wait sobre semafor no inicialitzat");
	CHECK(sem_wait(0), -1, "Provem de fer un wait sobre el P0, l'actual");
	PRINTF("Prova d'errors simples del sem_signal\n");
	CHECK(sem_signal(-1), -1, "Provem de fer un signal sobre un semafor amb n_sem negatiu");
	CHECK(sem_signal(19), -1, "Provem de fer un signal sobre un semafor no inicialitzat");
	

	PRINTF("Tots els errors simples de semafors provats. Comprovem ara el funcionament basic del sem_signal i sem_wait\n");
	PRINTF("\nLa sequencia d'esdeveniments hauria d'estar ordenada:");

	//Deixem el P0 parat per sempre
	x=fork();
	if(x==1) while(1);
	
	sem_init(0,0);

	x=fork();

	if(x!=0) //El pare
	{
		PRINTF("1. El pare escriu i es queda en espera\n");
		sem_wait(0);
		PRINTF("3. El pare hauria d'fer write aixo despres que el fill mori\n");
	} 
	else //El fill
	{
		PRINTF("2. El fill escriu quelcom, fa un signal i mor\n");
		sem_signal(0);
		exit();
	}

	CHECK(sem_destroy(0), 0, "Destruim el semafor anterior");

	PRINTF("Provem un funcionament mes complex amb 3 processos\n");
	PRINTF("La sequencia d'esdeveniments hauria d'estar ordenada: \n");

	sem_init(15,0);
	x=fork();
	if (x==0)
	{ //fill
		y=fork();
		if(y==0)
		{ //net
			CHECK(sem_destroy(15), -1, "Comprovem que no ens deixa destruir un semafor amb processos pendents");
			PRINTF("1. Soc el net, l'ultim en arribar pero el primer en fer write. Envio un isignal i moro\n");
			sem_signal(15);
			exit();
		}
		sem_wait(15);
		PRINTF("3. Soc el fill, gracies a que el pare espera, escric, envio signal i moro \n");
		sem_signal(15);
		exit();
	} 
	else //pare 
	{
		sem_wait(15);
		PRINTF("2. Soc el pare, he esperat al meu net, i ara enviare un signal i esperare al meu fill \n");
		sem_signal(15);
		sem_wait(15);
		PRINTF("4. Soc el pare, he esperat a tothom i ara ja puc morir tranquil\n");
		exit();	
	}

}

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
		CHECK( get_stats(mypid, (int*)1000), -1, "Probando get_stats @tics=1000");
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


void jpRead()
{
	PRINTF("\n**********INICI JOC DE PROVES PROPI LECTURA************\n");
	 
	int aux=0;
	char buffer[40];
	if(!fork())
	{
		// Provem de llegir menys del tamany del buffer circular amb el proces bloquejat
		PRINTF("Escriu 5 lletres:\n");
		read(0, buffer, 5);
		PRINTF("Lectura finalitzada\n");
		write(1, buffer, 5);
		PRINTF("\n");

		// Provem de llegir menys del tamany del buffer circular, pero amb caracteres introduits
		// al buffer circular abans de fer el read 
		PRINTF("Escriu 4 lletres:\n");
		for(aux = 0; aux < 3000000; aux++); //Creem un bucle d'espera
		PRINTF("Ha acabat el bucle d'espera");
		read(0, buffer, 4);
		PRINTF("Ha acabat de llegir.\n");
		write(1, buffer, 4);
		PRINTF("\n");

		// Provem de llegir mes del tamany del buffer circular amb el proces bloquejat
		PRINTF("Escriu 40 lletres:\n");
		read(0, buffer, 40);
		PRINTF("Ha acabat de llegir.\n");
		write(1, buffer, 40);
		PRINTF("\n");

		// Probem de llegir mes del tamany del buffer circular, pero amb caracters introduits 
		// al buffer circular abans de fer el read
		PRINTF("Escriu 40 lletres:\n");
		for(aux = 0; aux < 3000000; aux++);
		PRINTF("Ha acabat el bucle d'espera");
		read(0, buffer, 40);
		PRINTF("Ha acabat de llegir\n");
		write(1, buffer, 40);
		PRINTF("\n");

		// Probem dup i close i errors de canals amb read i write
		int fdr, fdw;
		fdw = dup(1);
		if(fdw != -1) write(fdw, "Canal duplicat OK\n", 19);

		aux = close(1);
		if(aux == 0) aux=write(1, buffer, 4);
		if(aux == -1) write(fdw, "S'ha tancat be.\n", 16);
		aux = close(1);
		if(aux == -1) write(fdw, "No es pot tancar un canal tancat.\n", 34);

		fdr = dup(0);
		aux = write(fdr, "NO\n", 3);
		if(aux==-1) write(fdw, "No es pot fer write en un canal de lectura.\n", 43);

		aux = read(fdw, buffer, 3);
		if(aux == -1) write(fdw, "No es pot llegir en un canal de escriptura.\n", 44);

		close(fdr);
		aux=read(fdr, buffer, 3);
		if (aux == -1) write(fdw, "No es pot llegir d'un canal tancat.\n", 36);

		write(fdw, "**********FINAL***********\n", sizeof("**********FINAL***********\n"));
	}
	while(1);
}

void jpZeosFat()
{
	int op,op1,op2;
	struct dir_ent buf[10];
	int ent, w, r, u;
	char buffer[1512];
	PRINTF("\n**********************INICI JOC DE PROVES PROPI ZEOSFAT***********************\n");
	PRINTF("Intentem crear 3 fitxers amb flags incorrectes:\n");
	op = open("fitxer2",O_RDONLY|O_WRONLY);
	perror();
	op = open("/fitxer1",O_RDWR | O_CREAT);
	if(op<0) PRINTF("\nOP INCORRECTE\n");
	op1 = open("fitxer1\0",O_CREAT);
	PRINTF("Hauria de donar error: ");
	if (op1 < 0){
		PRINTF("OK ");
		perror();
	}

	//LListem el fitxer del teclat	
	ent = readdir(buf, 0);
	llista_fitxers(buf, 1);
	

	w = write(op,"Projecte de Sistemes Operatius",30);
	
	
	op2 = open("fitxer1",O_RDONLY);
	if(op2<0) PRINTF("\nOP2 INCORRECTE\n");
	r = read(op2,buffer,50);
	
	PRINTF("Contingut del fitxer1: ");
	w = write(1,buffer,r);
	w = write(1,"\n",1);



	PRINTF("1a part del fitxer1: ");
	op2 = open("fitxer1",O_RDONLY);
	r = read(op2,buffer,15);
	w = write(1,buffer,r);
	w = write(1,"\n",1);

	PRINTF("2a part del fitxer1: ");
	r = read(op2,buffer,15);
	w = write(1,buffer,r);
	w = write(1,"\n",1);

	ent = readdir(buf,3);
	llista_fitxers(buf,1);
	

	close(op2);
	op2 = open("fitxer1",O_WRONLY);
	write(op2,"Facultat de informatica",23);
	
	PRINTF("Sobreescribim els primers 23 bytes del fitxer1 i tornem a llegir-lo:\n");
	close(op2);
	op2 = open("fitxer1",O_RDONLY);
	r = read(op2,buffer,30);
	write(1,buffer,30);
	w = write(1,"\n",1);


	PRINTF("Creem un arxiu i escrivim mes d'un bloc\n");
	close(op);
	op = open("fitxer2",O_CREAT | O_WRONLY);
	w = write (op,"En un lugar de la Mancha, de cuyo nombre no quiero acordarme, no ha mucho tiempo que vivía un hidalgo de los de lanza en astillero, adarga antigua, rocín flaco y galgo corredor. Una olla de algo más vaca que carnero, salpicón las más noches, duelos y quebrantos los sábados, lantejas los viernes, algún palomino de añadidura los domingos, consumían las tres partes de su hacienda. El resto della concluían sayo de velarte, calzas de velludo para las fiestas, con sus pantuflos de lo mesmo, y los días de entresemana se honraba con su vellorí de lo más fino. Tenía en su casa una ama que pasaba de los cuarenta y una sobrina que no llegaba a los veinte, y un mozo de campo y plaza que así ensillaba el rocín como tomaba la podadera. Frisaba la edad de nuestro hidalgo con los cincuenta años. Era de complexión recia, seco de carnes, enjuto de rostro, gran madrugador y amigo de la caza. Quieren decir que tenía el sobrenombre de «Quijada», o «Quesada», que en esto hay alguna diferencia en los autores que deste caso escriben, aunque por conjeturas verisímiles se deja entender que se llamaba «Quijana». Pero esto importa poco a nuestro cuento: basta que en la narración dél no se salga un punto de la verdad. Es, pues, de saber que este sobredicho hidalgo, los ratos que estaba ocioso —que eran los más del año—, se daba a leer libros de caballerías, con tanta afición y gusto, que olvidó casi de todo punto el ejercicio de la caza y aun la administración de su hacienda; y llegó a tanto su curiosidad y desatino en esto, que vendió muchas hanegas de tierra de sembradura para comprar libros de caballerías en que leer, y, así, llevó a su casa todos cuantos pudo haber dellos; y, de todos, ningunos le parecían tan bien como los que compuso el famoso Feliciano de Silva, porque la claridad de su prosa y aquellas entricadas razones suyas le parecían de perlas, y más cuando llegaba a leer aquellos requiebros y cartas de desafíos, donde en muchas partes hallaba escrito: «La razón de la sinrazón que a mi razón se hace, de tal manera mi razón enflaquece, que con razón me quejo de la vuestra fermosura». Y también cuando leía: «Los altos cielos que de vuestra divinidad divinamente con las estrellas os fortifican y os hacen merecedora del merecimiento que merece la vuestra grandeza...»", 1000);

	close(op);
	op = open("fitxer2",O_RDONLY); 
	r = read(op,buffer, 500);
	PRINTF("Printem els 500 primers bytes de fitxer2\n");
	w = write(1,buffer,r);
	write(1,"\n",1);
	

	r = read(op,buffer,150);
	PRINTF("\nPrintem els 150 bytes restants\n");
	w = write(1,buffer,r);
	write(1,"\n",1);

	PRINTF("Intentem esborrar el fitxer2, pero te referencies\n");
	unlink("fitxer2");
	PRINTF("Error del unlink: ");
	perror();
	PRINTF("\n\n");

	ent = readdir(buf, 4);
	llista_fitxers(buf, 1);
	
	PRINTF("Tanquem el canal i l'intentem esborrar ara\n");
	close(op);
	if (unlink("fitxer2") >= 0 ) PRINTF("Fitxer2 esborrat\n\n");
	perror();
	
	PRINTF("\nNo hauria de sortir cap fitxer en aquest list:\n");
	ent = readdir(buf, 4);
	llista_fitxers(buf, 1);

	PRINTF("Provem amb el fork, creem un fitxer3\n");
	op = open("fitxer3",O_WRONLY|O_CREAT);
	write(op,"Soc el pare, em sents fill?\n",25);

	switch (fork()){
		case 0: PRINTF("Soc el fill\n");
			if(write(op,"Si que et sento, pare", 21) < 0)
				perror();
			PRINTF("Escric en el fitxer aprofitant el canal del pare despres el llegeixo:\n");
			op2=open("fitxer3",O_RDWR);
			r=read(op2,buffer,40);
			w = write(1,buffer,r);
			PRINTF("\nTanco els canals i esborro fitxer3: ");
			close(op2);
			close(op);
			u = unlink("fitxer3");
			if (u >= 0){
				PRINTF("OK\n");

			}
			
			PRINTF("Creem un fitxer4 per comprovar que l'exit tanca els canals\n");
			op2 = open("fitxer4",O_CREAT|O_RDWR);
			 
			ent = readdir(buf, 4);
			llista_fitxers(buf, 1);
			if(fork() == 0)
			{
				PRINTF("Soc el net i intento esborrar el fitxer4 creat per el fill: ");		
				close(op2);
				u = unlink("fitxer4"); 
				if(u == 0)PRINTF("OK");
				if(u < 0)perror();
				PRINTF("\nHauria d'estar buit:\n");
				ent = readdir(buf, 4);
				llista_fitxers(buf, 1);
				PRINTF("******************************FINAL JP************************************\n");
				exit();
			}
			PRINTF("Em suicido\n");
			exit();
			break;

		case -1: PRINTF("Error en el fork\n");
			 break;
			
		default: PRINTF("Soc el pare i tanco el canal\n");
			 close(op);

			 PRINTF("Intento esborrar el fitxer\n Error del unlink:");
			 if (unlink("fitxer3") < 0) perror(); 
			
		   	 ent = readdir(buf, 4);
			 llista_fitxers(buf,1);


			break;
	}



	while(1);
}

int calcula_long(char *car)
{
	int i=0;
	char * c = car;
	while (*c != '\0')
	{
		c = c+1;	
		i++;

	}
	return i;
}


void llista_fitxers(struct dir_ent *buf, int ent)
{
	PRINTF("\nLlista de fitxers: \n");
	int i;
	for (i=0;i<ent;i++)
	{	
		PRINTF("Nom: ");
		write(1,buf[i].nom,calcula_long(buf[i].nom));
		PRINTF(" Tamany: ");
		char baf[4];
		baf[0] = ' ';
		baf[1] = ' ';
		baf[2] = ' ';
		baf[3] = ' ';
		proso_itoa(buf[i].size,baf);
		write(1,baf,4);
		PRINTF(" bytes \n");
	}
	
}


long inner (long n)
{
	int i;
	long suma;
	suma = 0;
	for (i=0;i<n;i++) suma = suma + i;
	return suma;
}

long outer (long n)
{
	int i;
	long acum;
	acum = 0;
	for (i=0;i<n;i++) acum = acum + inner(i);
	return acum;
}

int add (int par1, int par2)
{
	
	//1a versio
	__asm__ __volatile__ (
		"movl 8(%ebp),%ebx \n"
		"movl 12(%ebp),%ecx \n"
		"addl %ebx,%ecx \n"
		"movl %ecx,%eax "
	);
	
	//2a versio
	int suma;
	__asm__ __volatile__ (
		"movl 8(%%ebp),%%ebx \n"
		"movl 12(%%ebp),%%ecx \n"
		"addl %%ebx,%%ecx \n"
		"movl %%ecx,%0"
		: "=g" (suma)
	);
	return suma;
}


int Pract0()
{
	long count, acum;
  	count = 75;
	acum = 0;
	acum = outer(count);
	
	//int suma = add (3,5);

//prova de l'excepcio general_protection
	 __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); 

//prova de l'excepcio divide_error
	//int a = 3/0;

	while (1){}
	return 0;
}


