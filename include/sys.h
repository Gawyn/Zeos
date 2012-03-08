/*
 *  * sys.h - Definició de les crides al sistema
 *   */
#ifndef __SYS_H__
#define __SYS_H__

#include <zeosFat.h>
#include <fd.h>

int sys_ni_syscall();
int sys_write(int fd,char *buffer, int size);
int sys_getpid();
int sys_fork();
void sys_exit();
int sys_nice(int nouQuantum);
int sys_get_stats(int pid, int *tics);
int sys_sem_init(int n_sem, unsigned int value);
int sys_sem_wait(int n_sem);
int sys_sem_signal(int n_sem);
int sys_sem_destroy(int n_sem);
void printi(int i);
int sys_open(const char *path, int flags);
int buscar_canal_lliure();
int sys_dup(int fd);
int sys_close(int fd);
int sys_unlink(const char *path);
int sys_readdir(struct dir_ent *buffer, int offset);
void sys_open_kernel(const char *path, int fd, struct file_operations *fo);

#endif  /* __SYS_H__ */

