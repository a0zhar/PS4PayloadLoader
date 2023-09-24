#include "jailbreak.h"
#include "utils.h"

void jbc_run_as_root(void(*fn)(void* arg), void* arg, int cwd_mode) {
    struct jbc_cred cred;
    jbc_get_cred(&cred);
    struct jbc_cred root_cred = cred;
    jbc_jailbreak_cred(&root_cred);
    switch (cwd_mode) {
        case CWD_KEEP: default: root_cred.cdir = cred.cdir; break;
        case CWD_ROOT:
            root_cred.cdir = cred.rdir;
            break;
        case CWD_RESET:
            root_cred.cdir = root_cred.rdir;
            break;
    };
    jbc_set_cred(&root_cred);
    fn(arg);
    jbc_set_cred(&cred);
}

SYSCALL(12, static int chdir(char* path))
SYSCALL(22, static int unmount(const char* path, int flags))
SYSCALL(136, static int mkdir(const char* path, int mode))
SYSCALL(137, static int rmdir(const char* path))
SYSCALL(326, static int getcwd(char* buf, size_t sz))
SYSCALL(378, static int nmount(struct iovec* iov, unsigned int niov, int flags))

#define MAX_PATH 1024

static void do_mount_in_sandbox(void* op) {
    struct mount_in_sandbox_param* p = op;
    char path[MAX_PATH + 1];
    path[MAX_PATH] = 0;
    int result;
    // Get working directory pathname
    if (result = getcwd(path, MAX_PATH))
        goto err_out;

    size_t pathlen = 0;
    while (path[pathlen]) pathlen++;

    size_t l2 = 0;
    while (p->name[l2]) l2++;

    if (pathlen + l2 + 1 >= MAX_PATH)
        goto invalid;
    path[pathlen] = '/';
    int dots = 1;
    for (size_t i = 0; i <= l2; i++) {
        if (p->name[i] == '/')
            goto invalid;
        else if (p->name[i] != '.')
            dots = 0;
        path[pathlen + 1 + i] = p->name[i];
    }
    if (dots && l2 <= 2)
        goto invalid;
    //mount operation
    if (p->path) {
        if ((result = mkdir(path, 0777)))
            goto err_out;
        size_t l3 = 0;
        while (p->path[l3])
            l3++;
        struct iovec data[6] = {
            { "fstype", 7 }, { "nullfs", 7 },
            { "fspath", 7 }, { path, pathlen + l2 + 2 },
            { "target", 7 }, { p->path, l3 + 1 },
        };
        if ((result = nmount(data, 6, 0))) {
            rmdir(path);
            goto err_out;
        }

    } else { //unmount operation
        if ((result = unmount(path, 0)))
            goto err_out;
        if ((result = rmdir(path)))
            goto err_out;
    }
    return;
invalid:
    result = EINVAL;
err_out:
    p->ans = result;
}

int jbc_mount_in_sandbox(const char* system_path, const char* mnt_name) {
    struct mount_in_sandbox_param op = {
        .path = system_path,
        .name = mnt_name,
        .ans = 0,
    };
    jbc_run_as_root(do_mount_in_sandbox, &op, CWD_ROOT);
    return op.ans;
}

int jbc_unmount_in_sandbox(const char* mnt_name) {
    return jbc_mount_in_sandbox(0, mnt_name);
}