#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
#include "elf.h"
#include <sys/mman.h>
#include <string.h>


struct elf_header read_elf_header(const char *elfFile){
    // Open ELF file
    FILE *file = fopen(elfFile, "rb");
    printf("Opening binary %s\n", elfFile);             // Requirement of assignment 
    
    // Exit if file NULL
    if (file == NULL){
        printf("Error reading file!\n");
        exit(1);
    }

    struct elf_header header;
    // Read ELF file
    fread(&header, sizeof(struct elf_header), 1, file); 

    // Sanity Checks
    if (header.magic != ELF_MAGIC){
        printf("Error, this is not an ELF file!\n");
        exit(1);
    }
    if (header.machine != ELF_MACHINE_x64){
        printf("Error, wrong machine version!\n");
        exit(1);
    }
    if (header.version != ELF_VERSION){
        printf("Error, using wrong version!\n");
        exit(1);
    }
    
    // Close ELF file for read
    fclose(file);
    return header;
}

struct elf_proghdr* read_prog_headers(const char* elfFile, struct elf_header header) {
    int count = header.proghdr_count;
    FILE *file = fopen(elfFile, "rb");
    
    // List of program headers
    struct elf_proghdr *progHeaders = malloc(count * sizeof(struct elf_proghdr));

    // Move to the first Program Header file location
    fseek(file, header.proghdr_offset, SEEK_SET);

    // Read all Program Headers into the array
    fread(progHeaders,sizeof(struct elf_proghdr) * header.proghdr_count, 1,file);

    fclose(file);
    return progHeaders;
}

int main(int argc, char *argv[]) {
    struct elf_header header = read_elf_header("helloworld_static");
    struct elf_proghdr *progHeaders = read_prog_headers("helloworld_static", header);

    uint64_t totalSpan = 0;
    // iterate through array of program headers
    for (int i = 0; i < header.proghdr_count; i++) {
        if (progHeaders[i].type == ELF_PROG_LOAD) {
            // These are the Program Headers that are of type 'LOAD'
            totalSpan += progHeaders[i].size_memory + progHeaders[i].virtual_address;
        }
    }

    // Allocating memory using mmap()
    void *base = mmap(NULL, totalSpan, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Map the file into memory
    int fd = open("helloworld_static", O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    void *fileBase = mmap(NULL, header.file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (fileBase == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Fill allocated memory with binary contents, taking into account all offsets to preserve structure
    for (int i = 0; i < header.proghdr_count; i++) {
        if (progHeaders[i].type == ELF_PROG_LOAD) {
            memcpy(base + progHeaders[i].virtual_address, fileBase + progHeaders[i].file_offset, progHeaders[i].size_memory);
        }
    }

    // Unmap the file and allocated memory
    if (munmap(fileBase, header.file_size) == -1) {
        perror("munmap");
        exit(1);
    }
    if (munmap(base, totalSpan) == -1) {
        perror("munmap");
        exit(1);
    }

    free(progHeaders);
    return 0;
}

// Load the binary file into memory using fread(). It will return a pointer to the beginning of the binary
// in memory.

// Allocate memory for the binary file using mmap()

// Set proper permissions using of the chunk of memory using mprotect(). " This function accepts a bitwise-or 
// of flags for read, write, and execute"

// "The ‘virtual address’ is where the ELF binary expects us to load the segment into memory"