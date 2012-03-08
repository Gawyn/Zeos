/*
 * sys.c - Syscalls implementation
 */

#define LECTURA		0
#define ESCRIPTURA 	1
#define ALTRES		2

#include <devices.h>
#include <errno.h>
#include <io.h>
#include <mm.h>
#include <utils.h>
#include <segment.h>
#include <sched.h>
#include <fd.h>
#include <zeosFat.h>
#include <sys.h>

#define TALLA 512

int properPID = 1;

int comprova_fd(int fd, int operacio)
{
	char modeCanal;
	if((fd < 0) || (fd > NCANALS)) return -ENXIO;
	modeCanal = current()->taula_canals[fd].mode;
	if(current()->taula_canals[fd].estat != OBERT) return -EBADF; //bad file descriptor
	if(operacio == ESCRIPTURA) 
	{ 
		if(modeCanal != O_WRONLY && modeCanal != O_RDWR) return -EBADF;
	}
	if(operacio == LECTURA) 
	{
		if(modeCanal != O_RDONLY && modeCanal != O_RDWR) return -EBADF;
	}
	return 1;

}

//Routine o crida al sistema sys_write
int sys_write(int fd,char *buffer, int size)
{
	int volta = 0;
	char buffer_local[TALLA];
	int midalocal;
	int res;

	volta = 0;
	midalocal = size;
	res = comprova_fd(fd, ESCRIPTURA);	

	if (size == 0) return 0; // no bytes to write
	if (size < 0) return -ERANGE; // Math result not representable
	if (res<0) return res;
	
	//Buffer NUL = Error No Such Device or Address
	if (!buffer) return -ENXIO;

	while(midalocal > TALLA)
	{
		//Omplim el buffer local
		copy_from_user(buffer+(volta * TALLA),buffer_local, TALLA);
		//Escribim la part del buffer local
		
		//Cridem la part del write dependent del dispositiu	
		current()->taula_canals[fd].descriptor->funcio_write(fd, buffer_local, TALLA);

		//sys_write_console(buffer_local, TALLA); ELIMINAT
		volta++;
		midalocal = midalocal - TALLA;
	}
	
	//Omplim buffer en ultima o única volta de 512
	copy_from_user(buffer + (volta * TALLA), buffer_local, midalocal);
	
	//Cridem la part del write dependent del dispositiu	
	current()->taula_canals[fd].descriptor->funcio_write(fd, buffer_local, midalocal);

	return size;
}

int sys_read(int fd, char *buffer, int size)
{
	int c, llegits;

	//Comprovem que size sigui enter positiu
	if(size < 0) return -EINVAL;

	//Comprovem que el punter al buffer no sigui nul
	if(!buffer) return -ENXIO;

	//Comprovem que el punter es trobi dins de l'espai de l'usuari
	if((unsigned int)buffer < (unsigned int)PH_USER_START) return -EFAULT; 

	//Comprovem el canal
	c = comprova_fd(fd, LECTURA);
	if (c<0) return c;

	//Cridem a la funció read específica
	llegits = current()->taula_canals[fd].descriptor->funcio_read(fd, buffer,size);

	return llegits;
}

int sys_ni_syscall()
{
		//Error Function Not Implemented
		return -ENOSYS; 
}

int sys_getpid()
{
	//Obtenim el pid del procés actual. Sempre hi haurà, almenys, el pare	
	struct task_struct* proces = current();
	return proces->pid;
}

//Busquem una posició lliure dins del vector task
int buscar_pos_lliure()
{
	int i;
	for(i=0;(i<NR_TASKS);i++) 
	{
		if(task[i].task.estat == DEAD) return i;
	}
	return -EAGAIN; //No suficient espai per nous processos
}

int sys_fork()
{
	int pos_lliure, pidFill, frame, page;			
	int i = 0;

	//Busquem una posició lliure pel nou procés al vector de tasques
	pos_lliure = buscar_pos_lliure();
	if(pos_lliure < 0) return -ENOMEM;

	//Busquem l'espai en memòria
	frame = alloc_frames(NUM_PAG_DATA); 
	if(frame < 0) return -ENOMEM;

	page = PAG_LOG_INIT_DATA_P0 + NUM_PAG_DATA;

	//Copiem la pàgina/task_union del pare al fill
	copy_data(current(), &task[pos_lliure], 4096);	

	//habilitem les entrades temporals de la taula de pagines
	for (i=0;i<NUM_PAG_DATA;i++) set_ss_pag(page+i,frame+i); 	

	//Copiem les entrades de la memòria lògica a la fisica
	copy_data((void*)(L_USER_START + NUM_PAG_CODE * PAGE_SIZE), (void*)(page << 12), PAGE_SIZE * NUM_PAG_DATA);

	// eliminem les entrades temporals
	for (i=0;i<NUM_PAG_DATA;i++) del_ss_pag(page+i);

	set_cr3(); //flush del TLB

	//Inicialitzem el task_struct del fill
	pidFill = properPID;
	task[pos_lliure].task.pid = pidFill;
	task[pos_lliure].task.ph_page = frame;
	task[pos_lliure].task.tics = 0;
	task[pos_lliure].task.estat = READY;
	task[pos_lliure].task.quantum=INIT_QUANTUM;

	//Augmentem el numRef dels fitxers oberts
	for(i = 0; i < NCANALS ;i++)
	{
		if(task[pos_lliure].task.taula_canals[i].estat == OBERT) 
			directori[task[pos_lliure].task.taula_canals[i].numFile].numRef++;
	}  

	//Afegim el proces a la llista de READY
	list_add(&(task[pos_lliure].task.tasks), &runqueue);
	
	//Per retornar el 0 al fill:
	//Posem el resultat del fork al registre eax, és la posicio KERNEL_STACK_SIZE menys
	// 10 perque eax es el desè registre començant per la cua
	task[pos_lliure].stack[KERNEL_STACK_SIZE - 10] = 0; 
	
	//Incrementem el num del PID		
	properPID++;
	
	return pidFill; //retornem el pid del fill al pare
}

int sys_close(int fd)
{
        int c;
        c = comprova_fd(fd, ALTRES);
	if (c<0) return c;
        
	if(current()->taula_canals[fd].descriptor->funcio_close != 0) 
		current()->taula_canals[fd].descriptor->funcio_close(fd);
	
	//Decrementem el num de referències a aquell fitxer
	directori[current()->taula_canals[fd].numFile].numRef--;
	current()->taula_canals[fd].estat = TANCAT;
        return 0;
}

//Matem el procés
void sys_exit()
{
	int pagina, i;

	//comprovem que no sigui el proces inicial
	if (current()->pid == 0) return; 
	
	struct task_struct *t = list_head_to_task_struct(list_next((list_first(&runqueue))));
	
	// Controlem que t no sigui un punter nul
	if(!t) return;

	// Actualitzem estat del procés que matem
	current()->estat = DEAD;

	//eliminem el proces de la runqueue
	int err = list_del(&(current()->tasks),&runqueue);
	if (err < 0) return;

	//Modifiquem el phys_mem
	pagina = current()->ph_page;
	free_frames(pagina, NUM_PAG_DATA);

	//Fem close de tots els canals oberts
	for(i = 0;i < NCANALS;i++) 
	{
		if(current()->taula_canals[i].estat == OBERT) sys_close(i);
	}

	//Passem a executar el següent procés de la cua
	task_switch((union task_union *) t);
}

//Modifiquem el quantum del procés actual
int sys_nice(int nouQuantum)
{
	int oldquantum;
	if(nouQuantum <= 0) return -EINVAL; 
	
	//Salvem el quantum anterior
	oldquantum=current()->quantum;

	//Modifiquem el quantum pel nou
	current()->quantum=nouQuantum;
	
	return oldquantum;
}


//Retorna els tics que el proces pid ha estat a la CPU
int sys_get_stats(int pid, int *tics)
{
	int i=0;
	int trobat = 0;
	if(pid < 0) return -ENXIO;
	if((int)tics < (int)PH_USER_START) return -EFAULT; 
	// else
	while( i<(int)NR_TASKS && !trobat)
	{
		if(task[i].task.pid == pid && task[i].task.estat != DEAD) trobat=1;
		i++;
	}
	i--;
	copy_to_user(&task[i].task.tics, tics, sizeof(int));
	if (i==NR_TASKS) return -ENXIO;
	return 0;
}

//Inicialitza el semafor n_sem amb el valor value
int sys_sem_init(int n_sem, unsigned int value)
{
	if(n_sem >= NUM_SEMS || n_sem < 0) return -ENXIO; //No existeix el semàfor n_sem
	if(value < 0) return -EINVAL; //Establim value minim de 0
	if(semafors[n_sem].ini == 1) return -EINVAL; 
	semafors[n_sem].ini = 1;
	semafors[n_sem].value = value;
	list_init(&(semafors[n_sem].l));
	return 0; //Tot OK
}

int sys_sem_wait(int n_sem)
{
	if (n_sem >= NUM_SEMS || n_sem < 0) return -ENXIO;  //No existeix el semàfor n_sem
	else if(semafors[n_sem].ini==0) return -EPERM;  
	//El procés 0 no es pot bloquejar en cap cas
	if(current()->pid==0) return -EPERM; 
	if(semafors[n_sem].value>0) semafors[n_sem].value--; 
	else
	{
		struct task_struct *t = list_head_to_task_struct(list_next(&(current()->tasks)));
		list_del(&(current()->tasks), &runqueue);
		list_add(&(current()->tasks), &semafors[n_sem].l);
		task_switch((union task_union *) t);
	}
	return 0;
}

int sys_sem_signal(int n_sem)
{
	if (n_sem >= NUM_SEMS || n_sem < 0) return -ENXIO; //No existeix el semàfor n_sem
	else if(semafors[n_sem].ini==0) return -EPERM; 
	//Si la llista està buida incrementem el value
	if(list_empty(&(semafors[n_sem].l))) semafors[n_sem].value++;
	//Sino agafem el primer de la llista del semàfor i el posem a la runqueue
	else 
	{
		struct list_head *l = list_first(&(semafors[n_sem].l));
		list_del(list_first(&(semafors[n_sem].l)),&(semafors[n_sem].l));
		list_add(l, &runqueue);
	}
	return 0;
}

int sys_sem_destroy(int n_sem)
{
	if(n_sem >= NUM_SEMS || n_sem <0) return -ENXIO; //No existeix el semàfor n_sem
	else if(semafors[n_sem].ini==0) return -EPERM; 
	if (!list_empty(&(semafors[n_sem].l))) return -EPERM; 
	semafors[n_sem].ini=0;
	return 0;
}

int buscar_canal_lliure()
{
	int i;
	for(i=0;i<NCANALS;i++)
        {
                if(current()->taula_canals[i].estat == TANCAT) return i;
	}
	return -ENOSPC; //No Space Left On Device
}

int sys_dup(int fd) 
{
	int c, i;
	c = comprova_fd(fd, ALTRES);
	if(c < 0) return c;
	i = buscar_canal_lliure();
	if(i < 0) return -EAGAIN; // Recursos temporalment esgotats :> Not Enough Channels
	current()->taula_canals[i].estat = OBERT;
	current()->taula_canals[i].mode = current()->taula_canals[fd].mode;
	current()->taula_canals[i].descriptor = current()->taula_canals[fd].descriptor;
	current()->taula_canals[i].numFile = current()->taula_canals[fd].numFile;
	current()->taula_canals[i].pos = current()->taula_canals[fd].pos;
	directori[current()->taula_canals[i].numFile].numRef++;
	if(current()->taula_canals[fd].descriptor->funcio_dup != 0) current()->taula_canals[fd].descriptor->funcio_dup(fd, i);
	return i;
}

int sys_open(const char *path, int flags) 
{
	int i, trobat,x;
	trobat = 0;
	for(i=0;i<NCANALS && !trobat;i++)
	{
		if(current()->taula_canals[i].estat == TANCAT)
		{
			trobat = 1;
			current()->taula_canals[i].descriptor=&disk;
			x=current()->taula_canals[i].descriptor->funcio_open(path, flags);
			//Recollim els errors especifics
			if (x<0) return x;
			current()->taula_canals[i].estat = OBERT;
		}	
	}
	return --i;
}


int sys_unlink(const char *path)
{
	int pos, bloc;
	
	//Eliminem la barra del directori
	if(path[0] == '/') path++;

	pos = existeixFitxer(path);
	if(pos == NO_TROBAT) return -ENOENT; // No Such File or Directory
	
	if(directori[pos].numRef > 0) return -EBUSY; //Device or Ressource Busy
	
	bloc = directori[pos].primerBloc;
	while(bloc != EOF)
	{		
		zeosfat[bloc].lliure = LLIURE;
		bloc = zeosfat[bloc].seguent;
	}
	
	directori[pos].size = 0;
	directori[pos].nom[0] = '\0';
	directori[pos].primerBloc = EOF;

	return 0;
}

int sys_readdir(struct dir_ent *buffer, int offset)
{
	int i;
	struct dir_ent entrada;
	if(offset < 0) return -EINVAL; //Invalid Argument
	if(!buffer) return -EFAULT; //Bad address
	
	if((unsigned int)buffer < (unsigned int)PH_USER_START) return -EFAULT; 
	
	for(i = 0;i < FILE_NAME_SIZE; i++) entrada.nom[i] = directori[offset].nom[i];
	entrada.size = directori[offset].size;
	
	*buffer = entrada;

	return 0;
}

void sys_open_kernel(const char *path, int fd, struct file_operations *fo)
{
	int trobat, i, k;

	task[0].task.taula_canals[fd].estat = OBERT;
        task[0].task.taula_canals[fd].descriptor = fo;
	trobat = 0;
	i=0;

	while(!trobat)
	{
		if(directori[i].nom[0] == '\0') trobat = 1;
		i++;
	}
	//Marquem una sola referència
	directori[--i].numRef = 1;
	//copiem el nom
	for(k=0; k < FILE_NAME_SIZE -1;k++) directori[i].nom[k] = path[k];
	
	directori[i].nom[k]='\0';

	task[0].task.taula_canals[fd].numFile = i;
	task[0].task.taula_canals[fd].pos = 0;
	if (fd == 0) task[0].task.taula_canals[fd].mode = O_RDONLY;
	else if (fd == 1) task[0].task.taula_canals[fd].mode = O_WRONLY;
	else if (fd == 2) task[0].task.taula_canals[fd].mode = O_WRONLY;
}

