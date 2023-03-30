#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
#include "elf.h"
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

static int PAGESIZE = 4096;

struct elf_header read_elf_header(FILE *file) {
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
    
    return header;
}

struct elf_proghdr* read_prog_headers(FILE *file, struct elf_header header) {
    int count = header.proghdr_count;
    
    // List of program headers
    struct elf_proghdr *progHeaders = malloc(count * sizeof(struct elf_proghdr));

    // Move to the first Program Header file location
    fseek(file, header.proghdr_offset, SEEK_SET);

    // Read all Program Headers into the array
    fread(progHeaders, sizeof(struct elf_proghdr) * header.proghdr_count, 1, file);

    // fclose(file);
    return progHeaders;
}

int main(int argc, char *argv[], char *envp[]) { 
    // Open ELF file
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL)   {                   // Error checking if file not openable
        printf("Error reading file!\n");
        exit(1);
    }
    printf("Opening binary %s\n", argv[1]);
    
    struct elf_header header = read_elf_header(file);
    struct elf_proghdr *progHeaders = read_prog_headers(file, header);
    
    uint64_t maxSize = progHeaders[0].virtual_address + progHeaders[0].size_memory;
    uint64_t minSize = progHeaders[0].virtual_address;
    uint64_t totalSpan = 0;
    // iterate through array of program headers
    for (int i = 0; i < header.proghdr_count; i++) {
        if (progHeaders[i].type == ELF_PROG_LOAD) {
            // Part 2a
            // Determine the maximum span of all loadable segments
            minSize = MIN(minSize, progHeaders[i].virtual_address);
            maxSize = MAX(maxSize, progHeaders[i].virtual_address + progHeaders[i].size_memory);
        }
    }
    // Roundup to the closest page size
    totalSpan = ROUNDUP(maxSize - minSize, PAGESIZE);

    // Part 2a
    // Allocating memory for program headers using mmap()
    void *base = mmap(NULL, totalSpan, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Part 2b
    // Fill allocated memory with binary contents, taking into account all offsets to preserve structure
    for (int i = 0; i < header.proghdr_count; i++) {
        if (progHeaders[i].type == ELF_PROG_LOAD) {
            // Check if this memcpy works
            fseek(file, progHeaders[i].file_offset, SEEK_SET);
            fread(progHeaders[i].virtual_address + base, progHeaders[i].size_file, 1, file);
        }
    }
    
    // Part 2c
    // We're assuming this works
    // Set protection flags on memory regions based on program header flags
    for (int i = 0; i < header.proghdr_count; i++) {
        if (progHeaders[i].type == ELF_PROG_LOAD) {
            int prot = 0;
            if (progHeaders[i].flags & ELF_PROG_FLAG_READ) {
                prot |= PROT_READ;
            }
            if (progHeaders[i].flags & ELF_PROG_FLAG_WRITE) {
                prot |= PROT_WRITE;
            }
            if (progHeaders[i].flags & ELF_PROG_FLAG_EXEC) {
                prot |= PROT_EXEC;
            }
            void *segmentAddy = (void *) ROUNDDOWN(progHeaders[i].virtual_address + (uint64_t)base, PAGESIZE);
            size_t segmentSize = ROUNDUP(progHeaders[i].size_memory, PAGESIZE);
            mprotect(segmentAddy, segmentSize, prot);
        }
    }

    // Part 4
    // Construct a fresh stack for the new program with the exact structure as described in appendix C
    int stackSize = 5;
    for (int i = 1; i < argc; i++) {
        stackSize++;
    }
    int i;
    for (i = 0; envp[i] != NULL; i++) {
        stackSize++;
    }
    struct elf_auxvec *temp;
    temp = (struct elf_auxvec*) &envp[i+1];
    
    for (; temp->type != AT_NULL; temp++) {
        stackSize += 2;
    }

    // Allocate stack memory
    int index = 1;
    uint64_t *stack = alloca(stackSize * sizeof(uint64_t));
    memset(stack, 0, stackSize * sizeof(uint64_t));

    stack[0] = (uint64_t) argc - 1;
    for (int i = 1; i < argc; i++) {
        stack[index] = (uint64_t) argv[i];
        index++;
    }
    stack[index] = (uint64_t) NULL;        
    index++;
    for (int i = 0; envp[i] != NULL; i++) {
        stack[index] = (uint64_t) envp[i];
        index++;
    }
    stack[index] = (uint64_t) NULL;
    index++;

    struct elf_auxvec *auxv;
    while (*argv++ != NULL);    // walk past all arg pointers
    while (*envp++ != NULL);    // walk past all env pointers
    auxv = (struct elf_auxvec *)envp;    // find ELF aux vector

    for (; auxv->type != AT_NULL; auxv++) {
        // change values at types 3, 4, 5 and 9
        stack[index] = (uint64_t) auxv->type;
        index++;
        if (auxv->type == AT_PHDR) {
            stack[index] = (uint64_t) (base + header.proghdr_offset);
        } else if (auxv->type == AT_PHENT) {
            stack[index] = (uint64_t) header.proghdr_size;
        } else if (auxv->type == AT_PHNUM) {
            stack[index] = (uint64_t) header.proghdr_count;
        } else if (auxv->type == AT_ENTRY) {
            stack[index] = (uint64_t) (base + header.entry);
        } else {
            stack[index] = (uint64_t) auxv->value;
        }
        index++;
    }
    stack[index] = (uint64_t) NULL;
    
    // Last element of stack needs to be NULL
    stack[stackSize - 1] = (uint64_t) NULL;

    // Part 5: Set the exit point, load the stack pointer and jump to the entry point
    jump(base + header.entry, stack);
    
    // Freeing allocated mmap memory
    if (munmap(base, totalSpan) == -1) {
        perror("munmap");
        exit(1);
    }
    fclose(file);
    free(progHeaders);
    finalize();
    return 0;
}
