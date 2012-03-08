#ifndef DEVICES_H__
#define  DEVICES_H__


int sys_write_console(int fd, char *buffer, int size);
int sys_read_keyboard(int fd, char *buffer, int size);
int sys_open_disk(const char *path, int flags);
int sys_read_disk(int fd, char *buffer, int size);
int sys_write_disk(int fd, char *buffer, int size);


#endif /* DEVICES_H__*/
