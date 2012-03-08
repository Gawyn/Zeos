/*
* 	bufCirc.h - Definició del vector circular de teclat
**/

#ifndef __BUFCIRC_H__
#define __BUFCIRC_H__

#define MIDA  		10
#define BUIT		0
#define PLE		2
#define DISPONIBLE 	1

struct bufferCircular 
{
	char buf[MIDA];
	int estatBuf;
	int first;
	int last;
};

void init_bufCirc(struct bufferCircular *buffer);
void afegir(struct bufferCircular *buffer, char c);
char extreure(struct bufferCircular *buffer);
int sizeBuf(struct bufferCircular *buffer);

extern struct bufferCircular bufCirc;

#endif  /* __BUFCIRC_H__ */

