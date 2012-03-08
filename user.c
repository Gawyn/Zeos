#include <libc.h>
#include <jp.h>
char prueba[10];
	int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /*  Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /* __asm__ __volatile__ ("mov %0, %%cr3"::" r" (0) ); */

//	Jocs de proves disponibles:

//	jpProcessos();
//	jpSemafors();
//	jpRead();
	jpZeosFat();	
	while(1);
}
