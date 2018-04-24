#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

const size_t fileSize = 100000;

int timeBetween(const struct timeval since)
{
  struct timeval now;

  gettimeofday(&now, NULL);

  // In microseconds
  const int diff = now.tv_usec - since.tv_usec;

  // In milliseconds
  return diff / 1000;
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

      if (write(fileWrite, buf, bytes) < bytesRead) {
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
  FILE* fileRead = fopen("./testfile", "r");
  FILE* fileWrite = fopen("./testfile_copy", "w");
  int bytesRead = 0;
  char buf[chunkSize];

  while (1) {
    int bytes = 0;

    if (fgets(buf, chunkSize, fileRead)) {
      bytesRead += bytes;

      if (fputs(buf, fileWrite))
    } else {
      break;
    }
  }

  fclose(fileRead);
  fclose(fileWrite);

  return 0;
}

int main()
{
  // Create file
  {
    srand(time(NULL));

    int file = open("./testfile", O_CREAT | O_WRONLY, 0770);
    char buf[fileSize];

    for (int i = 0; i < fileSize; ++i) {
      buf[i] = (char)(rand() % 255);
    }

    write(file, buf, fileSize);
    close(file);
  }

  // System calls - one character at a time
  {
    struct timeval timepoint;
    gettimeofday(&timepoint, NULL);

    int bytesCopied = copyFile(1);

    printf("Copying one byte at a time: %dms, bytes read & written: %d", timeBetween(timepoint), bytesCopied);
  }

  // 64 characters at a time
  {
    struct timeval timepoint;
    gettimeofday(&timepoint, NULL);

    int bytesCopied = copyFile(64);

    printf("Copying one byte at a time: %dms, bytes read & written: %d", timeBetween(timepoint), bytesCopied);
  }

  // C library calls
  {

  }

  return EXIT_SUCCESS;
}