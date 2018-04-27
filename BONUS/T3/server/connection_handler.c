#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <pthread.h>
#include <semaphore.h>

#ifndef MAX_CONNECTIONS
  #define MAX_CONNECTIONS 10
#endif

#if MAX_CONNECTIONS <= 1
  #error Invalid value for MAX_CONNECTIONS (must be positive)
#endif

extern const char* const queueName;

typedef struct
{
  int sock;
  struct sockaddr_in attr;
  int running;

} socket_info;

socket_info sock_info[MAX_CONNECTIONS];
pthread_mutex_t sock_info_mutex;
sem_t connection_semaphore;

int printError_conn(const char* where)
{
  printf("ERROR (conn_thread): %s - %d\n", where, errno);
  return EXIT_FAILURE;
}

int find_free_slot()
{
  pthread_mutex_lock(&sock_info_mutex);

  int freeIndex = -1;

  for (int i = 0; i < MAX_CONNECTIONS; ++i) {
    socket_info* info = &sock_info[i];

    if (info->sock == -1) {
      freeIndex = i;
      break;
    }
  }

  pthread_mutex_unlock(&sock_info_mutex);
}

void* connection_handler(void* data);

int init_connection_pool()
{
  pthread_mutex_init(&sock_info_mutex, NULL);
  sem_init(&connection_semaphore, 0, 0);

  for (int i = 0; i < MAX_CONNECTIONS; ++i) {
    socket_info* info = &sock_info[i];

    info->sock = -1;
    info->running = 0;

    pthread_t thread;
    pthread_create(&thread, NULL, connection_handler, NULL);
    //pthread_detach(thread);
  }
}

in_port_t prepare_new_connection()
{
  socket_info* info;
  int index;
  while (1) {
    index = find_free_slot();

    if (index == -1) {
      sleep(1);
      continue;
    }

    info = &sock_info[index];
    break;
  }

  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock == -1) {
    printError_conn("socket creation");
    return 0;
  }

  struct sockaddr_in attr;
  attr.sin_addr.s_addr = INADDR_ANY;
  attr.sin_family = AF_INET;
  attr.sin_port = 0; // Select any available port

  if (bind(sock, (struct sockaddr*)&attr, (socklen_t)sizeof(attr)) == -1) {
    printError_conn("socket bind");
    return 0;
  }

  if (listen(sock, 10) == -1) {
    printError_conn("socket listen");
    return 0;
  }

  socklen_t attrlen = sizeof(attr);
  
  if (getsockname(sock, (struct sockaddr*)&attr, &attrlen) == -1) {
    printError_conn("getsockname");
    return 0;
  }

  pthread_mutex_lock(&sock_info_mutex);
  info->running = 0;
  info->sock = sock;
  info->attr = attr;
  pthread_mutex_unlock(&sock_info_mutex);

  sem_post(&connection_semaphore);

  return attr.sin_port;
}

void* connection_handler(void* data)
{
  mqd_t queue = mq_open(queueName, O_WRONLY);

  while (1) {
    sem_wait(&connection_semaphore);

    // Find first initialized, but non-running socket
    pthread_mutex_lock(&sock_info_mutex);

    int sock_server;
    socket_info* info;
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
      info = &sock_info[i];
      
      if (info->sock != -1 && !info->running) {
        sock_server = info->sock;
        info->running = 1;
        break;
      }
    }

    pthread_mutex_unlock(&sock_info_mutex);

    struct sockaddr_in client;
    socklen_t clientlen = sizeof(client);
    int sock_client = accept(sock_server, (struct sockaddr*)&client, &clientlen);

    if (sock_client == -1) {
      printError_conn("socket accept");
    }

    // Start listening to messages
    char buf[1024];

    while (1) {
      ssize_t received = recv(sock_client, buf, 1024, 0);

      if (received == -1) {
        printError_conn("recv");
        break;
      } else if (received == 0) {
        break; // Disconnection
      }

      if (mq_send(queue, buf, received, 1) == -1) {
        printError_conn("message queue send");
      }
    }

    pthread_mutex_lock(&sock_info_mutex);
    info->sock = -1;
    pthread_mutex_unlock(&sock_info_mutex);
  }

  return NULL;
}