/*
** read_elf_header.c for read_elf_header in /home/zeng_d
**
** Made by David Zeng
** Login   <zeng_d@epitech.net>
**
** Started on  Mon Feb 13 12:11:35 2017 David Zeng
** Last update Mon Feb 13 12:11:35 2017 David Zeng
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_objdump.h"

static int	check_ident(Elf64_Ehdr *elf, int fd, char const *filename)
{
  int		ret;

  ret = read(fd, elf, sizeof(Elf64_Ehdr));
  if (ret == 0)
    return (1);
  if (ret != sizeof(Elf64_Ehdr))
    return (file_truncated(filename));
  if (elf->e_ident[EI_MAG0] != ELFMAG0 || elf->e_ident[EI_MAG1] != ELFMAG1 ||
      elf->e_ident[EI_MAG2] != ELFMAG2 || elf->e_ident[EI_MAG3] != ELFMAG3)
    {
      fprintf(stderr, "%s: %s: File format not recognized\n",
	      g_prog_name, filename);
      return (1);
    }
  return (0);
}

static void	add_flags(t_elf *elf, Elf64_Shdr *shdr, bool e_type)
{
  if (e_type)
    {
      if (elf->ehdr.e_type == ET_REL)
	elf->ehdr.e_flags |= HAS_RELOC;
      else if (elf->ehdr.e_type == ET_EXEC)
	elf->ehdr.e_flags |= EXEC_P;
      else if (elf->ehdr.e_type == ET_DYN)
	elf->ehdr.e_flags |= DYNAMIC;
    }
  else
    {
      if (shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM)
	elf->ehdr.e_flags |= HAS_SYMS;
      else if (shdr->sh_type == SHT_DYNAMIC)
	elf->ehdr.e_flags |= D_PAGED;
    }
}

static int	check_shdr(t_elf *elf, int fd)
{
  int		ret;
  int		i;

  lseek(fd, elf->ehdr.e_shoff - sizeof(Elf64_Ehdr), SEEK_CUR);
  if ((elf->shdr = malloc(sizeof(Elf64_Shdr) * elf->ehdr.e_shnum)) == NULL)
    {
      fprintf(stderr, "Malloc out of memory\n");
      return (1);
    }
  i = -1;
  while (++i < elf->ehdr.e_shnum)
    {
      ret = read(fd, &elf->shdr[i], sizeof(Elf64_Shdr));
      if (ret == 0)
	return (1);
      if (ret != sizeof(Elf64_Shdr))
	return (file_truncated(elf->filename));
      // TODO VERIFY ??
      add_flags(elf, &elf->shdr[i], false);
    }
  return (0);
}

int	parse_elf(t_elf *elf, int fd)
{
  if (check_ident(&elf->ehdr, fd, elf->filename) != 0 ||
      check_shdr(elf, fd) != 0)
    return (1);
  add_flags(elf, NULL, true);
  return (0);
}