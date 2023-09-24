// https://gist.github.com/flatz/1055a8d7819c8478db1b464842582c9c
#include <sys/types.h>
#include <stddef.h>

// Define a structure to hold information about segments within a module
struct module_segment {
	uint64_t addr;  // Address of the segment
	uint32_t size;  // Size of the segment
	uint32_t flags; // Flags associated with the segment
};

// Define a structure to hold detailed information about a loaded module
struct module_info_ex {
	size_t st_size;               // Size of the structure itself
	char name[256];               // Name of the module
	int id;                       // Module ID
	uint32_t tls_index;           // Thread-local storage (TLS) index
	uint64_t tls_init_addr;       // Address of the TLS initialization function
	uint32_t tls_init_size;       // Size of the TLS initialization information
	uint32_t tls_size;            // Total size of the TLS data
	uint32_t tls_offset;          // Offset of the TLS data within the module
	uint32_t tls_align;           // Alignment of the TLS data
	uint64_t init_proc_addr;      // Address of the module initialization function
	uint64_t fini_proc_addr;      // Address of the module finalization function
	uint64_t reserved1;           // Reserved field
	uint64_t reserved2;           // Reserved field
	uint64_t eh_frame_hdr_addr;   // Address of the exception handling frame header
	uint64_t eh_frame_addr;       // Address of the exception handling frame
	uint32_t eh_frame_hdr_size;   // Size of the exception handling frame header
	uint32_t eh_frame_size;       // Size of the exception handling frame
	struct module_segment segments[4]; // Array of module segments (up to 4)
	uint32_t segment_count;       // Number of module segments
	uint32_t ref_count;           // Reference count for the module
};

long long dynlib_load_prx(const char *path, int flags, int *handle, int reserved);
int       dynlib_get_info_ex(int handle, int id, struct module_info_ex *info);
long long dynlib_dlsym(int handle, const char *name, void **addr);
long long dynlib_dlclose(int handle);

// Function to load a dynamic library with additional data
void *dlopen_ex(const char *path, int mode, void *data, size_t data_len) {
	int handle = 0;

	// Attempt to load the dynamic library 
	if (dynlib_load_prx(path, 0, &handle, 0))
		return NULL;// Return NULL upon failure

	struct module_info_ex mi;
	mi.st_size = sizeof(mi);

	// Retrieve detailed module information
	if (dynlib_get_info_ex(handle, 0, &mi))
		return NULL;// Return NULL upon failure

	// Check if the module reference count is less than 2
	if (mi.ref_count < 2) {
		// Cast the module's initialization function and call it
		((int(*)(size_t, void *, void *))mi.init_proc_addr)(data_len, data, NULL);
	}
	// Return the handle
	return (void *)(intptr_t)handle;
}

void *dlopen(const char *path, int mode) {
	return dlopen_ex(path, mode, NULL, 0);
}

void *dlsym(void *handle, const char *name) {
	void *addr = NULL;
	dynlib_dlsym((int)(intptr_t)handle, name, &addr);
	return addr;
} 
int dlclose(void *handle) {
	return dynlib_dlclose((int)(intptr_t)handle);
}