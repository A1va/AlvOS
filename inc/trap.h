#ifndef ALVOS_INC_TRAP_H
#define ALVOS_INC_TRAP_H

// 中断向量
// 这些是由CPU定义的:
#define T_DIVIDE     0		// 0.除零错误
#define T_DEBUG      1		// 1.调试异常
#define T_NMI        2		// 2.不可屏蔽中断
#define T_BRKPT      3		// 3.断点(一个字节的INT3指令)
#define T_OFLOW      4		// 4.溢出(INTO指令)
#define T_BOUND      5		// 5.边界检验(BOUND指令)
#define T_ILLOP      6		// 6.非法操作符
#define T_DEVICE     7		// 7.设备不可用
#define T_DBLFLT     8		// 8.双重错误
/* #define T_COPROC  9 */	// reserved (not generated by recent processors)
#define T_TSS       10		// 10.无效的TSS
#define T_SEGNP     11		// 11.段不存在
#define T_STACK     12		// 12.栈异常
#define T_GPFLT     13		// 13.通用保护
#define T_PGFLT     14		// 14.页错误
/* #define T_RES    15 */	// reserved
#define T_FPERR     16		// 16.x87FPU 浮点错误
#define T_ALIGN     17		// 17.界限检查
#define T_MCHK      18		// 18.机器检查
#define T_SIMDERR   19		// 19.SIMD 浮点错误

// These are arbitrarily chosen, but with care not to overlap
// processor defined exceptions or interrupt vectors.
#define T_SYSCALL   48		// 系统调用
#define T_DEFAULT   500		// catchall

#define IRQ_OFFSET	32	// IRQ 0 对应于 int IRQ_OFFSET

// 硬件 IRQ 号码. 接收的号码是(IRQ_OFFSET + IRQ_xxx)
#define IRQ_TIMER        0
#define IRQ_KBD          1
#define IRQ_SERIAL       4
#define IRQ_SPURIOUS     7
#define IRQ_IDE         14
#define IRQ_ERROR       19

#ifndef __ASSEMBLER__

#include "inc/types.h"

struct PushRegs {
	/* pushal 指令就能把以下所有寄存器压栈 */
    uint64_t reg_r15;
    uint64_t reg_r14;
    uint64_t reg_r13;
    uint64_t reg_r12;
    uint64_t reg_r11;
    uint64_t reg_r10;
    uint64_t reg_r9;
    uint64_t reg_r8;
	uint64_t reg_rsi;
	uint64_t reg_rdi;
	uint64_t reg_rbp;
	uint64_t reg_rdx;
	uint64_t reg_rcx;
	uint64_t reg_rbx;
	uint64_t reg_rax;
} __attribute__((packed));	// 使编译器取消结构体在编译过程中的优化对齐,按照实际占用字节数进行对齐

// 当环境不运行时需要保护原环境的环境帧(寄存器集)，以后恢复原环境，主要用于上下文切换
// 由栈特性，该结构寄存器压栈由下往上，出栈从上至下
// EFLAGS:状态寄存器，RIP:指令指针，RSP:栈指针(栈顶地址)
// padding: 段寄存器只有16位，为了补足栈帧规定的64位
struct Trapframe {
	struct PushRegs tf_regs;
	uint16_t tf_es;
	uint16_t tf_padding1;
    uint32_t tf_padding2;
	uint16_t tf_ds;
	uint16_t tf_padding3;
    uint32_t tf_padding4;
	uint64_t tf_trapno;	// 中断向量
	/* 以下的寄存器由 x86 架构硬件定义 */
	uint64_t tf_err;	// 错误码
	uintptr_t tf_rip;
	uint16_t tf_cs;
	uint16_t tf_padding5;
    uint32_t tf_padding6;
	uint64_t tf_eflags;
	/* 以下的寄存器是只有环境跳转才会压栈，例如从用户态到内核态 */
	uintptr_t tf_rsp;
	uint16_t tf_ss;
	uint16_t tf_padding7;
    uint32_t tf_padding8;
} __attribute__((packed));

// 为用户自定义的中断处理函数设计，存储错误发生前的环境信息(保护)，错误处理后恢复并继续执行(恢复)
// 与 Trapframe 的差异：
//  1.少了段寄存器信息，因为栈是用户态->内核态->用户态，用户态的环境是同一个用户程序，不涉及段的切换
//  2. Trapframe 存储完整的环境帧，中断后被中断程序保存，在各函数中传递
//     UTrapframe 将引起错误的环境状态信息push进用户错误栈，并未在内核其他地方使用
struct UTrapframe {
	/* 有关用户级错误的信息 */
	uint64_t utf_fault_va;	// 引起错误的虚拟地址(目前只有页错误)
	uint64_t utf_err;		// error code
	/* 陷阱时恢复状态 */
	struct PushRegs utf_regs;
	uintptr_t utf_rip;
	uint64_t utf_eflags;
	/* 陷阱时恢复用户栈 */
	uintptr_t utf_rsp;
} __attribute__((packed));

#endif /* !__ASSEMBLER__ */

#endif
