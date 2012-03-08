/*
 * sched.c - initializes struct for task 0
 */

#include <sched.h>
#include <mm.h>
#include <segment.h>
#include <io.h>
#include <sys.h>
#include <fd.h>
#include <zeosFat.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

struct list runqueue;
int flagEOI=0;
struct list keyboardqueue;

void init_task0()
{
	int i = 0;

 	// inicialitzem dades
	task[0].task.pid = 0;
	task[0].task.estat = RUN; //Hem de buscar un valor per a inicialitzar l'estat
	task[0].task.quantum = INIT_QUANTUM; //Hem de buscar un valor per a inicialitzar el quantum
	task[0].task.tics = 0;	
	
	//Encuem a la runqueue el procés inicial
	list_init(&runqueue);
	
	list_add(&(task[0].task.tasks), &runqueue);
	
	//Inicialitzem la pàgina física del procés 0
	task[0].task.ph_page = NUM_PAG_KERNEL + NUM_PAG_CODE;
	
	//Obrim els canals basics del sistema
	sys_open_kernel("teclat", 0, &teclat); //stdin
        sys_open_kernel("screen", 1, &pantalla); //stdout
        sys_open_kernel("syslog", 2, &pantalla); //stderr

	//Tanquem la resta de canals
	for(i=3;i<NCANALS;i++) task[0].task.taula_canals[i].estat = TANCAT;
}

void init_keyqueue()
{
	list_init(&keyboardqueue);
}

void init_sems()
{	
	//Inicialitzem tots els semàfors a No Incialitzat
	int i;
	for (i=0;i<NUM_SEMS;i++) semafors[i].ini=0;
}

//Matem tots els processos del vector task
void initialize_task_union()
{
	int i;
	for(i=0;i<NR_TASKS;i++)
	{
		task[i].task.pid = -1;
		task[i].task.estat = DEAD;
		task[i].task.tics = 0;
	}
}

struct task_struct* current()
{
	int direccio;

	__asm__ __volatile__(
			"movl %%esp, %0\n"
			: "=g" (direccio)
			:
			: "memory"
	);

	//Posem a 0 els 3 bytes de la part d'abaix per anar al principi de la pàgina. És a dir, al task_struct
	return (struct task_struct*)(direccio&0xFFFFF000); 
}

struct task_struct* list_head_to_task_struct(struct list_head* l)
{
	//l'adreça del task_struct es la mateixa que la del list_head, ja que es el primer camp de l'struct
	return (struct task_struct*)l;
}

void task_switch(union task_union *t)
{
	int page;
	int frame;
	int esp;
	int aux;
	int i=0;
	
	//Comprovem que no sigui un procés que s'intenti canviar per si mateix
	if(current()->pid == t->task.pid) return;

	//actualitzem la tss perque apunti a la pila de sistema de t
	//He deduit que era el camp esp0 de la tss mirant el contingut
	//Al inicialitzar la tss, esp0 té KERNEL_ESP que està definida així:
 	// #define KERNEL_ESP (DWord) &task[0].stack[KERNEL_STACK_SIZE]
	tss.esp0 =(DWord) &(t->stack [KERNEL_STACK_SIZE]);

	// Actualitzem la taula de pagines
	page = PAG_LOG_INIT_DATA_P0;
	frame = t->task.ph_page;
	for (i=0;i<NUM_PAG_DATA;i++) set_ss_pag(page+i,frame+i);
	
	// fem un flush al TLB
	set_cr3();


	// Actualitzem l'estat
	t->task.estat = RUN; //Running

	// rectifiquem l'esp a la pila del nou proces
	esp = (int)&t->stack[KERNEL_STACK_SIZE-16];

	aux = flagEOI;
	flagEOI = 0;

	//Actualitzem la pila del nou procés
	//En funció de flagEOI farem també l'EOI
	__asm__ __volatile__(
		"movl %0, %%esp\n"
		"movl %1, %%ebx\n"
		"cmpl $0, %%ebx\n"
		"je no_cal_eoi\n"
	
		//Codi EOI
		"movb $0x20, %%al\n"
		"outb %%al, $0x20\n"

		"no_cal_eoi:\n"
		"popl %%ebx\n"
		"popl %%ecx\n"
		"popl %%edx\n"
		"popl %%esi\n"
		"popl %%edi\n"
		"popl %%ebp\n"
		"popl %%eax\n"
		"popl %%ds\n"
		"popl %%es\n"
		"popl %%fs\n"
		"popl %%gs\n"
		"iret\n"
		: 
		: "g" (esp), "g" (aux)
	);

}

void actualitzaQuantum()
{
	int x=current()->quantum;
	current()->quantum=x-1;
	current()->tics++;

	if(x==0)
	{
		struct task_struct *t = list_head_to_task_struct(list_next(list_first(&runqueue)));
		
		 //eliminem el proces de la runqueue
     		int err = list_del(&(current()->tasks),&runqueue);
       		if (err < 0) return; //error de llistes (impossible a priori)
		
		err = list_add(&(current()->tasks),&runqueue);
	       	if (err < 0) return; //error de llistes (impossible a priori)
		
		current()->quantum = INIT_QUANTUM;
		//Canviem l'estat del procés que surt
		current()->estat = READY; //Ready

		//Venim d'una interrupció de rellotge i, com que no tornarem, hem d'avisar al sistema que cal fer un EOI
		flagEOI = 1;
		task_switch((union task_union *) t);
	}
}
