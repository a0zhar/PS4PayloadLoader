#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#define pid_t jbc_pid_t
#include "jbcLib/jailbreak.h"
#undef pid_t

void* dlopen(const char* path, int mode);
void* dlsym(void* handle, const char* name);
int(*sceSysUtilSendSystemNotificationWithText)(int, const char*);
#define sendNoti(m) sceSysUtilSendSystemNotificationWithText(222,m); 


int main() {
    struct jbc_cred cr;
    jbc_get_cred(&cr);
    jbc_jailbreak_cred(&cr);
    jbc_set_cred(&cr);
    void* handle = dlopen("/system/common/lib/libSceSysUtil.sprx", 0);
    int(*sceSysUtilSendSystemNotificationWithText)(int, const char*) = dlsym(handle, "sceSysUtilSendSystemNotificationWithText");
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        sendNoti("sock: socket() error!");
        return 0;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = 0;
    addr.sin_port = 0x3d23;//9021
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        sendNoti("bind() error");
        close(sock);
    }
    if (listen(sock, 1) != 0) {
        sendNoti("listen() error");
        close(sock);
    }
    sendNoti("Waiting for Payload...");
    int sock2 = accept(sock, 0, 0);
    if (sock2 == -1) {
        sendNoti("sock2: accept() error!");
        close(sock);
        return 0;
    }
    void* mem = 0;
    size_t size = 0;
    size_t offset = 0;
    for (;;) {
        if (offset == size) {
            size_t new_size = size * 2;
            if (!new_size) new_size = 0x4000;
            void* newMappedMem = mmap(0,
                new_size,
                PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_PRIVATE | MAP_ANON,
                -1, 0
            );
            if (newMappedMem == MAP_FAILED) {
                sendNoti("mmap() failed");
                goto cleanup;
            }
            char* newMem = newMappedMem;
            char* oldMem = mem;
            for (size_t i = 0; i < size; i++) newMem[i] = oldMem[i];
            if (mem) {
                if (munmap(mem, size) == MAP_FAILED) {
                    sendNoti("munmap() failed");
                    goto cleanup;
                }
            }
            size = new_size;
            mem = newMappedMem;
        }
        ssize_t chunk = read(sock2, (char*)mem + offset, size - offset);
        if (chunk <= 0)break;
        offset += chunk;
    }
    sendNoti("Launching Payload!");
    ((void(*)(void))mem)();
cleanup:;
    close(sock2);
    close(sock);
    return 0;
}

