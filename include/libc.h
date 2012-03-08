/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <zeosFat.h>

int write(int fd, char *buffer, int size);
int read(int fd, char*buffer, int size);
int open(const char *path, int flag);
int close(int fd);
int dup(int fd);
void perror();
int getpid();
int fork();
int get_stats(int pid, int *tics);
int nice(int nouQuantum);
void exit(void);
void printi(int num);
int sem_init(int n_sem, unsigned int value);
int sem_wait(int n_sem);
int sem_signal(int n_sem);
int sem_destroy(int n_sem);
int unlink(const char *path);
int readdir(struct dir_ent *buffer, int offset);

void llista_fitxers(struct dir_ent *buf, int ent);



#endif  /* __LIBC_H__ */
