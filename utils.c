#include <utils.h>
#include <types.h>
#include <sys.h>

void copy_data(void *start, void *dest, int size)
{
  DWord *p = start, *q = dest;
  while(size > 0) {
    *q++ = *p++;
    size -= 4;
  }
}
/* Copia de espacio de usuario a espacio de kernel, devuelve 0 si ok y -1 si error*/
int copy_from_user(void *start, void *dest, int size)
{
  DWord *p = start, *q = dest;
  while(size > 0) {
    *q++ = *p++;
    size -= 4;
  }
  return 0;
}
/* Copia de espacio de kernel a espacio de usuario, devuelve 0 si ok y -1 si error*/
int copy_to_user(void *start, void *dest, int size)
{
  DWord *p = start, *q = dest;
  while(size > 0) {
    *q++ = *p++;
    size -= 4;
  }
  return 0;
}
void printi(int num)
{
        char str[10];
        int i = 0;
        //fem itoa() mentre no arribem al final del num
	str[0] = (char)(num % 10 + 48);
	while (num /= 10) str[++i]=(char)(num % 10 + 48);
        //fem write del resultat convertit
        for (i=i;i>=0;i--) sys_write(1,&str[i],1);
}

int equalStrings(char *s1, char *s2)
{
	int i = 0;
	for(i = 0;s1[i] != '\0'&& s2[i] !='\0';i++)
	{
		if(s1[i] != s2[i]) return 0;	
	}
	if(i != 0) --i;
	if(s1[i] == s2[i]) return 1;
	else return 0;
} 
