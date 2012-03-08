#include <io.h>
#include <devices.h>
#include <sched.h>
#include <list.h>
#include <bufCirc.h>
#include <zeosFat.h>
#include <sys.h>
#include <errno.h>

//Part dependent del dispositiu en la crida al sistema de Write
int sys_write_console(int fd, char *buffer, int size)
{
 	int i;
	for(i=0;i<size;i++) printc(buffer[i]);
	return i;
}

int sys_read_keyboard(int fd, char *buffer, int size)
{
	int i;
	char actual;

	//Cas en que el proces pot llegir directament del teclat
	if ((sizeBuf(&bufCirc) >= size) && (list_empty(&keyboardqueue)))
	{
		for(i=0;i<size;i++)
		{
			actual = extreure(&bufCirc);
			buffer[i] = actual;
		}
	}
	else 	//Cas en que el procés haurà d'esperar abans de llegir (pq té processos esperant de 
		//llegir del teclat o bé perquè encara no s'ha acabat d'escriure al teclat)
	{
		if(current()->pid == 0) return -EAGAIN; 
		//Encara no hem llegit res
		current()->charsLlegits = 0;
		//Els chars que hem de llegir
		current()->charsDemanats = size;
		//Ens guardem el buffer a llegir
		current()->bufferLectura = buffer;

		//Bloquegem el procés actual i donem pas al següent procés
		list_del(&(current()->tasks), &runqueue);
		list_add(&(current()->tasks), &keyboardqueue);

		struct task_struct *t = list_head_to_task_struct(list_first(&runqueue));
		task_switch((union task_union*) t);
	}
	return 0;
}

int sys_open_disk(const char *path, int flags)
{
	int canal, trobat, bloc, i;

	//Comprovem que l'usuari no hagi escrit redundantment la barra de directori
	if(path[0] == '/') path++;

	//Comprova l'existencia del fitxer
	trobat = existeixFitxer(path);

	//Comprova que les flags siguin correctes
	if (flags!=O_RDONLY && flags!=O_WRONLY && flags!=O_RDWR && flags!=(O_RDONLY | O_CREAT) && flags!=(O_WRONLY | O_CREAT) && flags!=(O_RDWR | O_CREAT))
	{
		return -EBADF; //Buscar ERROR
	}
    	if((trobat == NO_TROBAT) && ((flags & O_CREAT) != O_CREAT)) 
	{
		return -ENXIO; //BUSCAR ERROR	
	}

	canal = buscar_canal_lliure();
	if(canal < 0) return -EAGAIN; //Recurs temporalment no disponible

	if(trobat == NO_TROBAT)
	{
		trobat = buscarFitxer();
		if(trobat < 0) return -ENOSPC; // No Space Left On Device
		bloc = buscarBloc();
		if(bloc < 0) return -ENOSPC; // No Space Left On Device


		//Copiem el nom
		for(i=0; i<FILE_NAME_SIZE -1; i++) directori[trobat].nom[i] = path[i];

		directori[trobat].nom[i] = '\0';
		directori[trobat].primerBloc = bloc;
		directori[trobat].numRef = 1;
		directori[trobat].size = 1;  //Com a mínim un file de disc ocupa un bloc
	}	
	else directori[trobat].numRef++;
	
	current()->taula_canals[canal].estat = OBERT;
	//Describim el mode l'accés al canal que és el mateix que les flags sense O_CREAT
	current()->taula_canals[canal].mode = flags & 3; 	
	//Definim la posició dinàmica inicial de lectura del fitxer
	current()->taula_canals[canal].pos = 0;
	//Definim la file_operations de disc
	current()->taula_canals[canal].descriptor = &disk;
	//Establim l'enllaç canal - fitxer
	current()->taula_canals[canal].numFile = trobat;
	
	return canal;
}


int sys_read_disk(int fd, char *buffer, int size)
{
	int punterPos, i, bloc, byte, llegits;
	
	punterPos = current()->taula_canals[fd].pos;

	//Comprovem que la size a llegir no sigui més gran que el tamany del fitxeri
	if(size > directori[current()->taula_canals[fd].numFile].size - punterPos)
	{
		size = (directori[current()->taula_canals[fd].numFile].size - punterPos);
		if(size <= 0) return 0;
	}

	bloc = directori[current()->taula_canals[fd].numFile].primerBloc;
	
	for(i=0; i<punterPos/BLOCK_SIZE; i++) bloc = zeosfat[bloc].seguent;

	byte = punterPos % BLOCK_SIZE;
	
	llegits = 0;
	while(bloc != EOF && llegits < size)
	{
		buffer[llegits] = espaiDisk[byte + (bloc * BLOCK_SIZE)];
		if(byte == (BLOCK_SIZE - 1))
		{
			byte = 0;
			bloc = zeosfat[bloc].seguent;
		}
		else byte++;
		llegits++;
	}
	current()->taula_canals[fd].pos += llegits;
	return llegits;
}



int sys_write_disk(int fd, char *buffer, int size)
{
	int punterPos, i, bloc, byte, escrits;
	char fiMem;

	//Agafem el punter de posicio
	punterPos = current()->taula_canals[fd].pos;

	//Agafem el punter al priner bloc
	bloc = directori[current()->taula_canals[fd].numFile].primerBloc;

	//Avancem el punter fins l'últim bloc
	for(i=0; i<punterPos/BLOCK_SIZE; i++) bloc = zeosfat[bloc].seguent;

	//Avancem el punter dins del bloc fins la posició a partir de la que hem d'escriure
	byte = punterPos % BLOCK_SIZE;
	
	escrits = 0;
	fiMem = 0;
	while(!fiMem && escrits < size)
	{
		//Escrivim els caràcters del buffer al disc
		espaiDisk[byte + (bloc * BLOCK_SIZE)] = buffer[escrits];

		//Cas en que s'ha acabat el bloc, busquem un de nou
		if(byte == (BLOCK_SIZE - 1))
		{	
			byte = 0;
			zeosfat[bloc].seguent = buscarBloc();
			bloc = zeosfat[bloc].seguent;
			if(bloc == EOF) 
			{
				fiMem = 1;
			}
		}
		else byte++;
		escrits++;
	}
	//Modifiquem el punter de posició
	current()->taula_canals[fd].pos += escrits;
	if(directori[current()->taula_canals[fd].numFile].size < current()->taula_canals[fd].pos)
	directori[current()->taula_canals[fd].numFile].size = current()->taula_canals[fd].pos;

	return escrits;
}
