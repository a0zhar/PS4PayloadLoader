#include "defs.h"
#include "kernelrw.h"
#include "jailbreak.h"

// Assembly definitions for open and close system calls
asm("open:\nmov $5, %rax\nmov %rcx, %r10\nsyscall\nret");
asm("close:\nmov $6, %rax\nmov %rcx, %r10\nsyscall\nret");

// Function prototypes for open and close
int open(const char*, int);
int close(int);

// Variables to hold prison0 and rootvnode addresses
static uintptr_t prison0;
static uintptr_t rootvnode;

// Helper function to resolve prison0 and rootvnode addresses
static int resolve(void) {
restart:;
    uintptr_t td = jbc_krw_get_td(); // Get the current thread structure
    uintptr_t proc = jbc_krw_read64(td + 8, KERNEL_HEAP); // Get the process structure from the thread structure
    for (;;) {
        int pid;
        if (jbc_krw_memcpy((uintptr_t)&pid, proc + 0xb0, sizeof(pid), KERNEL_HEAP)) // Read the process ID from the process structure
            goto restart;
        if (pid == 1) // Check if the process ID is 1 (init process)
            break;
        uintptr_t proc2 = jbc_krw_read64(proc, KERNEL_HEAP); // Get the parent process structure
        uintptr_t proc1 = jbc_krw_read64(proc2 + 8, KERNEL_HEAP); // Get the grandparent process structure
        if (proc1 != proc) // Check if the grandparent process structure is the same as the current process structure
            goto restart;
        proc = proc2; // Move to the parent process structure
    }
    uintptr_t pid1_ucred = jbc_krw_read64(proc + 0x40, KERNEL_HEAP); // Get the ucred structure of the process
    uintptr_t pid1_fd = jbc_krw_read64(proc + 0x48, KERNEL_HEAP); // Get the file descriptor structure of the process
    if (jbc_krw_memcpy((uintptr_t)&prison0, pid1_ucred + 0x30, sizeof(prison0), KERNEL_HEAP)) // Read the prison0 address from the ucred structure
        return -1;
    if (jbc_krw_memcpy((uintptr_t)&rootvnode, pid1_fd + 0x18, sizeof(rootvnode), KERNEL_HEAP)) { // Read the rootvnode address from the file descriptor structure
        prison0 = 0;
        return -1;
    }
    return 0;
}

// Get the address of prison0
uintptr_t jbc_get_prison0(void) {
    if (!prison0) resolve(); // Resolve the addresses if not already resolved
    return prison0;
}

// Get the address of rootvnode
uintptr_t jbc_get_rootvnode(void) {
    if (!rootvnode) resolve(); // Resolve the addresses if not already resolved
    return rootvnode;
}

// Helper function to copy data from user space to kernel space
static inline int ppcopyout(void* u1, void* u2, uintptr_t k) {
    return jbc_krw_memcpy((uintptr_t)u1, k, (uintptr_t)u2 - (uintptr_t)u1, KERNEL_HEAP);
}

// Helper function to copy data from kernel space to user space
static inline int ppcopyin(const void* u1, const void* u2, uintptr_t k) {
    return jbc_krw_memcpy(k, (uintptr_t)u1, (uintptr_t)u2 - (uintptr_t)u1, KERNEL_HEAP);
}

// Get the current process credentials
int jbc_get_cred(struct jbc_cred* ans) {
    uintptr_t td = jbc_krw_get_td(); // Get the current thread structure
    uintptr_t proc = jbc_krw_read64(td + 8, KERNEL_HEAP); // Get the process structure from the thread structure
    uintptr_t ucred = jbc_krw_read64(proc + 0x40, KERNEL_HEAP); // Get the ucred structure of the process
    uintptr_t fd = jbc_krw_read64(proc + 0x48, KERNEL_HEAP); // Get the file descriptor structure of the process

    if (ppcopyout(&ans->uid, 1 + &ans->svuid, ucred + 4) // Copy user IDs (uid and svuid) from ucred structure to ans structure
        || ppcopyout(&ans->rgid, 1 + &ans->svgid, ucred + 8) // Copy group IDs (rgid and svgid) from ucred structure to ans structure
        || ppcopyout(&ans->groups, 16 + &ans->groups, ucred + 0x10) // Copy group array from ucred structure to ans structure
        || ppcopyout(&ans->prison, 1 + &ans->prison, prison0) // Copy the prison address from prison0 to ans structure
        || ppcopyout(&ans->rootvnode, 1 + &ans->rootvnode, rootvnode) // Copy the rootvnode address from rootvnode to ans structure
        || ppcopyout(&ans->jail, 1 + &ans->jail, proc + 0x38) // Copy the jail address from process structure to ans structure
        || ppcopyout(&ans->td_ucred, 1 + &ans->td_ucred, td + 0x350) // Copy the td_ucred address from thread structure to ans structure
        || ppcopyout(&ans->td_retval, 1 + &ans->td_retval, td + 0x3a8)) // Copy the td_retval address from thread structure to ans structure
        return -1;
   
    return 0;
}
