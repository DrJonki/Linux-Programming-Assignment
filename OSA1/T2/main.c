#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
  int handle = open("./tiedosto", O_CREAT | O_RDWR, 0770);

  const char* text = "pieni maara tekstia";
  write(handle, text, strlen(text));

  close(handle);

  pid_t pid;
  switch (pid = fork())
  {
    case -1:
      exit(1);
    case 0:
    {
      execl("/bin/cp", "/bin/cp", "./tiedosto", "./tiedosto_backup", (char *)0);
      exit(EXIT_SUCCESS);
    }
  }

  wait(NULL);

  struct stat* buf = malloc(sizeof(struct stat));
  stat("./tiedosto", buf);

  printf("Filename: tiedosto\n");
  printf("User: %u\n", buf->st_uid);
  printf("Size: %ld\n", buf->st_size);
  printf("Last access: %ld\n", buf->st_atime);

  free(buf);

  return EXIT_SUCCESS;
}
