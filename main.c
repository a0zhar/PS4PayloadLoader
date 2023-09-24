#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <stdarg.h>

#define pid_t jbc_pid_t
#include "libJBC/jailbreak.h"
#undef pid_t

void *dlopen(const char *path, int mode);
void *dlsym(void *handle, const char *name);
int  dlclose(void *handle);

// Function used inside of the ps4 to send notifications
int   (*sceSysUtilSendSystemNotificationWithText)(int type, const char *msg);
void *(*memcpy)(void *dst, const void *src, size_t len);
int   (*vsnprintf)(char *str, size_t size, const char *format, va_list ap);

#define NOTIF_BUF_SIZE 500

// This function allows sending formatted notification messages, similar to printf.
int sendNotificationEx(const char *_Format, ...) {
    // PS4 Notification messages can contain a maximum of 1024 characters.
    // However, we use a stack buffer that can hold up to 500 characters.
    char msg_buff[NOTIF_BUF_SIZE];

    // We build the formatted string using arguments.
    va_list args;
    va_start(args, _Format);
    int result = vsnprintf(msg_buff, NOTIF_BUF_SIZE - 1, _Format, args);
    va_end(args);

    // Now we send the formatted message.
    sceSysUtilSendSystemNotificationWithText(222, msg_buff);

    // Return the value returned by vsnprintf.
    return result;
}

// This macro allows sending a standard PS4 Notification 
// using a non-formatted message.
#define SendNotification(message) sceSysUtilSendSystemNotificationWithText(222, message)

// This macro simplifies the usage of the dlopen function, 
// which is a programmatic interface to the dynamic linker 
// for loading one of the PS4 modules. It also initializes 
// a variable to contain a descriptor returned by dlopen.
#define loadModule(filename, output) output = dlopen("/system/common/lib/"filename, 0)

// This macro simplifies the usage of dlsym to import/hook 
// a function present within a loaded module on the PS4. 
// It assigns the function to one of our locally defined 
// function pointers, such as memcpy.
#define importFunction(moduleHandle, functionName) functionName = dlsym(moduleHandle, #functionName)


#define MAX_PAYLOAD_SIZE 0x4000
#define NULL (void*)0
#define _INVALID_SOCK -1
// Error return from mmap()
#define _MAP_FAILED	((void *)-1)

typedef int SOCKET;
// Main function
int main() {
    // Jailbreak the process
    struct jbc_cred credentials = {};
    jbc_get_cred(&credentials);
    jbc_jailbreak_cred(&credentials);
    jbc_set_cred(&credentials);

    // Handle of loaded libSceSysUtil.sprx PS4 module. 
    // Used for sceSysUtilSendSystemNotificationWithText
    void *hSysUtil = NULL;
    // Handle of loaded libSceLibcInternal.sprx PS4 module.
    // Used for vsnprintf, memcpy
    void *hLibc = NULL;

    // Load the two required modules from PS4 System.
    loadModule("libSceLibcInternal.sprx", hLibc);
    if (hLibc == NULL) return -1;

    loadModule("libSceSysUtil.sprx", hSysUtil);
    if (hSysUtil == NULL) {
        dlclose(hLibc);
        return -1;
    }

    // Import functions from the loaded modules
    importFunction(hSysUtil, sceSysUtilSendSystemNotificationWithText);
    importFunction(hLibc, memcpy);
    importFunction(hLibc, vsnprintf);

    // Send notification
    SendNotification("New Payload Launcher!\nCoded by A0zhar");

    // Initialize socket variables
    int s_sock = _INVALID_SOCK;
    int c_sock = _INVALID_SOCK;

    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (s_sock == _INVALID_SOCK) {
        SendNotification("socket() failed for server");
        goto cleanup;
    }

    struct sockaddr_in _server;
    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = 0;
    _server.sin_port = 0x3d23; // Port 9021

    // Bind socket
    if (bind(s_sock, (struct sockaddr *)&_server, sizeof(_server))) {
        SendNotification("bind() failed");
        goto cleanup;
    }

    // Listen for connections
    if (listen(s_sock, 1)) {
        SendNotification("listen() failed");
        goto cleanup;
    }

    // Accept incoming connection
    c_sock = accept(s_sock, 0, 0);
    if (c_sock == _INVALID_SOCK) {
        SendNotification("accept() failed");
        goto cleanup;
    }

    // Allocate memory for payload
    void *payload_memory = NULL;
    size_t pl_size = 0, pl_offset = 0;

    for (;;) {
        // Resize memory if necessary
        if (pl_offset == pl_size) {
            // Calculate the new size based on doubling the existing size or starting with MAX_PAYLOAD_SIZE
            size_t map_size = (pl_size == 0) ? MAX_PAYLOAD_SIZE : pl_size * 2;

            // Allocate new memory and check for errors
            void *mapped_mem = mmap(NULL, map_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
            if (mapped_mem == _MAP_FAILED) {
                SendNotification("mmap() failed");
                goto cleanup;
            }

            // Copy contents of the old payload memory to the new memory
            if (payload_memory != NULL) {
                memcpy(mapped_mem, payload_memory, pl_size);

                // Unmap the old payload memory
                if (munmap(payload_memory, pl_size) == -1) {
                    SendNotification("munmap failed");
                    goto cleanup;
                }
            }

            // Update pl_size and payload_memory
            pl_size = map_size;
            payload_memory = mapped_mem;
        }
        ssize_t bytesRead = read(
            c_sock,                      // Read data from the client socket.
            payload_memory + pl_offset,  // Read data into the payload memory buffer starting from pl_offset.
            pl_size - pl_offset          // Maximum number of bytes to read, which is the remaining space in the payload memory.
        );

        // If the length of the received data is less than or equal to 0, exit the loop
        if (bytesRead <= 0)
            break;

        // Increase pl_offset by the number of bytes just received
        pl_offset += bytesRead;
    }


    // If memory that should contain the payload data isn't NULL (default value)
    if (payload_memory != NULL) {
        // Notify the user that the payload is launching now
        SendNotification("Launching Payload");

        // Execute the payload_memory data as a function.
        ((void(*)(void))payload_memory)();
    }

cleanup:;
    // Close the client socket if it's not set to an invalid value
    if (c_sock != _INVALID_SOCK)
        close(c_sock);
    // Close the server socket if it's not set to an invalid value
    if (s_sock != _INVALID_SOCK)
        close(s_sock);

    dlclose(hLibc);
    dlclose(hSysUtil);
    return 0;
}
 