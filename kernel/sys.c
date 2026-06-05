#include <n7OS/sys.h>
#include <n7OS/syscall_defs.h>
#include <n7OS/console.h>
#include <n7OS/irq.h>
#include <unistd.h>
#include <n7OS/cpu.h>
#include <n7OS/processus.h>

extern void handler_syscall();

int sys_example(void);
int sys_shutdown(int n);
int sys_write(const char *s, int len);
pid_t sys_getpid(void);
int sys_exit(void);
int sys_fork(const char *name, fnptr function);
int sys_sleep(int seconds);
int sys_kill(pid_t pid);

void init_syscall() {
  // ajout de la fonction de traitement de l'appel systeme
  add_syscall(NR_example, sys_example);
  add_syscall(NR_shutdown, sys_shutdown);
  add_syscall(NR_write, sys_write);
  add_syscall(NR_getpid, sys_getpid);
  add_syscall(NR_exit, sys_exit);
  add_syscall(NR_fork, sys_fork);
  add_syscall(NR_sleep, sys_sleep);
  add_syscall(NR_kill, sys_kill); 

  // initialisation de l'IT soft qui gère les appels systeme
  init_irq_entry(0x80, (uint32_t) handler_syscall);
}

// code de la fonction de traitement de l'appel systeme example
int sys_example() {
  // on ne fait que retourner 1
  return 1;
}

int sys_write(const char *s, int len) {
  console_putbytes(s, len);
  return 1;
}

int sys_shutdown(int n) {
  if (n == 1) {
    outw(0x2000, 0x604); // Poweroff qemu > 2.0
    return -1;
  } else {
    return n;
  }
}

pid_t sys_getpid() {
  return getpid_process();
}

int sys_exit() {
  terminer(getpid_process());
  return 0; // ne sera jamais atteint
}

int sys_fork(const char *name, fnptr function) {
  return fork_process(name, function);
}

int sys_sleep(int seconds) {
  return sleep_process(seconds * 1000); // Convertir les secondes en millisecondes
}

int sys_kill(pid_t pid) {
  terminer(pid);
  return 0;
}