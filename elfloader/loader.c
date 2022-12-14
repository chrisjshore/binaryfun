#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <assert.h>
#include <stdbool.h>

int is_image_valid(Elf32_Ehdr *hdr)
{
    // Check that the file starts with the magic ELF number
    // 0x7F followed by ELF(45 4c 46) in ASCII
    assert(hdr->e_ident[EI_MAG0] == 0x7F);
    assert(hdr->e_ident[EI_MAG1] == 0x45);
    assert(hdr->e_ident[EI_MAG2] == 0x4c);
    assert(hdr->e_ident[EI_MAG3] == 0x46);

    return 1;
}

void* resolve(const char* sym, Elf32_Dyn* dyns, const char* dyn_strings)
{
    static void *handle = NULL;
    int i = 0;
    const char* lib;

    while(dyns[i].d_tag != DT_NULL)
    {
        if(dyns[i].d_tag == DT_NEEDED || dyns[i].d_tag == DT_RPATH || dyns[i].d_tag == DT_RUNPATH)
        {
            lib = dyn_strings + dyns[i].d_un.d_val;
            handle = dlopen(lib, RTLD_NOW);
            
            dlerror();
            void* resolved_sym = dlsym(handle, sym);
            //printf("resolved: %d\n", resolved_sym);
            char* error = dlerror();
            
            if(error != NULL){                
                printf("%s\n", error);
                i++;
                continue;
            }
            //dlclose(handle);
            
            return resolved_sym;
        }
        i++;
    }
    
    //if (handle == NULL)
    //{
        //handle = dlopen("libc.so.6", RTLD_NOW);
        //handle = dlopen((const char*)0, RTLD_NOW);
        //handle = dlopen(lib, RTLD_NOW);
    //}

    //assert(handle != NULL);

    ///void* resolved_sym = dlsym(handle, sym);

    // assert(resolved_sym != NULL);

    ///return resolved_sym;
    return NULL;
}

void relocate(Elf32_Shdr* shdr, const Elf32_Sym* syms, Elf32_Dyn* dyns, const char* dyn_strings, const char* strings, const char* src, char* dst)
{
    Elf32_Rel* rel = (Elf32_Rel*)(src + shdr->sh_offset);

    for(int j = 0; j < shdr->sh_size / sizeof(Elf32_Rel); j += 1)
    {
        const char* sym = strings + syms[ELF32_R_SYM(rel[j].r_info)].st_name;
        
        if(syms[ELF32_R_SYM(rel[j].r_info)].st_info == 0x20) continue;  // possibly STB_WEAK symbol binding, skip it...

        switch (ELF32_R_TYPE(rel[j].r_info))
        {
            case R_386_JMP_SLOT:
            case R_386_GLOB_DAT:
                //*(Elf32_Word*)(dst + rel[j].r_offset) = (Elf32_Word)resolve(sym);
                *(Elf32_Word*)(dst + rel[j].r_offset) = (Elf32_Word)resolve(sym, dyns, dyn_strings);
                break;
        }
    }
}

int find_dynstr_section(Elf32_Ehdr* hdr, Elf32_Shdr* shdr, const char* section_strings)
{
    for (int i = 0; i < hdr->e_shnum; i++)
    {
        const char* dynstr = section_strings + shdr[i].sh_name;

        if(strcmp(dynstr, ".dynstr") == 0){
            return i;
        }
    }

    return -1;
}

int find_dynamic_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr)
{
    for (int i = 0; i < hdr->e_shnum; i++)
    {       
        if (shdr[i].sh_type == SHT_DYNAMIC)
        {
            return i;
            break;
        }   
    }

    return -1;
}

int find_global_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr)
{
    for (int i = 0; i < hdr->e_shnum; i++)
    {
        if (shdr[i].sh_type == SHT_DYNSYM)
        {
            return i;
            break;
        }
    }

    return -1;
}

int find_symbol_table(Elf32_Ehdr* hdr, Elf32_Shdr* shdr)
{
    for (int i = 0; i < hdr->e_shnum; i++)
    {
        if (shdr[i].sh_type == SHT_SYMTAB)
        {
            return i;
            break;
        }
    }

    return -1;
}

void* find_sym(const char* name, Elf32_Shdr* shdr, Elf32_Shdr* shdr_sym, const char* src, char* dst)
{
    Elf32_Sym* syms = (Elf32_Sym*)(src + shdr_sym->sh_offset);
    const char* strings = src + shdr[shdr_sym->sh_link].sh_offset;
    
    for (int i = 0; i < shdr_sym->sh_size / sizeof(Elf32_Sym); i += 1)
    {
        if (strcmp(name, strings + syms[i].st_name) == 0)
        {
            return dst + syms[i].st_value;
        }
    }

    return NULL;
}

void* image_load(char *elf_start, unsigned int size)
{
    Elf32_Ehdr      *hdr     = NULL;
    Elf32_Phdr      *phdr    = NULL;
    Elf32_Shdr      *shdr    = NULL;
    char            *start   = NULL;
    char            *taddr   = NULL;
    void            *entry   = NULL;
    int i = 0;
    char *exec = NULL;

    hdr = (Elf32_Ehdr *) elf_start;
    
    if (!is_image_valid(hdr))
    {
        printf("Invalid ELF image\n");
        return 0;
    }

    exec = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if (!exec)
    {
        printf("Error allocating memory\n");
        return 0;
    }

    // Start with clean memory.
    memset(exec, 0x0, size);

    // Entries in the program header table
    phdr = (Elf32_Phdr *)(elf_start + hdr->e_phoff);

    // Go over all the entries in the ELF
    for (i = 0; i < hdr->e_phnum; ++i)
    {
        if (phdr[i].p_type != PT_LOAD)
        {
            continue;
        }

        if (phdr[i].p_filesz > phdr[i].p_memsz)
        {
            printf("image_load:: p_filesz > p_memsz\n");
            munmap(exec, size);
            return 0;
        }

        if (!phdr[i].p_filesz)
        {
            continue;
        }

        // p_filesz can be smaller than p_memsz,
        // the difference is zeroe'd out.
        start = elf_start + phdr[i].p_offset;
        taddr = phdr[i].p_vaddr + exec;
        memmove(taddr, start, phdr[i].p_filesz);

        if (!(phdr[i].p_flags & PF_W))
        {
            // Read-only.
            mprotect((unsigned char *) taddr, phdr[i].p_memsz, PROT_READ);
        }

        if (phdr[i].p_flags & PF_X)
        {
            // Executable.
            mprotect((unsigned char *) taddr, phdr[i].p_memsz, PROT_EXEC);
        }
    }

    // Section table
    shdr = (Elf32_Shdr *)(elf_start + hdr->e_shoff);

    // Find the global symbol table
    int global_symbol_table_index = find_global_symbol_table(hdr, shdr);
    // Symbols and names of the dynamic symbols (for relocation)
    Elf32_Sym* global_syms = (Elf32_Sym*)(elf_start + shdr[global_symbol_table_index].sh_offset);
    char* global_strings = elf_start + shdr[shdr[global_symbol_table_index].sh_link].sh_offset;
    
    // find section header string table
    char* section_strings = elf_start + shdr[hdr->e_shstrndx].sh_offset;

    // Find the .dynamic section
    int dynamic_index = find_dynamic_symbol_table(hdr, shdr);
    Elf32_Dyn* libs = (Elf32_Dyn*)(elf_start + shdr[dynamic_index].sh_offset);

    // Find the .dynster section
    int dynstr_index = find_dynstr_section(hdr, shdr, section_strings);
    char* dyn_strings = elf_start + shdr[dynstr_index].sh_offset;

    // Relocate global dynamic symbols
    for (i = 0; i < hdr->e_shnum; ++i)
    {
        if (shdr[i].sh_type == SHT_REL)
        {
            //relocate(shdr + i, global_syms, global_strings, elf_start, exec);
            relocate(shdr + i, global_syms, libs, dyn_strings, global_strings, elf_start, exec);
        }
    }

    // Find the main function in the symbol table
    int symbol_table_index = find_symbol_table(hdr, shdr);
    entry = find_sym("main", shdr, shdr + symbol_table_index, elf_start, exec);

   return entry;
}

int main(int argc, char** argv)
{
    char buf[1048576]; // Allocate 1MB for the program
    memset(buf, 0x0, sizeof(buf));

    FILE* elf = fopen(argv[1], "rb");

    if (elf != NULL)
    {
        int (*ptr)(int, char **);

        fread(buf, sizeof(buf), 1, elf);
        ptr = image_load(buf, sizeof(buf));

        if (ptr != NULL)
        {
            printf("Run the loaded program:\n");

            // Run the main function of the loaded program
            ptr(argc, argv);
        }
        else
        {
            printf("Loading unsuccessful...\n");
        }

        fclose(elf);

        return 0;
    }
    
    return 1;
}