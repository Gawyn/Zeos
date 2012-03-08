#include <fd.h>
#include <devices.h>

void init_descriptors()
{
	pantalla.funcio_read = 0;
        pantalla.funcio_write = sys_write_console;
        pantalla.funcio_close = 0;
        pantalla.funcio_dup = 0;
        pantalla.funcio_open = 0;

        teclat.funcio_read = sys_read_keyboard;
        teclat.funcio_write = 0;
        teclat.funcio_close = 0;
        teclat.funcio_dup = 0;
        teclat.funcio_open = 0;

        disk.funcio_read =  sys_read_disk;
        disk.funcio_write = sys_write_disk;
        disk.funcio_close = 0;
        disk.funcio_dup = 0;
        disk.funcio_open = sys_open_disk;

}
