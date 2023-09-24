#pragma once
#ifndef DEFS_H_
#define DEFS_H_

#include <stdint.h>   // Add this line to include the stdint.h header

// Add necessary includes for other header files used in the code

typedef long int int64_t;   // Define int64_t if it's not available

typedef int64_t ssize_t;
typedef int64_t off_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t pid_t;

// Add missing declarations for unknown types and functions

typedef unsigned long size_t;
typedef long int ssize_t;

typedef long int intptr_t;
typedef unsigned long int uintptr_t;

#define AF_UNIX 1
#define SOCK_STREAM 1

int socketpair(int domain, int type, int protocol, int* out);
ssize_t read(int fd, void* dst, size_t sz);
ssize_t write(int fd, const void* dst, size_t sz);
int close(int fd);

#endif