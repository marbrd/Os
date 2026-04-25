#include <inttypes.h>
#include <n7OS/time.h>
#include <n7OS/cpu.h>
#include <n7OS/irq.h>
#include <stdio.h>

extern void handler_timer();

volatile int timer;

void init_timer() {
    timer = 0;
    duree_exec();
    uint32_t frequence = 1190/1;
    outb(0x34,0x43);
    outb(frequence&0xFF, 0x40);
    outb(frequence>>8, 0x40);
    outb(inb(0x21)&0xfe, 0x21);
    init_irq_entry(0x20, (uint32_t) handler_timer);
}

void handler_timer_C() {
    outb(0x20, 0x20);
    timer++;
    if (timer%1000==0) {
        duree_exec();
    }
}

void duree_exec() {
    int heures, minutes, secondes;
    int timer_s = timer/1000;
    heures = timer_s/3600;
    minutes = (timer_s%3600)/60;
    secondes = timer_s%60;
    timer_setup();
    printf("%02d:%02d:%02d", heures, minutes, secondes);
    timer_cleanup();
}

