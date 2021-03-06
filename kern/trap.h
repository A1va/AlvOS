#ifndef ALVOS_KERN_TRAP_H
#define ALVOS_KERN_TRAP_H
#ifndef ALVOS_KERNEL
# error "This is a AlvOS kernel header; user programs should not #include it"
#endif

#include "inc/trap.h"
#include "inc/mmu.h"

/* 内核的 IDT(interrupt descriptor table) */
extern struct Gatedesc idt[];
extern struct Pseudodesc idt_pd;

void trap_init(void);
void trap_init_percpu(void);
void print_regs(struct PushRegs *regs);
void print_trapframe(struct Trapframe *tf);
void page_fault_handler(struct Trapframe *);
void backtrace(struct Trapframe *);

#endif
