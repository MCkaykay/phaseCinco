// main.c, 159
// OS bootstrap and The Kernel for OS phase 1
//
// Team Name: null  
// Members: Mckayla Glaves, Javanika Naik

#include "include.h"  // given SPEDE stuff
#include "types.h"    // kernle data types
#include "lib.h"      // small handly functions
#include "proc.h"     // all user process code here
#include "isr.h"      // kernel ISR code
#include "entry.h"    // entries to kernel (TimerEntry, etc.)

// kernel data are all here:
int cur_pid;                        // current running PID; if -1, none selected
q_t ready_q, avail_q;               // avail PID and those created/ready to run
pcb_t pcb[PROC_MAX];                // Process Control Blocks
char stack[PROC_MAX][STACK_SIZE];   // process runtime stacks

void InitKernel(void) {             // init and set up kernel!
   int i;
   struct i386_gate *IVT_p;         // IVT's DRAM location

   IVT_p = get_idt_base();          // get IVT location
   fill_gate(&IVT_p[TIMER], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0); // fill out IVT for timer
   outportb(PIC_MASK, MASK);                   // mask out PIC for timer

   Bzero(&avail_q, sizeof(q_t));                      // clear 2 queues
   Bzero(&ready_q, sizeof(q_t));
   for(int i=0; i<= PROC_MAX-1; i++){                 // add all avail PID's to the queue
     EnQ(&avail_q, i);
}

void Scheduler(void) {             // choose a cur_pid to run
   //if cur_pid is greater than 0, just return; // a user PID is already picked
   if (cur_pid > 0) return;
   if (ready_q == 0 && cur_pid == 0) return;  //if ready_q is empty && cur_pid is 0, just return; // InitProc OK

   if (ready_q == 0 && cur_pid == -1) {
      cons_printf "Kernel panic: no process to run!\n);
      breakpoint();                                  // to GDB we go
   }

   //if cur_pid is not -1, then append cur_pid to ready_q; // suspend cur_pid
   //replace cur_pid with the 1st one in ready_q; // pick a user proc
   if (cur_pid != -1) {
     EnQ(cur_pid, &ready_q);
     pcb[cur_pid].state = READY;
     cur_pid = ready_q[0];
   }
   pcb[cur_pid].time = 0;
   pcb[cur_pid].state = READY;
   // reset process time
   // change its state
}

int main(void) {                       // OS bootstraps
   //initialize the kernel-related stuff by calling ...
   
   //create InitProc process;            // create InitProc
   //set cur_pid to the 1st PID;         // select cur_pid
   //call Loader(with its TF_p);         // load proc to run
   Loader(pcb[0].TF_p);
   return 0; // statement never reached, compiler needs it for syntax
}

void TheKernel(TF_t *TF_p) {           // kernel runs
   char ch;
   pcb[cur_pid].TF_p = TF_p;
   TimerISR();
   //....TF_p = TF_p; // save TF addr

   //call Timer ISR;                     // handle tiemr event

   //if PC KB pressed {                  // if keyboard pressed
     // get the pressed key/character
      //if it's 'b':                     // 'b' for breakpoint
        // ...                        // go into GDB
         //break;
      //if it's 'n':                     // 'n' for new process
        // call NewProc ISR (with UserProc as argument); // create a UserProc
     //}
   if (cons_kbhit()) {
     char key = cons_getchar();
     if ('b' : breakpoint()) 
       break;
     if ('n' : NewProcISR(UserProc));
   }
   Scheduler();
   Loader(pcb[cur_pid].TF_p);
   //call Scheduler() // which may pick another proc
   //call Loader(with TF_p of scheduled process); // load proc to run!
}
