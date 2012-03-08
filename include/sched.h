/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <fd.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024
#define NUM_SEMS 20
#define INIT_QUANTUM 15
#define RUN 1
#define DEAD 0
#define READY 2

struct task_struct {
 	struct list_head tasks;
   	int pid;
    	int estat; //Estat=0 :> Mort |  Estat=1 :> Running  | Estat=2 :> Ready
	int quantum;
  	int tics;
	int ph_page;
	struct canal taula_canals[NCANALS];
	int charsLlegits;
	int charsDemanats;
	char* bufferLectura;
};

struct semafor {
	int ini; 		//El semàfor està actiu
	int value; 		//El que val ara mateix els emàfor
	struct list l;
};

union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

extern union task_union task[NR_TASKS];
struct list runqueue;
struct list keyboardqueue;
struct semafor semafors[NUM_SEMS];

/* Inicialitza les dades del proces inicial */
void init_task0();
void initialize_task_union();
void init_sems();
struct task_struct* current();
struct task_struct* list_head_to_task_struct(struct list_head* l);
void task_switch(union task_union *t);
int nice(int quantum);
void actualitzaQuantum();
void init_keyqueue();

#endif  /* __SCHED_H__ */
