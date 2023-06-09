#include "kernelrw.h"
#include "jailbreak.h"

#define count_of(arr) sizeof(arr) / sizeof(arr[0])

asm("open:\nmov $5, %rax\nmov %rcx, %r10\nsyscall\nret");
int open(const char*, int);
asm("close:\nmov $6, %rax\nmov %rcx, %r10\nsyscall\nret");
int close(int);

static void* fake_vtable[16];
static uintptr_t prison0;   
static uintptr_t rootvnode; 

// Function resolves the process list up to PID 1 (init) and copies
// its prison and root directory into the calling process
static int resolve(void) {
restart:;
    uintptr_t thread = jbc_krw_get_td();
    uintptr_t proc = jbc_krw_read64(thread + 8, KERNEL_HEAP);
    int processid;
    for (;;) {
        // Copy data of Address at (<proc> + 0xb0) into processid variable
        if (jbc_krw_memcpy(
            (uintptr_t)&processid,
            proc + 0xb0,
            sizeof(processid),
            KERNEL_HEAP
        )) goto restart;

        if (processid == 1) break;

        uintptr_t proc2 = jbc_krw_read64(proc, KERNEL_HEAP);
        uintptr_t proc1 = jbc_krw_read64(proc2 + 8, KERNEL_HEAP);
        if (proc1 != proc)
            goto restart;
        proc = proc2;
    }
    uintptr_t pid1_ucred = jbc_krw_read64(proc + 0x40, KERNEL_HEAP);
    uintptr_t pid1_fd    = jbc_krw_read64(proc + 0x48, KERNEL_HEAP);
    if (jbc_krw_memcpy((uintptr_t)&prison0, pid1_ucred + 0x30, sizeof(prison0), KERNEL_HEAP))
        return -1;
    if (jbc_krw_memcpy((uintptr_t)&rootvnode, pid1_fd + 0x18, sizeof(rootvnode), KERNEL_HEAP)) {
        prison0 = 0;
        return -1;
    }
    return 0;
}

// Returns the prison0 address, resolving it if necessary
uintptr_t jbc_get_prison0(void) {
    if (!prison0) 
        resolve();
    return prison0;
}
// Function Returns the rootvnode address, resolving it if necessary
uintptr_t jbc_get_rootvnode(void) {
    if (!rootvnode)
        resolve();
    return rootvnode;
}
// Function Copies data from user space to kernel space
static inline int ppcopyout(void* u1, void* u2, uintptr_t k) {
    return jbc_krw_memcpy((uintptr_t)u1, k, (uintptr_t)u2 - (uintptr_t)u1, KERNEL_HEAP);
}
// Function Copies data from kernel space to user space
static inline int ppcopyin(const void* u1, const void* u2, uintptr_t k) {
    return jbc_krw_memcpy(k, (uintptr_t)u1, (uintptr_t)u2 - (uintptr_t)u1, KERNEL_HEAP);
}

// Function Gets the current process's credentials and stores them
// in the provided structure
int jbc_get_cred(struct jbc_cred* ans) {
    uintptr_t td = jbc_krw_get_td();
    uintptr_t proc = jbc_krw_read64(td + 8, KERNEL_HEAP);
    uintptr_t ucred = jbc_krw_read64(proc + 0x40, KERNEL_HEAP);
    uintptr_t fd = jbc_krw_read64(proc + 0x48, KERNEL_HEAP);

    if (ppcopyout(&ans->uid, 1 + &ans->svuid, ucred + 4)
        || ppcopyout(&ans->rgid, 1 + &ans->svgid, ucred + 20)
        || ppcopyout(&ans->prison, 1 + &ans->prison, ucred + 0x30)
        || ppcopyout(&ans->cdir, 1 + &ans->jdir, fd + 0x10)
        || ppcopyout(&ans->sceProcType, 1 + &ans->sceProcCap, ucred + 88))
        return -1;

    return 0;
}

// Function Sets the current process's credentials to the provided
// values.
static int jbc_set_cred_internal(const struct jbc_cred* ans) {
    uintptr_t td = jbc_krw_get_td();
    uintptr_t proc = jbc_krw_read64(td + 8, KERNEL_HEAP);
    uintptr_t ucred = jbc_krw_read64(proc + 0x40, KERNEL_HEAP);
    uintptr_t fd = jbc_krw_read64(proc + 0x48, KERNEL_HEAP);

    if (ppcopyin(&ans->uid, 1 + &ans->svuid, ucred + 4)
        || ppcopyin(&ans->rgid, 1 + &ans->svgid, ucred + 20)
        || ppcopyin(&ans->prison, 1 + &ans->prison, ucred + 0x30)
        || ppcopyin(&ans->cdir, 1 + &ans->jdir, fd + 0x10)
        || ppcopyin(&ans->sceProcType, 1 + &ans->sceProcCap, ucred + 88))
        return -1;
    return 0;
}

// Escapes the sandbox by setting the current process's credentials
// to jailbreak credentials
int jbc_jailbreak_cred(struct jbc_cred* ans) {
    uintptr_t prison0, rootvnode;

    prison0 = jbc_get_prison0();
    if (!prison0)
        return -1;

    rootvnode = jbc_get_rootvnode();
    if (!rootvnode)
        return -1;

    //without some modules wont load like Apputils
    ans->sceProcCap = 0xffffffffffffffff;
    ans->sceProcType = 0x3801000000000013;
    ans->sonyCred = 0xffffffffffffffff;
    ans->uid = ans->ruid = ans->svuid = ans->rgid = ans->svgid = 0;
    ans->prison = prison0;
    ans->cdir = ans->rdir = ans->jdir = rootvnode;
    return 0;
}

// Function opens a file or directory using the provided credentials
// and vnode address
static int jbc_open_this(const struct jbc_cred* cred0, uintptr_t vnode) {
    if (!vnode) return -1;
    struct jbc_cred cred = *cred0;
    jbc_jailbreak_cred(&cred);
    cred.cdir = cred.rdir = cred.jdir = vnode;
    jbc_set_cred_internal(&cred);
    int ans = open("/", 0);
    jbc_set_cred_internal(cred0);
    return ans;
}

// Swaps the values at two memory addresses
static int swap64(uintptr_t p1, KmemKind k1, uintptr_t p2, KmemKind k2) {
    uintptr_t value1 = jbc_krw_read64(p1, k1);
    uintptr_t value2 = jbc_krw_read64(p2, k2);

    if (ppcopyin(&value2, 1 + &value2, p1) ||
        ppcopyin(&value1, 1 + &value1, p2))
        return -1;

    return 0;
}

// Helper function that always returns 0
static int return0(void) { return 0; }

int jbc_set_cred(const struct jbc_cred* newp) {
    struct jbc_cred old_cred;
    if (jbc_get_cred(&old_cred))
        return -1; // get the current process's credentials

    struct jbc_cred new_cred = *newp; // copy the new credentials

    if (old_cred.cdir && !new_cred.cdir) new_cred.cdir = jbc_get_rootvnode();
    if (old_cred.rdir && !new_cred.rdir) new_cred.rdir = jbc_get_rootvnode();
    if (old_cred.jdir && !new_cred.jdir) new_cred.jdir = jbc_get_rootvnode();

    int cdir_fd = jbc_open_this(&old_cred, new_cred.cdir);
    int rdir_fd = jbc_open_this(&old_cred, new_cred.rdir);
    int jdir_fd = jbc_open_this(&old_cred, new_cred.jdir);

    // create a new set of credentials with the old credentials' directories
    struct jbc_cred elevated_cred = new_cred;
    elevated_cred.cdir = old_cred.cdir;
    elevated_cred.jdir = old_cred.jdir;
    elevated_cred.rdir = old_cred.rdir;

    // this offset is the same in 5.05-9.60, probably safe to use
    uint32_t rc;
    
    if (elevated_cred.prison) {
        if (jbc_krw_memcpy((uintptr_t)&rc, elevated_cred.prison + 0x14, sizeof(rc), KERNEL_HEAP) &&
            jbc_krw_memcpy((uintptr_t)&rc, elevated_cred.prison + 0x14, sizeof(rc), KERNEL_TEXT))
            return -1;
        rc++;
        if (jbc_krw_memcpy(elevated_cred.prison + 0x14, (uintptr_t)&rc, sizeof(rc), KERNEL_HEAP) &&
            jbc_krw_memcpy(elevated_cred.prison + 0x14, (uintptr_t)&rc, sizeof(rc), KERNEL_TEXT))
            return -1;
    }
    if (jbc_set_cred_internal(&elevated_cred)) return -1;
    if (old_cred.prison) {
        if (jbc_krw_memcpy((uintptr_t)&rc, old_cred.prison + 0x14, sizeof(rc), KERNEL_HEAP) &&
            jbc_krw_memcpy((uintptr_t)&rc, old_cred.prison + 0x14, sizeof(rc), KERNEL_TEXT))
            return -1;
        rc--;
        if (jbc_krw_memcpy(old_cred.prison + 0x14, (uintptr_t)&rc, sizeof(rc), KERNEL_HEAP) &&
            jbc_krw_memcpy(old_cred.prison + 0x14, (uintptr_t)&rc, sizeof(rc), KERNEL_TEXT))
            return -1;
    }

    uintptr_t thread = jbc_krw_get_td();
    uintptr_t proc = jbc_krw_read64(thread + 8, KERNEL_HEAP);
    uintptr_t user_credentials = jbc_krw_read64(proc + 0x40, KERNEL_HEAP);
    uintptr_t fd = jbc_krw_read64(proc + 0x48, KERNEL_HEAP);
    uintptr_t ofiles = jbc_krw_read64(fd, KERNEL_HEAP);
    int dir_file_descriptor[3] = { cdir_fd, rdir_fd, jdir_fd };
    uintptr_t offset, file;

    for (int i = 0; i < 3; i++) {
        if (dir_file_descriptor[i] < 0)
            continue;

        offset = ofiles + 8 * dir_file_descriptor[i];
        file = jbc_krw_read64(offset, KERNEL_HEAP);

        if (swap64(file, KERNEL_HEAP, fd + 0x10 + 8 * i, KERNEL_HEAP)) return -1;
        if (jbc_krw_read64(file, KERNEL_HEAP) == 0) {
            if (!fake_vtable[0]) {
                for (int i = 0; i < count_of(fake_vtable); i++)
                    fake_vtable[i] = return0;
            }
            uintptr_t p = (uintptr_t)fake_vtable;
            if (ppcopyin(&p, 1 + &p, file + 8))
                return -1;
        }
        close(dir_file_descriptor[i]);
    }
    return 0;
}
