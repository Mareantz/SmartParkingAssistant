#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <thread>
#include <atomic>
#include "window.h"
#include "mutex.h"

int port;
Window window;
std::atomic<bool> isRunning(true);
std::mutex guard;

bool parcare[6][24];
bool** parcarePtr=new bool*[6];
int parkingSlot_i,parkingSlot_j;
int* parkingSlotPtr_i=&parkingSlot_i;
int* parkingSlotPtr_j=&parkingSlot_j;

int main(int argc, char *argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    if (argc != 3)
    {
        printf("Sintaxa: %s <adresa_server> <port> \n", argv[0]);
        return -1;
    }
    port = atoi(argv[2]);

    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Eroare la connect().\n");
        return errno;
    }

    for(int i=0;i<6;i++)
    {
        parcarePtr[i]=parcare[i];
    }

    char message[256];
    strcpy(message, "client");
    if (send(socket_desc, message, strlen(message), 0) < 0)
    {
        perror("[client]Eroare la send().\n");
        return errno;
    }
    int length = 0;
    message[0] = '\0';

    std::thread receiveThread([&]() {
        while (isRunning) {
            bzero(message, 256);
            if (read(socket_desc, message, 256) < 0) {
                perror("[client]Eroare la read() de la server.\n");
                return errno;
            }
            message[strlen(message)] = '\0';
            char parkingStatus[256];
            guard.lock();
            sscanf(message, "%s %d %d", parkingStatus, &parkingSlot_i, &parkingSlot_j);
            
            int index = 0;
            for (int i = 0; i < 6; ++i) {
                for (int j = 0; j < 24; ++j) {
                    if (index >= strlen(parkingStatus)) {
                        break;
                    }
                    parcare[i][j] = (parkingStatus[index++] == '1');
                }
            }
            guard.unlock();
        }
        return 0;
    });

    window.run(parcarePtr,socket_desc,parkingSlotPtr_i,parkingSlotPtr_j,&isRunning);
    receiveThread.join();

    close(socket_desc);
    return 0;
}