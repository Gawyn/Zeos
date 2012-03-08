/*
 * mm.c - Memory Management: Paging & segment memory management
 */

#include <types.h>
#include <mm.h>
#include <segment.h>
#include <hardware.h>
#include <sched.h>
#include <errno.h>

/* SEGMENTATION */
/* Memory segements description table */
Descriptor  *gdt = (Descriptor *) GDT_START;
/* Register pointing to the memory segments table */
Register    gdtR;

/* PAGING */
/* Variables containing the page directory and the page table */
  
page_table_entry dir_pages[TOTAL_PAGES]
  __attribute__((__section__(".data.task")));

page_table_entry pagusr_table[TOTAL_PAGES]
  __attribute__((__section__(".data.task")));

/* TSS */
TSS         tss; 



/***********************************************/
/************** PAGING MANAGEMENT **************/
/***********************************************/

/* Init page table directory */
  
void init_dir_pages()
{
  dir_pages[ENTRY_DIR_PAGES].entry = 0;
  dir_pages[ENTRY_DIR_PAGES].bits.pbase_addr = (((unsigned int)&pagusr_table) >> 12);
  dir_pages[ENTRY_DIR_PAGES].bits.user = 1;
  dir_pages[ENTRY_DIR_PAGES].bits.rw = 1;
  dir_pages[ENTRY_DIR_PAGES].bits.present = 1;
}

/* Initializes the page table (kernel pages only) */
void init_table_pages()
{
  int i;
  /* reset all entries */
  for (i=0; i<TOTAL_PAGES; i++)
    {
      pagusr_table[i].entry = 0;
    }
  /* Init kernel pages */
  for (i=0; i<NUM_PAG_KERNEL; i++)
    {
      // Logical page equal to physical page (frame)
      pagusr_table[i].bits.pbase_addr = i;
      pagusr_table[i].bits.rw = 1;
      pagusr_table[i].bits.present = 1;
    }
}

/* Initialize pages for initial process (user pages) */
void set_user_pages( int first_ph_page)
{
 int pag; 
  /* CODE */
  for (pag=PAG_LOG_INIT_CODE_P0;pag<PAG_LOG_INIT_DATA_P0;pag++){
  	pagusr_table[pag].entry = 0;
  	pagusr_table[pag].bits.pbase_addr = first_ph_page;
  	pagusr_table[pag].bits.user = 1;
  	pagusr_table[pag].bits.present = 1;
        first_ph_page++;
  }
  
  /* DATA */ 
  for (pag=PAG_LOG_INIT_DATA_P0;pag<PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA;pag++){
  	pagusr_table[pag].entry = 0;
  	pagusr_table[pag].bits.pbase_addr = first_ph_page;
  	pagusr_table[pag].bits.user = 1;
  	pagusr_table[pag].bits.rw = 1;
  	pagusr_table[pag].bits.present = 1;
	first_ph_page++;
  }
}

/* Writes on CR3 register producing a TLB flush */
void set_cr3()
{
 	asm volatile("movl %0,%%cr3": :"r" (dir_pages));
}

/* Macros for reading/writing the CR0 register, where is shown the paging status */
#define read_cr0() ({ \
         unsigned int __dummy; \
         __asm__( \
                 "movl %%cr0,%0\n\t" \
                 :"=r" (__dummy)); \
         __dummy; \
})
#define write_cr0(x) \
         __asm__("movl %0,%%cr0": :"r" (x));
         
/* Enable paging, modifying the CR0 register */
void set_pe_flag()
{
  unsigned int cr0 = read_cr0();
  cr0 |= 0x80000000;
  write_cr0(cr0);
}

/* Associates logical page 'page' with physical page 'frame' */
void set_ss_pag(unsigned page,unsigned frame)
{
	pagusr_table[page].entry=0;
	pagusr_table[page].bits.pbase_addr=frame;
	pagusr_table[page].bits.user=1;
	pagusr_table[page].bits.rw=1;
	pagusr_table[page].bits.present=1;
}

//Marquem com a buida la pàgina lògica page
void del_ss_pag(unsigned page)
{
	pagusr_table[page].entry = 0;
}
/* Initializes paging an the process 0 address space */
void init_mm()
{
	int first_ph;
 	init_table_pages();
	init_frames();
	first_ph = initialize_P0_frames();
 	set_user_pages( first_ph );
	init_dir_pages();
	set_cr3();
 	set_pe_flag();
}
/***********************************************/
/************** SEGMENTATION MANAGEMENT ********/
/***********************************************/
void setGdt()
{
  /* Configure TSS base address, that wasn't initialized */
  gdt[KERNEL_TSS>>3].lowBase = lowWord((DWord)&(tss));
  gdt[KERNEL_TSS>>3].midBase  = midByte((DWord)&(tss));
  gdt[KERNEL_TSS>>3].highBase = highByte((DWord)&(tss));

  gdtR.base = (DWord)gdt;
  gdtR.limit = 256 * sizeof(Descriptor);

  set_gdt_reg(&gdtR);
}

/***********************************************/
/************* TSS MANAGEMENT*******************/
/***********************************************/
void setTSS()
{
  tss.PreviousTaskLink   = NULL;
  tss.esp0               = KERNEL_ESP;
  tss.ss0                = __KERNEL_DS;
  tss.esp1               = NULL;
  tss.ss1                = NULL;
  tss.esp2               = NULL;
  tss.ss2                = NULL;
  tss.cr3                = NULL;
  tss.eip                = 0;
  tss.eFlags             = INITIAL_EFLAGS; /* Enable interrupts */
  tss.eax                = NULL;
  tss.ecx                = NULL;
  tss.edx                = NULL;
  tss.ebx                = NULL;
  tss.esp                = USER_ESP;
  tss.ebp                = tss.esp;
  tss.esi                = NULL;
  tss.edi                = NULL;
  tss.es                 = __USER_DS;
  tss.cs                 = __USER_CS;
  tss.ss                 = __USER_DS;
  tss.ds                 = __USER_DS;
  tss.fs                 = NULL;
  tss.gs                 = NULL;
  tss.LDTSegmentSelector = KERNEL_TSS;
  tss.debugTrap          = 0;
  tss.IOMapBaseAddress   = NULL;

  set_task_reg(KERNEL_TSS);
}

 
/* Initializes the ByteMap of free physical pages.
 * The kernel pages are marked as used */
int init_frames( void )
{
    int i;
    /* Mark pages as Free */
    for (i=0; i<TOTAL_PAGES; i++) {
        phys_mem[i] = FREE_FRAME;
    }
    /* Mark kernel pages as Used */
    for (i=0; i<NUM_PAG_KERNEL; i++) {
        phys_mem[i] = USED_FRAME;
    }
    return 0;
}

/* initialize_P0_frames - Initializes user code frames and user data frames for Initial Process P0.
 * Returns the First Physical frame for P0 */
int initialize_P0_frames(void)
{
    int i;
    /* Mark User Code pages as Used */
    for (i=0; i<NUM_PAG_CODE; i++) {
        phys_mem[NUM_PAG_KERNEL+i] = USED_FRAME;
    }
    /* Mark User Data pages as Used */
    for (i=0; i<NUM_PAG_DATA; i++) {
        phys_mem[NUM_PAG_KERNEL+NUM_PAG_CODE+i] = USED_FRAME;
    }
    return NUM_PAG_KERNEL;
}

//Busca espais lliures a phys_mem[]
int busca(int i, int nframes)
{
	int k=0;
	int suma=0;
	if (nframes+i>TOTAL_PAGES) return -ENOMEM;
	while(k<nframes)
	{
		suma=suma+phys_mem[i+k];
		k++;	
	}

	if (suma>0) return 0;
	return 1;

}

//Busca nframes lliures a la memòria fisica, els marca com a ocupats i retorna
// el punter a la primera posicio trobada dins el phys_mem
//Nota: De moment només tractem els casos en que els frames són consecutius en memòria.
int alloc_frames(int nframes)
{
	int i=0;
	int res=0;
	int k=0;

	if (nframes>TOTAL_PAGES)
	{
		return -ENOMEM;	
	}

	for(i=0;i<TOTAL_PAGES;i++)
	{
		res=busca(i,nframes);
		if (res==1) 
		{
			for (k=0;k<nframes;k++)	phys_mem[i+k]=USED_FRAME;
			return i;
		}
		else if(res==-1) return -ENOMEM; 
	}
	return -EAGAIN;
}

//Allibera nframes a partir de frame
void free_frames(unsigned int frame, int nframes)
{
	int i=0;
	for(i=0;i<nframes;i++)
	{
		phys_mem[frame+i] = FREE_FRAME;
	}
}


