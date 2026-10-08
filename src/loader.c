#include <proc.h>
#include <elf.h>
#include <fs.h>

#if defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
# define EXPECT_TYPE EM_RISCV
#endif

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);

  Elf_Ehdr ehdr;
  fs_read(fd, &ehdr, sizeof(ehdr));
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);   // ELF 魔数

  for (int i = 0; i < ehdr.e_phnum; i++) {
    Elf_Phdr ph;
    fs_lseek(fd, ehdr.e_phoff + i * ehdr.e_phentsize, SEEK_SET);
    fs_read(fd, &ph, sizeof(ph));
    if (ph.p_type != PT_LOAD) continue;

    fs_lseek(fd, ph.p_offset, SEEK_SET);
    fs_read(fd, (void *)ph.p_vaddr, ph.p_filesz);
    memset((void *)(ph.p_vaddr + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz);
  }

  fs_close(fd);
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

