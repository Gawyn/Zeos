#include <zeosFat.h>
#include <utils.h>
#include <errno.h>

struct file directori[MAX_FILES]   
__attribute__((__section__(".data.directori")));

struct bloc zeosfat[MAX_BLOCKS];
char espaiDisk[MAX_BLOCKS * BLOCK_SIZE];

int initZeosFAT(void)
{
	int i;

	//Inicialitzem el ZeosFAT a tot lliure
	for(i=0;i<MAX_BLOCKS;i++)
	{
		zeosfat[i].lliure = LLIURE;
		zeosfat[i].seguent = EOF;
	}
	
	for(i=0;i<MAX_FILES;i++)
	{
		directori[i].nom[0] = '\0';
		directori[i].primerBloc = EOF;
		directori[i].numRef = 0;
		directori[i].size = 0;
	}
	return 0;
}

int existeixFitxer(const char *path)
{
	int i;
	char trobat = 0;

	for(i=0;i<MAX_FILES && !trobat;i++)
	{
		if(equalStrings(directori[i].nom, (char *)path)) return i;
	}
	return NO_TROBAT;
}

//Busca la posició d'un Bloc lliure
int buscarBloc()
{
	int i;
	for(i=0;i<MAX_BLOCKS;i++)
	{
		if(zeosfat[i].lliure == LLIURE)
		{
			zeosfat[i].lliure = OCUPAT;
			return i;
		}
	}
	return EOF;
}

//Busca una posició lliure per un fitxer dins del directori
int buscarFitxer()
{
	int i;
	for(i=0; i<MAX_FILES; i++)
	{ 	
		if(directori[i].nom[0] == '\0') return i;
	}
	return -ENOSPC;
}
