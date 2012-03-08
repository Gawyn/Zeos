/*
 * libc.c 
 */

#include <libc.h>
#include <errno.h>
#include <jp.h>
int errno;
 

void perror()
{
	write(2, "\n", 1);	
	switch(errno) 
	{
		case -EBADF:
			write(2, "Bad file number", sizeof("Bad file number"));
			break;
		case -EFAULT:
			write(2, "Bad adress", sizeof("Bad adress"));
			break;
		case -ENOSYS:
		        write(2,"Function not implemented", sizeof("Function not implemented"));
		        break;
		case -ERANGE:
		        write(2,"Math result not representable", sizeof("Math result not representable"));
			break;
	     	case -EINVAL:
		        write(2,"Invalid argument",17);
		        break;            
		case -ENXIO:
			write(2,"No such device or address", sizeof("No such device or address"));
			break;
		case ENXIO:
			write(2,"No such device or address", sizeof("No such device or address"));
			break;
		case -EPERM:
			write(2,"Operation not permited", sizeof("Operation not permited"));
			break;
		case -ENOMEM:
			write(2,"Not enough memory", sizeof("Not enough memory"));
			break;
		case -ENOENT:
			write(2,"No such file or directory", sizeof("No such file or directory"));
			break;
		case -ENOSPC:
			write(2,"No space left on device", sizeof("No space left on device"));
			break;
		case -EBUSY:
			write(2,"Device or ressource busy", sizeof("Device or ressoure busy"));
			break;
		default:
			write(2,"Unknown error",14);
	}
	write(2, "\n", 1);
}

/* Wrapper of  write system call*/
int write(int fd,char *buffer,int size)
{
	int resultat;
	__asm__ __volatile__(
		"pushl %%ebx\n"		//; Guardem contingut registres
		"pushl %%ecx\n"
		"pushl %%edx\n"
		"movl %3, %%edx\n"	//; %edx = size
		"movl %2, %%ecx\n"	//; %ecx = buffer
		"movl %1, %%ebx\n"	//; %ebx = fd
		"movl $4, %%eax\n"	//; Posem 4 a l'%eax per l'operacó write
		"int $0x80\n"		//; crida interrupció sys_call
		"movl %%eax, %0\n"	//; resultat = %eax
		"popl %%edx\n"		//; Restaurem contingut anterior dels registres
		"popl %%ecx\n"
		"popl %%ebx\n"
     	: "=g" (resultat)
     	: "g" (fd), "g" (buffer), "g" (size)
	  	: "ax", "cx", "bx", "dx", "memory"
	);

	/* processar el resultat de la crida */
	if (resultat>=0)
		return resultat;	//Torna els bytes escrits si tot va bé
	else { 
		errno=resultat;
		//perror();
		return -1;		//Retorna -1 si ha fallat
	}
}

/* Wrapper of read system call*/
int read(int fd,char *buffer,int size)
{
	int resultat;
	__asm__ __volatile__(
		"pushl %%ebx\n"		//; Guardem contingut registres
		"pushl %%ecx\n"
		"pushl %%edx\n"
		"movl %3, %%edx\n"	//; %edx = size
		"movl %2, %%ecx\n"	//; %ecx = buffer
		"movl %1, %%ebx\n"	//; %ebx = fd
		"movl $3, %%eax\n"	//; Posem 3 a l'%eax per l'operacó read
		"int $0x80\n"		//; crida interrupció sys_call
		"movl %%eax, %0\n"	//; resultat = %eax
		"popl %%edx\n"		//; Restaurem contingut anterior dels registres
		"popl %%ecx\n"
		"popl %%ebx\n"
     	: "=g" (resultat)
     	: "g" (fd), "g" (buffer), "g" (size)
	  	: "ax", "cx", "bx", "dx", "memory"
	);
	
	/* processar el resultat de la crida */
	if (resultat>=0)
		return resultat;	//Torna els bytes llegits si tot va bé
	else { 
		errno = resultat;
		//perror();
		return -1;
	}
}

/* Wrapper of open system call*/
int open(const char *path, int flag)
{
        int resultat;
        __asm__ __volatile__(
                "pushl %%ebx\n"         //; Guardem contingut registres
                "pushl %%ecx\n"
                "movl %2, %%ecx\n"      //; %ecx = flag
                "movl %1, %%ebx\n"      //; %ebx = *path
                "movl $5, %%eax\n"      //; Posem 5 a l'%eax per l'operacó read
                "int $0x80\n"           //; crida interrupció sys_call
                "movl %%eax, %0\n"      //; resultat = %eax
                "popl %%ecx\n"
                "popl %%ebx\n"
        : "=g" (resultat)
        : "g" (path), "g" (flag)
        : "ax", "cx", "bx", "memory"
        );

        /* processar el resultat de la crida */
        if (resultat>=0)
                return resultat;        //Torna els bytes llegits si tot va bé
        else {
                errno = resultat;
                //perror();
                return -1;
        }
}

/* Wrapper del Close */
int close(int fd)
{
	__asm__ __volatile__("etiquetaE:");
        int resultat;
        __asm__ __volatile__(
                "pushl %%ebx\n"         //; Guardem contingut registres
                "movl %1, %%ebx\n"      //; %ebx = fd
                "movl $6, %%eax\n"     //; Posem 6 a l'%eax per l'operacó close
                "int $0x80\n"           //; crida interrupció sys_call
		"etiquetaF0:"
                "movl %%eax, %0\n"      //; resultat = %eax
                "popl %%ebx\n"
        : "=g" (resultat)
        : "g" (fd)
                : "ax", "bx", "memory"
        );

        /* processar el resultat de la crida */
        if (resultat==0)
	{
                return resultat;
	}        
        else 
	{
                errno=resultat;
                return -1;              //Retorna -1 si ha fallat
        }
}
        
/* Wrapper del close() */    
int dup(int fd)
{
        int resultat;
        __asm__ __volatile__(
                "pushl %%ebx\n"         //; Guardem contingut registres
                "movl %1, %%ebx\n"      //; %ebx = fd
                "movl $41, %%eax\n"     //; Posem 41 a l'%eax per l'operacó dup
                "int $0x80\n"           //; crida interrupció sys_call
                "movl %%eax, %0\n"      //; resultat = %eax
                "popl %%ebx\n"
        : "=g" (resultat)
        : "g" (fd)
                : "ax", "bx", "memory"
        );

        /* processar el resultat de la crida */
        if (resultat>0)
                return resultat;
        else {
                errno=resultat;
                //perror();
                return -1;              //Retorna -1 si ha fallat
        }
}


/* Wrapper del getpid()  */
int getpid()
{
	int resultat;

	__asm__ __volatile__(
		"movl $20, %%eax\n"	//; Posem 4 a l'%eax per l'operacó sys_getpid()
		"int $0x80\n"		//; crida interrupció sys_call
		"movl %%eax, %0\n"	//; resultat = %eax
     	: "=g" (resultat)
     	: 
		: "ax", "memory"
	);
	
	/* processar el resultat de la crida */
	if (resultat>=0)
		return resultat;	//Torna els bytes escrits si tot va bé
	else 
	{ 
		errno=resultat;
		//perror();
		return -1;		//Retorna -1 si ha fallat
	}
}


/* Wrapper del fork()  */
int fork()
{
	int resultat;

	__asm__ __volatile__(
		"movl $2, %%eax\n"	//; Posem 2 a l'%eax per l'operacó sys_fork()
		"int $0x80\n"		//; crida interrupció sys_call
		"movl %%eax, %0\n"	//; resultat = %eax
     	: "=g" (resultat)
     	: 
		: "ax", "memory"
	);
	
	/* processar el resultat de la crida */
	if (resultat>=0)
		return resultat;
	else { 
		errno=resultat;
		//perror();
		return -1;		//Retorna -1 si ha fallat
	}
}

/* Wrapper del exit()  */
void exit(void)
{
	__asm__ __volatile__(
			"movl $1,%eax\n"	//posar num de crida a sistema
			"int $0x80\n"		//trap
			"movl %ebp,%esp\n"
			"popl %ebp\n"
			"ret;"
	);		
}

/* Wrapper of  nice system call*/
int nice(int nouQuantum)
{
	int resultat;
	__asm__ __volatile__(
		"pushl %%ebx\n"		//; Guardem contingut registres
		"movl %1, %%ebx\n"	//; %ebx = quantum
		"movl $34, %%eax\n"	//; Posem 4 a l'%eax per l'operacó nice
		"int $0x80\n"		//; crida interrupció sys_call
		"movl %%eax, %0\n"	//; quantum = %eax
		"popl %%ebx\n"
     	: "=g" (resultat)
     	: "g" (nouQuantum)
	  	: "ax", "bx", "memory"
	);
	
	/* processar el resultat de la crida */
	if (resultat>=0)
		return resultat;	//Torna el quantum si tot va bé
	else { 
		errno=resultat;
		//perror();
		return -1;		//Retorna -1 si ha fallat
	}
}


//Imprimeix per pantalla un int
void printi(int num)
{
        char str[10];
        int i = 0;
	//fem itoa() mentre no arribem al final del num
        str[0] = (char)(num % 10 + 48);
        while (num /= 10) str[++i]=(char)(num % 10 + 48);
	//fem write del resultat convertit
        for (i=i;i>=0;i--) write(1,&str[i],1);
}

/* Wrapper of getstats system call*/
int get_stats(int pid, int *tics)
{
	int resultat;

    __asm__ __volatile__(
        "pushl %%ebx\n"        //; Guardem contingut registres
        "movl %1, %%ebx\n"    //; %ebx = pid
        "movl %2, %%ecx\n"    //; %ecx = tics 
        "movl $35, %%eax\n"   //; Posem el 35 a l'eax per l'operacio de get_stats
        "int $0x80\n"         //; crida interrupció sys_call
        "movl %%eax, %0\n"    //; resultat = %eax
        "popl %%ebx\n"
         : "=g" (resultat)
         : "g" (pid), "g" (tics)
         : "ax","bx","cx", "memory"
    );
    
    // processar el resultat de la crida 
    if (resultat>=0)
        return resultat;    //Torna els tics si tot va bé
    else {
        errno=resultat;
        //perror();
       return -1;        //Retorna -1 si ha fallat
    }
}

int sem_init(int n_sem, unsigned int value)
{
	int resultat;
	 __asm__ __volatile__(
		"pushl %%ebx\n"         //; Guardem contingut registres
		"movl %2, %%ecx\n"
		"movl %1, %%ebx\n"
		"movl $21, %%eax\n"
		"int $0x80\n"
		"movl %%eax, %0\n"
        	"popl %%ebx\n"
		: "=g" (resultat)
		: "g" (n_sem), "g" (value)
		: "ax", "bx", "cx", "memory"
	); 

 /* processar el resultat de la crida */
    if (resultat>=0)
        return resultat; 
    else {
        errno = resultat;
        //perror();
       return -1;        //Retorna -1 si ha fallat
    }
}

int sem_wait(int n_sem)
{
	int resultat;
	 __asm__ __volatile__(
               "pushl %%ebx\n"         //; Guardem contingut registres
		"movl %1, %%ebx\n"
		"movl $22, %%eax\n"
		"int $0x80\n"
		"movl %%eax, %0\n"
        	"popl %%ebx\n"
		: "=g" (resultat)
		: "g" (n_sem) 
		: "ax", "bx", "memory"
	); 

 /* processar el resultat de la crida */
    if (resultat>=0)
        return resultat; 
    else {
        errno = resultat;
        //perror();
       return -1;        //Retorna -1 si ha fallat
    }
}


int sem_signal(int n_sem)
{
	int resultat;
	 __asm__ __volatile__(
            	"pushl %%ebx\n"         //; Guardem contingut registres

		"movl %1, %%ebx\n"
		"movl $23, %%eax\n"

		"int $0x80\n"
		"movl %%eax, %0\n"
        	
		"popl %%ebx\n"

		: "=g" (resultat)
		: "g" (n_sem) 
		: "ax", "bx", "memory"
	); 

 /* processar el resultat de la crida */
    if (resultat>=0)
        return resultat; 
    else {
        errno = resultat;
        //perror();
       return -1;        //Retorna -1 si ha fallat
    }
}


int sem_destroy(int n_sem)
{
	int resultat;
	 __asm__ __volatile__(
                "pushl %%ebx\n"         //; Guardem contingut registres

		"movl %1, %%ebx\n"
		"movl $24, %%eax\n"

		"int $0x80\n"
		"movl %%eax, %0\n"

        	"popl %%ebx\n"

		: "=g" (resultat)
		: "g" (n_sem) 
		: "ax", "bx", "memory"
	); 

 /* processar el resultat de la crida */
    if (resultat>=0)
        return resultat; 
    else {
        errno = resultat;
        //perror();
       return -1;        //Retorna -1 si ha fallat
    }
}



/* Wrapper del Unlink */
int unlink(const char *path)
{
        int resultat;
        __asm__ __volatile__(
                "pushl %%ebx\n"         //; Guardem contingut registres
                "movl %1, %%ebx\n"      //; %ebx = fd
                "movl $10, %%eax\n"     //; Posem 10 a l'%eax per l'operacó close
                "int $0x80\n"           //; crida interrupció sys_call
                "movl %%eax, %0\n"      //; resultat = %eax
                "popl %%ebx\n"
        : "=g" (resultat)
        : "g" (path)
                : "ax", "bx", "memory"
        );

        /* processar el resultat de la crida */
        if (resultat>=0)
                return resultat;        
        else {
                errno=resultat;
        	//perror();
                return -1;              //Retorna -1 si ha fallat
        }
}


/* Wrapper of  readdir system call*/
int readdir(struct dir_ent *buffer, int offset)
{
	int resultat;
	__asm__ __volatile__(
		"pushl %%ebx\n"		//; Guardem contingut registres
		"pushl %%ecx\n"
		"movl %2, %%ecx\n"	//; %ecx = offset
		"movl %1, %%ebx\n"	//; %ebx = buffer
		"movl $141, %%eax\n"	//; Posem 141 a l'%eax per l'operacó write
		"int $0x80\n"		//; crida interrupció sys_call
		"movl %%eax, %0\n"	//; resultat = %eax
		"popl %%ecx\n"
		"popl %%ebx\n"
     	: "=g" (resultat)
     	: "g" (buffer), "g" (offset)
	  	: "ax", "cx", "bx", "memory"
	);
	
	/* processar el resultat de la crida */
	if (resultat>=0)
		return resultat;	//Torna els bytes escrits si tot va bé
	else { 
		errno=resultat;
		//perror();
		return -1;		//Retorna -1 si ha fallat
	}
}

