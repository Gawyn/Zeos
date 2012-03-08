#ifndef __ZEOSFAT_H__
#define __ZEOSFAT_H__

#define EOF			-1 
#define FILE_NAME_SIZE		12
#define MAX_BLOCKS		32
#define LLIURE			1
#define OCUPAT			0
#define MAX_FILES		12 // 10 + teclat + pantalla
#define NO_TROBAT		-1
#define O_RDONLY		1
#define O_WRONLY		2
#define O_RDWR			3
#define O_CREAT			4
#define BLOCK_SIZE		256


struct dir_ent
{
	char nom[FILE_NAME_SIZE]; //nom del fitxer
	short int size; //mida del fitxer al disc
};

struct bloc
{
	int lliure;
	int seguent;
};

struct file
{
	char nom[FILE_NAME_SIZE];
	int primerBloc;
	int numRef;
	short int size;
};

extern struct file directori[MAX_FILES];
extern struct bloc zeosfat[MAX_BLOCKS];
extern char espaiDisk[MAX_BLOCKS * BLOCK_SIZE];

int initZeosFAT(void);
int existeixFitxer(const char *path);
int buscarFitxer();
int buscarBloc();

#endif


