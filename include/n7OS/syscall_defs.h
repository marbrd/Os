#ifndef __SYSCALL_DEFS_H__
#define __SYSCALL_DEFS_H__

#include "n7OS/processus.h"

#define NB_SYSCALL 8
    
int sys_example();
int sys_write(const char *s, int len);
int sys_shutdown(int n);
pid_t sys_getpid(void);
int sys_exit();
int sys_fork(const char *name, fnptr function);
int sys_sleep(int duration);
int sys_kill(pid_t pid);

typedef int (*fn_ptr)();
extern fn_ptr syscall_table[NB_SYSCALL];

void add_syscall(int num, fn_ptr function);

#endif
