#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_getppid(void){
	
	cprintf("My pid is %d\n", myproc()->pid);
	cprintf("my ppid is %d\n", myproc()->parent->pid);

	return 0;
}
