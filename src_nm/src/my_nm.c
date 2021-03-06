/*
** my_nm.c for my_nm in /home/zeng_d/test/trash/tarliba
**
** Made by David Zeng
** Login   <zeng_d@epitech.net>
**
** Started on  Wed Feb 15 21:30:27 2017 David Zeng
** Last update Wed Feb 15 21:30:27 2017 David Zeng
*/

#include "my_nm.h"

static int	open_file(char const *filename)
{
  int		fd;

  fd = open(filename, O_DIRECTORY);
  if (fd != -1)
    {
      close(fd);
      fprintf(stderr, "%s: Warning: '%s' is not an ordinary file\n",
	      g_prog_name, filename);
      return (-1);
    }
  fd = open(filename, O_RDONLY);
  if (fd == -1)
    fprintf(stderr, "%s: '%s': No such file\n", g_prog_name, filename);
  return (fd);
}

static int	loop_archive(int fd)
{
  char		*filename;
  int		ret;
  int		ret2;
  int		ret3;
  size_t	offset;
  t_cont	cont;

  ret2 = 0;
  skip_first(fd);
  filename = NULL;
  cont = (t_cont) {.next = 0, .tab = NULL, .idx = 0};
  while ((ret = get_next_ar_file(fd, &filename, &offset, &cont)) == 0)
    {
      if ((ret3 = display_file(filename, fd, offset, true)) == 1)
	ret2 = 1;
      free(filename);
      if (ret3 == -1)
	{
	  ret = 1;
	  break;
	}
    }
  close(fd);
  return (ret == 1 ? -1 : ret2);
}

int	my_nm(char const *filename, bool many)
{
  int	fd;
  int	ret;

  if ((fd = open_file(filename)) == -1)
    return (1);
  if (is_archive(fd))
    {
      if (many)
	printf("\n%s:\n", filename);
      if ((ret = loop_archive(fd)) == -1)
  	{
  	  fprintf(stderr, "%s: %s: Malformed archive\n", g_prog_name, filename);
  	  return (1);
  	}
      return (ret);
    }
  ret = display_file(filename, fd, 0, many);
  close(fd);
  return (ret);
}
