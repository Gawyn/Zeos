/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <entry.h>
#include <io.h>
#include <errno.h>
#include <utils.h>
#include <sched.h>
#include <sys.h>
#include <bufCirc.h>
#include <mm.h>

Gate idt[IDT_ENTRIES];
Register    idtR;

time temps = {0,0,0,0};

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}
void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
  setInterruptHandler(0,divide_error_handler,0);
  setInterruptHandler(1,debug_handler,3);
  setInterruptHandler(2,NMI_handler,0);
  setInterruptHandler(3,breakpoint_handler,3);
  setInterruptHandler(4,overflow_handler,0);
  setInterruptHandler(5,bounds_check_handler,0);
  setInterruptHandler(6,invalid_opcode_handler,0);
  setInterruptHandler(7,device_not_available_handler,0);
  setInterruptHandler(8,double_fault_handler,0);
  setInterruptHandler(9,coprocessor_segment_overrun_handler,0);
  setInterruptHandler(10,invalid_tss_handler,0);
  setInterruptHandler(11,segment_not_present_handler,0);
  setInterruptHandler(12,stack_exception_handler,0);
  setInterruptHandler(13,general_protection_handler,0);
  setInterruptHandler(14,page_fault_handler,0);
  setInterruptHandler(16,floating_point_error_handler,0);
  setInterruptHandler(17,aligment_check_handler,0);

  setInterruptHandler(32,timer_handler,0);
  setInterruptHandler(33,keyboard_handler,0);

  setTrapHandler(0x80,sys_call_handler,3);

  set_idt_reg(&idtR);
}


void divide_error_routine()
{
	printk("Divide Error (0)");
	while(1);
}
void debug_routine()
{
	printk("Debug (1)");
	while(1);
}
void NMI_routine()
{
	printk("NMI (2)");
	while(1);
}
void breakpoint_routine()
{
	printk("Breakpoint (3)");
	while(1);
}
void overflow_routine()
{
	printk("Overflow (4)");
	while(1);
}
void bounds_check_routine()
{
	printk("Bounds Check (5)");
	while(1);
}
void invalid_opcode_routine()
{
	printk("Invalid Opcode (6)");
	while(1);
}
void device_not_available_routine()
{
	printk("Device Not Available (7)");
	while(1);
}
void double_fault_routine()
{
	printk("Double Fault (8)");
	while(1);
}

void coprocessor_segment_overrun_routine()
{
	printk("Coprocessor Segment Overrun (9)");
	while(1);
}

void invalid_tss_routine()
{
	printk("Invalid TSS (10)");
	while(1);
}

void segment_not_present_routine()
{
	printk("Segment not present (11)");
	while(1);
}

void stack_exception_routine()
{
	printk("Stack Exception (12)");
	while(1);
}

void general_protection_routine()
{
	printk("General Protection (13)");
	while(1);
}

//Si un proces provoca una fallada de pagina s'imprimeix l'error i mor.
//Si aquest procés el el P0 es queda en un bucle infinit
void page_fault_routine()
{
	int pid = current()->pid;
	printk("\nExcepcio de fallada de pagina (14). El proces fallat es el ");
	printi(pid);
	printk("\n");
	// Si el procés fallat és el 0 -> bucle infinit
	if(pid == 0)
	{
//		__asm__ __volatile__("sti");
		while(1);
	}

	// Matem el procés fallat diferent de 0
	sys_exit();	
}

void floating_point_error_routine()
{
	printk("Floatin Point Error (16)");
	while(1);
}

void aligment_check_routine()
{
	printk("Aligment Check (17)");
	while(1);
}

void timer_routine()
{
	char a;
	temps.tics++;
	if (temps.tics == 18)
	{
		temps.tics = 0;

		// Actualitzar time
		temps.segons++;
		if (temps.segons > 59) {
			temps.segons = 0;
			temps.minuts++;
			if (temps.minuts > 59) {
				temps.minuts = 0;
				temps.hores++;
			}
		}
		
		itoa(temps.segons % 10,&a); //Agafem la unitat
		printc_xy(79,0,a);
		itoa(temps.segons / 10,&a); //Agafem la desena
		printc_xy(78,0,a);
		
		printc_xy(77,0,':');
		
		itoa(temps.minuts % 10,&a);//Agafem la unitat
		printc_xy(76,0,a);
		itoa(temps.minuts / 10,&a);//Agafem la desena
		printc_xy(75,0,a);
		
		printc_xy(74,0,':');
		
		itoa(temps.hores % 10,&a);//Agafem unitat
		printc_xy(73,0,a);
		itoa((temps.hores / 10)% 10,&a);//Afafem desena
		printc_xy(72,0,a);
	}
	actualitzaQuantum();
}

//Escriu tecla
void keyboard_update(unsigned char key)
{
	//Tecla no valida o de control
	if (key > sizeof(char_map)  || char_map[key] == '\0' || char_map[key] == '\n') {
		printc_xy(73,24,'C');
		printc_xy(74,24,'o');
		printc_xy(75,24,'n');
		printc_xy(76,24,'t');
		printc_xy(77,24,'r');
		printc_xy(78,24,'o');
		printc_xy(79,24,'l');
	//Esborra Control i escriu lletra corresponent
	} else {
		printc_xy(73,24,' ');
		printc_xy(74,24,' ');
		printc_xy(75,24,' ');
		printc_xy(76,24,' ');
		printc_xy(77,24,' ');
		printc_xy(78,24,' ');
		printc_xy(79,24, char_map[key]);
	}
}

//Recull tecla i l'envia a update
void keyboard_routine()
{
	unsigned char key;
	char tecla, aux;
	int k;

	key = inb(0x60); //Agafem el 'port' de registre de dades
	if (key < 0x80) //Si es un make, entres
	{  
    		key = key & 0x7F; //Ens quedem amb el codi de rastreig
    		keyboard_update(key);
		tecla = char_map[key];

		//Cas en que no hi ha processos bloquejats a la cua del teclat
		if(list_empty(&keyboardqueue))
		{
			//Afegeix la tecla al buffer en cas de no ser ple
			if(bufCirc.estatBuf != PLE) afegir(&bufCirc, tecla);
		}
		else //Cas en que hi ha processos bloquejats
		{
			//Consultem el primer procés de la cua de bloquejats pel teclat
			struct task_struct *primerBloquejat = list_head_to_task_struct(list_first(&keyboardqueue));
			// Ens donem visibilitat sobre el procés on haurem de guardar els caràcters premuts
			for(k=0;k<NUM_PAG_DATA;k++) set_ss_pag(PAG_LOG_INIT_DATA_P0 + NUM_PAG_DATA + k, primerBloquejat->ph_page + k);
			
			//Flush TLB (Hem modificat la taula de pagines)
			set_cr3();

			//Agafem l'@ del buffer del procés
			char *buffer = primerBloquejat->bufferLectura;

			//Desplacem el punter de l'@ 
			 buffer = buffer + PAGE_SIZE * NUM_PAG_DATA;

			//Cas en que el buffer no està BUIT, per si de cas fos ple traiem primer un char del buffer circular
			//i afegim el nou.
			if(bufCirc.estatBuf != BUIT)
			{
				aux = extreure(&bufCirc);
				afegir(&bufCirc, tecla);
			}
			else aux = tecla; //Cas en que el buffer està BUIT. No afegim res. 
		
			//Afegeim la tecla al buffer del procés que l'esperava i incrementem el nombre de charsLlegits
			buffer[primerBloquejat->charsLlegits] = aux;
			primerBloquejat->charsLlegits++;

			//k = caràcters que resten per llegir del primer procés bloquejat
			k = (primerBloquejat->charsDemanats) - (primerBloquejat->charsLlegits);
			//printi(k); 		

			while(k>0 && bufCirc.estatBuf != BUIT)
			{
				buffer[primerBloquejat->charsLlegits] = extreure(&bufCirc);
				primerBloquejat->charsLlegits++;
				k--;
			}
			//En cas que el procés ha acabat ja el seu read(), li afegim el resultat i el retornem a la runqueue
			if(k == 0)
			{
				//KERNEL_STACK_SIZE - 10 = %eax
				((union task_union *)(primerBloquejat))->stack[KERNEL_STACK_SIZE -10] = primerBloquejat->charsLlegits;
				list_del(&(primerBloquejat->tasks), &keyboardqueue);
				list_add(&(primerBloquejat->tasks), &runqueue);
			}
			// Tornem a restaurar l'estat de visibilitat de pàgines anterior
			for(k=0;k<NUM_PAG_DATA;k++) del_ss_pag(PAG_LOG_INIT_DATA_P0 + NUM_PAG_DATA + k);

			//Flush TLB
			set_cr3();
		}
	}
}

