#include "types.h"
#include "stat.h"
#include "user.h"
//#include "proc.h"

//From proc.h Proc structure
// Per-process state
enum procstate
{
    UNUSED,
    EMBRYO,
    SLEEPING,
    RUNNABLE,
    RUNNING,
    ZOMBIE
};
struct proc
{
    enum procstate state; // Process state
    int pid;              // Process ID
    int ppid;             // Parent process ID
    char name[16];        // Process name
};

#define MAX_PROC 10

int main(int argc, char *argv[])
{
    struct proc ptable[MAX_PROC];
    struct proc *p;
    int err;

    err = getptable(10 * sizeof(struct proc), &ptable);
    if (err != 0)
        printf(1, "Error getting ptable");

    p = &ptable[0];
    printf(1, " PID\t\tPPID\t\tSTATE\t\t\tCMD\n");
    while (p != &ptable[MAX_PROC - 1] && p->state != UNUSED){
        if (p->ppid < 0) printf(1, " %d\t\t%d\t", p->pid, p->ppid);
        else             printf(1, " %d\t\t%d\t\t", p->pid, p->ppid);
        switch (p->state)
        {
        case UNUSED:
            printf(1, "%s\t\t", "UNUSED  ");
            break;
        case EMBRYO:
            printf(1, "%s\t\t", "EMBRYO  ");
            break;
        case SLEEPING:
            printf(1, "%s\t\t", "SLEEPING");
            break;
        case RUNNABLE:
            printf(1, "%s\t\t", "RUNNABLE");
            break;
        case RUNNING:
            printf(1, "%s\t\t", "RUNNING ");
            break;
        case ZOMBIE:
            printf(1, "%s\t\t", "ZOMBIE  ");
            break;
        }
        printf(1, "%s\n", p->name);
        p++;
    }

    exit();
}