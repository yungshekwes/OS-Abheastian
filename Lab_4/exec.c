#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
#include "elf.h"


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
    
    // Moving to the first program header location
    for (int i = 0; i < count; i++) {
        fseek(file, header.proghdr_offset+(i*header.proghdr_size), SEEK_SET);
        fread(&progHeaders[i], sizeof(struct elf_proghdr), 1, file);
    }
    fclose(file);
    return progHeaders;
}

int main(int argc, char *argv[]) {
    struct elf_header header = read_elf_header("helloworld_static");
    struct elf_proghdr *progHeaders = read_prog_headers("helloworld_static", header);
    for (int i = 0; i < header.proghdr_count; i++){
        printf("Prog header size of file: %ld for idx %d\n", progHeaders[i].size_file, i);
    }
    // printf("Main header size = %d\nProgram header size = %d\nSection header size = %d\nProgram header count = %d", 
    // header.header_size, header.proghdr_size, header.secthdr_size, header.proghdr_count);
    free(progHeaders);
    return 0;
}

// Load the binary file into memory using fread(). It will return a pointer to the beginning of the binary
// in memory.

// Allocate memory for the binary file using mmap()

// Set proper permissions using of the chunk of memory using mprotect(). " This function accepts a bitwise-or 
// of flags for read, write, and execute"

// "The ‘virtual address’ is where the ELF binary expects us to load the segment into memory"