#pragma once
#ifndef UTILS_H_
#define UTILS_H_
#include "defs.h"

#define CWD_KEEP  0
#define CWD_ROOT  1
#define CWD_RESET 2
#define EINVAL 22

struct iovec {
    const void* ptr;
    size_t size;
};

struct mount_in_sandbox_param {
    const char* path;
    const char* name;
    int ans;
};

#define SYSCALL(nr, function) \
__attribute__((naked)) \
function { \
    asm volatile("mov $" #nr ", %rax\nmov %rcx, %r10\nsyscall\nret"); \
}

void jbc_run_as_root(void(*fn)(void* arg), void* arg, int cwd_mode);
int jbc_mount_in_sandbox(const char* system_path, const char* mnt_name);
int jbc_unmount_in_sandbox(const char* mnt_name);

#endif  // UTILS_H_