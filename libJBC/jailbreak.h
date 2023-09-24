#pragma once
#ifndef JAILBREAK_H_
#define JAILBREAK_H_
#include "defs.h"

// Using https://man.freebsd.org/ as Source for these comments
struct jbc_cred {
    uid_t uid;            // effective user ID (alias euid)
    uid_t ruid;           // real user ID
    uid_t svuid;          // saved UID from a setuid executable
    gid_t rgid;           // real group ID
    gid_t svgid;          // saved gid from a setgid executable
    uintptr_t prison;     // i think prison structure associated with the process
    uintptr_t cdir;       // current directory
    uintptr_t rdir;       // root directory
    uintptr_t jdir;       // jail directory
    uint64_t sceProcType; // value indicating the type of SCE process.
    uint64_t sonyCred;    // credential associated with Sony.
    uint64_t sceProcCap;  // the capability of the SCE process.
};

uintptr_t jbc_get_prison0(void);
uintptr_t jbc_get_rootvnode(void);
int jbc_get_cred(struct jbc_cred* cred);
int jbc_jailbreak_cred(struct jbc_cred* cred);
int jbc_set_cred(const struct jbc_cred* cred);

#endif