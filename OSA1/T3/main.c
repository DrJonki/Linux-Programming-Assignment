#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

// 1 MB
const size_t fileSize = 1 * 1024 * 1024;

long timeBetween(const struct timeval since)
{
  struct timeval now;

  gettimeofday(&now, NULL);

  // In microseconds
  const suseconds_t diff = now.tv_usec - since.tv_usec;

  // In milliseconds
  return diff / 1000l;
}

int copyFile(const int chunkSize)
{
  int fileRead = open("./testfile", O_RDONLY);
  int fileWrite = open("./testfile_copy", O_CREAT | O_WRONLY, 0770);
  int bytesRead = 0;
  char buf[chunkSize];

  while (1) {
    ssize_t bytes = read(fileRead, buf, chunkSize);
    if (bytes > 0) {
      bytesRead += bytes;

      if (write(fileWrite, buf, bytes) < bytes) {
        break;
      }
    } else {
      break;
    }
  }

  close(fileRead);
  close(fileWrite);

  return bytesRead;
}

int copyFileC(const int chunkSize)
{
  FILE* fileRead = fopen("testfile", "r");
  FILE* fileWrite = fopen("testfile_copy", "w");
  int bytesRead = 0;
  char buf[chunkSize];

  while (1) {
    size_t bytes = fread(buf, 1, chunkSize, fileRead);
    if (bytes > 0) {
      bytesRead += bytes;

      if (fwrite(buf, 1, bytes, fileWrite) < bytes) {
        break;
      }
    } else {
      break;
    }
  }

  fclose(fileRead);
  fclose(fileWrite);

  return bytesRead;
}

int main()
{
  setbuf(stdout, NULL);

  // Create file
  {
    unlink("./testfile");
    unlink("./testfile_copy");

    printf("Writing random bytes: %luMB\n", fileSize / 1024u / 1024u);

    srand(time(NULL));

    int file = open("./testfile", O_CREAT | O_WRONLY, 0770);

    const int chunkSize = 1024;
    char buf[chunkSize];

    for (int bytesLeft = fileSize; bytesLeft > 0; bytesLeft -= chunkSize) {
      for (int i = 0; i < chunkSize; ++i) {
        buf[i] = (char)(rand() % 255);
      }

      write(file, buf, chunkSize);
    }

    close(file);
  }

  // System calls - one character at a time
  {
    struct timeval timepoint;
    gettimeofday(&timepoint, NULL);

    int bytesCopied = copyFile(1);

    printf("System: Copying one byte at a time: %ld ms, bytes copied: %d\n", timeBetween(timepoint), bytesCopied);
  }

  // 64 characters at a time
  {
    struct timeval timepoint;
    gettimeofday(&timepoint, NULL);

    int bytesCopied = copyFile(64);

    printf("System: Copying 64 bytes at a time: %ld ms, bytes copied: %d\n", timeBetween(timepoint), bytesCopied);
  }

  // C library calls - one character at a time
  {
    struct timeval timepoint;
    gettimeofday(&timepoint, NULL);

    int bytesCopied = copyFileC(1);

    printf("C: Copying one byte at a time: %ld ms, bytes copied: %d\n", timeBetween(timepoint), bytesCopied);
  }

  // 64 characters at a time
  {
    struct timeval timepoint;
    gettimeofday(&timepoint, NULL);

    int bytesCopied = copyFileC(64);

    printf("C: Copying 64 bytes at a time: %ld ms, bytes copied: %d\n", timeBetween(timepoint), bytesCopied);
  }

  printf("Done\n");

  return EXIT_SUCCESS;
}

/*

Sample output:

$ Writing random bytes: 1MB
$ System: Copying one byte at a time: 370 ms, bytes copied: 1048576
$ System: Copying 64 bytes at a time: 150 ms, bytes copied: 1048576
$ C: Copying one byte at a time: 49 ms, bytes copied: 1048576
$ C: Copying 64 bytes at a time: 7 ms, bytes copied: 1048576
$ Done

*/