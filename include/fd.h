#ifndef __FD_H__
#define __FD_H__

#define NDISPS                	10
#define NCANALS         	10
#define OBERT			1
#define TANCAT			0
#define O_RDONLY		1
#define O_WRONLY		2
#define O_RDWR			3
#define O_CREAT			4


struct file_operations 
{
        int (*funcio_read)();
        int (*funcio_write)();
        int (*funcio_close)();
        int (*funcio_dup)();
	int (*funcio_open)();
};

struct canal 
{
        char estat; //Indica si el canal està obert o tancat
        char mode; //Indica el mode d'acces
        struct file_operations* descriptor; //Punter al descriptor del dispositiu
	int pos; //Posicio del punter
	int numFile;
};

struct file_operations pantalla;
struct file_operations teclat;
struct file_operations disk;
	
void init_descriptors();

#endif


