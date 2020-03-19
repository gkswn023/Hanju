#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

#define NULL 0

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int flag = 0;
int nextpid = 1;
extern void forkret(void);
extern void trapret(void);
extern pte_t* walkpgdir(pde_t* pgdir, const void* va, int alloc);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->level = 0;
  p->priority = 0;
  p->tick = 0;
  p->mono = 0;
  p->tid = p->pid;
  p->main_t = p;
  for(int i = 0; i < NPROC; i++){
	p->child_t[i] = NULL;
	p->retval[i] = NULL;
	p->ustack[i] = NULL;
  }
  p->child_t[0] = p;
  //if unused, level/priority/ticks = 0. 

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();
  struct proc *mp = myproc()->main_t;

  sz = mp->sz;
  if(n > 0){
    if((sz = allocuvm(mp->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(mp->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  for(int i = 0; i < NPROC; i++){
	if(mp->child_t[i])
		mp->child_t[i]->sz = sz;
  }
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();
  struct proc *mp = myproc()->main_t;

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = mp;
  *np->tf = *curproc->tf;

  for(i = 0; i < NPROC; i++){
	np->ustack[i] = mp->ustack[i];
	if(mp->child_t[i] == curproc && i !=0){
		pte_t *pteo = walkpgdir(np->pgdir, (void*)np->ustack[i] - PGSIZE, 0);
		uint pao = PTE_ADDR(*pteo);
		pte_t *pten = walkpgdir(np->pgdir, (void*)np->ustack[0] - PGSIZE, 0);
		uint pan = PTE_ADDR(*pten);
		memmove((char*)P2V(pan), (char*)P2V(pao), PGSIZE);
		np->tf->esp = (np->tf->esp - np->ustack[i]) + np->ustack[0];
	}
  }

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}
// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *mp = myproc()->main_t;
  struct proc *p;
  int i, fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files include threads.

  for(i = 0; i < NPROC; i++){
	if(mp->child_t[i]){
		if(mp->child_t[i]->state != ZOMBIE){
			for(fd = 0; fd < NOFILE; fd++){
				if(mp->child_t[i]->ofile[fd]){
					fileclose(mp->child_t[i]->ofile[fd]);
					mp->child_t[i]->ofile[fd] = 0;
					}
			}
			begin_op();
			iput(mp->child_t[i]->cwd);
			end_op();
			mp->child_t[i]->cwd = 0;
		}
	}
  }

  acquire(&ptable.lock);

  for(i = 0; i < NPROC; i++){
	if(mp->child_t[i])
		mp->child_t[i]->state = ZOMBIE;
  }
  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == mp){
      p->parent = mp->parent;
      if(p->state == ZOMBIE)
        wakeup1(mp->parent);
    }
  }
  kill2(curproc);

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid, i;
  struct proc *mp = myproc()->main_t;
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != mp)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
		if(p == p->main_t){
			kill2(p);
			freevm(p->pgdir);
		}
		else{
			for(i = 1; i < NPROC; i++){
				if(p->main_t->child_t[i] == p){
				p->main_t->child_t[i] = 0;
				break;
				}
			}	
		}
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
		p->level = 0;
		p->priority = 0;
		p->tick = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || mp->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(mp, &ptable.lock);  //DOC: wait-sleep
  }
}

void
kill2(struct proc* curproc)
{
  struct proc *p;
  struct proc *mp = curproc->main_t;
  int i;

  for(i = 0; i < NPROC; i++){
	if(mp->child_t[i]){
		if(mp->child_t[i]->state == ZOMBIE){
			p = mp->child_t[i];
			if(p == curproc) continue;
			mp->child_t[i] = NULL;
			kfree(p->kstack);
			p->kstack = 0;
			p->pid = 0;
			p->parent = 0;
			p->name[0] = 0;
			p->killed = 0;
			p->chan = 0;
			p->state = UNUSED;
			p->tick = 0;
			p->level = 0;
			p->mono = 0;
			p->priority = 0;
		}
	}
  }
}

int
thread_create(thread_t* thread, void* (*start_routine)(void*), void* arg)
{
  int i;
  struct proc *nt;
  struct proc *curproc = myproc();
  uint sp, ustack[3];

  if((nt = allocproc()) == 0){
	return -1;
  }

  nt->pgdir = curproc->pgdir;
  nt->sz = curproc->sz;
  nt->parent = curproc->parent;
  *nt->tf = *curproc->tf;
  nt->pid = curproc->pid;

  for(i = 0; i < NOFILE; i++)
	if(curproc->ofile[i])
		nt->ofile[i] = filedup(curproc->ofile[i]);
  nt->cwd = idup(curproc->cwd);

  safestrcpy(nt->name, curproc->name, sizeof(curproc->name));
  
  acquire(&ptable.lock);
  
  for(i = 1; i < NPROC; i++){
	if(curproc->child_t[i] == NULL){
		curproc->child_t[i] = nt;
		sp = curproc->ustack[i];
		memset((void*)(sp-PGSIZE), 0, PGSIZE);
		break;	
	}	
  }
  if(i == NPROC) goto bad;

  ustack[0] = 0xffffffff;
  ustack[1] = (uint)arg;
  ustack[2] = 0;
  
  sp -= sizeof(ustack);

  if(copyout(curproc->pgdir, sp, ustack, sizeof(ustack)) < 0)
	goto bad;

  nt->child_t[0] = NULL; //child_t[0] is process
  nt->main_t = curproc;
  nt->tf->esp = sp;
  *thread = nt->tid;
  nt->tf->eip = (uint)start_routine;
  nt->state = RUNNABLE;

  release(&ptable.lock);
  
  return 0;

  bad :
	curproc->child_t[i] = NULL;
	nt->state = UNUSED;
	return -1;
}

void thread_exit(void *retval)
{
  struct proc *curproc = myproc();
  int fd, i;

  if(curproc == curproc->main_t)
	panic("main thread called thread_exit -> exit()");

  for(i = 1; i < NPROC; i++){
	if(curproc->main_t->child_t[i] == curproc)
		curproc->main_t->retval[i] = retval;
  }

  for(fd = 0; fd < NOFILE; fd++){
	if(curproc->ofile[fd]){
		fileclose(curproc->ofile[fd]);
		curproc->ofile[fd] = 0;
	}
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  wakeup1(curproc->main_t);
  curproc->state = ZOMBIE;

  sched();
}

int
thread_join(thread_t thread, void **retval)
{
  struct proc *t;
  struct proc *curproc = myproc();
  struct proc *mt = myproc()->main_t;
  int flag, i;

  acquire(&ptable.lock);

  for(;;){
	flag = 0;
	for(i = 0; i < NPROC; i++){
		if(mt->child_t[i]){
			if(mt->child_t[i]->tid == thread){
				t = mt->child_t[i];
				flag = 1;
				if(t->state == ZOMBIE){
					if(retval != NULL) *retval = mt->retval[i];
					mt->child_t[i] = NULL;
					mt->retval[i] = NULL;
					kfree(t->kstack);
					t->pid = 0;
					t->parent = 0;
					t->name[0] = 0;
					t->killed = 0;
					t->state = UNUSED;
					t->level = 0;
					t->priority = 0;
					t->tick = 0;
					t->tid = 0;
					t->main_t = NULL;
					release(&ptable.lock);
					return 0;				
				}
				break; 
			}
		}
	}

	if(flag == 0 || curproc->killed){
		release(&ptable.lock);
		return -1;
	}
	
	sleep(mt, &ptable.lock);
  }
}


//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;

  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
#ifdef FCFS_SCHED
	struct proc* f_p = 0;
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->state != RUNNABLE)
			continue;
		
		if(f_p == 0){
			f_p = p;
		}
		else{
			if(f_p->pid > p->pid){
				f_p = p;
			}
		}
	}
	if(f_p != 0){
		c->proc = f_p;
		switchuvm(f_p);
		f_p->state = RUNNING;

		swtch(&(c->scheduler), f_p->context);
		switchkvm();

		c->proc = 0;
	}

#elif MLFQ_SCHED
	struct proc* m_p;
	flag = 0;	
	m_p = 0;
	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->level == 0 && p->state == RUNNABLE){
			flag = 1;
			
			c->proc = p;
			switchuvm(p);
			p->state = RUNNING;
		
			swtch(&(c->scheduler), p->context);
			switchkvm();

			c->proc = 0;
		}
	}
	//all of process are SLEEPING
	if(flag == 0){
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
			if(p->state != RUNNABLE)
				continue;
			
			if(m_p == 0)
				m_p = p;
			else{
				if(m_p->priority <= p->priority){
					if(m_p->priority == p->priority){
						if(m_p->pid > p->pid)
							m_p = p;
					}
					else{
						m_p = p;
					}
				}	
			}
		}
		
		if(m_p != 0){
			c->proc = m_p;
			switchuvm(m_p);
			m_p->state = RUNNING;

			swtch(&(c->scheduler), m_p->context);
			switchkvm();
		
			c->proc = 0;
		}
	}
#else
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
#endif
    release(&ptable.lock);

  }
}

void 
priority_boosting(void)
{
	struct proc *p;

	acquire(&ptable.lock);
	acquire(&tickslock);

	if((ticks%100) == 0){
		for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
			p->level = 0;
			p->priority = 0;
			p->tick = 0;
		}	
	}

	release(&tickslock);
	release(&ptable.lock);
}

void
addtick(void)
{
	acquire(&ptable.lock);
	myproc()->tick++;
	release(&ptable.lock);
}

void
setpriority(int pid, int priority)
{
	struct proc *p;
	
	acquire(&ptable.lock);

	for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->pid == pid){
			p->priority = priority;
			break;
		}		
	}

	release(&ptable.lock);
}

void
monopolize(int password){
	acquire(&ptable.lock);

	if(password == 2014005032){
		myproc()->mono++;
		myproc()->level = 0;
		if((myproc()->mono % 2) == 0){
			myproc()->priority = 0;
		}
	}
	else{
		myproc()->killed = 1;
		myproc()->level = 0;
		myproc()->mono = 0;
	}

	release(&ptable.lock);
}

int
getlev(void){
	int i;

	acquire(&ptable.lock);
	i = myproc()->level;
	release(&ptable.lock);
	
	return i;
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

void
mlfq_yield(void)
{
	acquire(&ptable.lock);
	
	if(myproc()->level == 0 && myproc()->tick == 4){
		myproc()->level++;
		myproc()->tick = 0;
		myproc()->state = RUNNABLE;
		sched();
	}
	else if(myproc()->level == 1 && myproc()->tick == 8){
		if(myproc()->priority != 0)
			myproc()->priority--;
		myproc()->tick = 0;
		myproc()->state = RUNNABLE;
		sched();
	}
	
	release(&ptable.lock);
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
#ifdef MLFQ_SCHED
  myproc()->tick = 0;
#endif
  myproc()->state = RUNNABLE;	
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;
#ifdef FCFS_SCHED
  p->tick = 0;
#endif

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      p->main_t->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}
