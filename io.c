/*
 * io.c - 
 */

#include <io.h>
#include <sys.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y=15;

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}

void printc(char c)
{
  Word ch = (Word) (c & 0x00FF) | 0x0200;
  DWord screen = 0xb8000 + (y * NUM_COLUMNS + x) * 2;
  if (c=='\n') 
  {
	  y++;
	  x=0;
  }
  else
  {
	  if (++x >= NUM_COLUMNS)
	  {
	    x = 0;
	    if (++y >= NUM_ROWS)
	      y = 0;
	  }
	  asm("movw %0, (%1)" : : "g"(ch), "g"(screen));
  }
	//Per imprimir per pantalla d'ubuntu
	__asm__ __volatile__ (
	"movb %0, %%al\n"
	"outb $0xe9"
	::"g"(c)
	); 
}

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}


//Converteix un enter en un char. Per això li sumem 48.
void itoa(int num, char *buffer) 
{
	*buffer = (char)(num+48);
}

//Igual que el printc però amb posicio determinada xy
void printc_xy(int x, int y, char c)
{
  Word ch = (Word) (c & 0x00FF) | 0x0200;
  DWord screen = 0xb8000 + (y * NUM_COLUMNS + x) * 2;
  asm("movw %0, (%1)" : : "g"(ch), "g"(screen));
}


