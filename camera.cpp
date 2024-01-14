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
#include <cstdlib>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <fstream>

bool parcare1[6][24];
bool client_slots[6][24] = {false};

int port;

int main(int argc, char *argv[])
{
    std::ifstream fin("parking_state.txt");
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 24; ++j)
        {
            char c;
            fin >> c;
            parcare1[i][j] = (c == '1');
        }
    }
    fin.close();
    srand(time(NULL));
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

    int flags = fcntl(socket_desc, F_GETFL, 0);
    fcntl(socket_desc, F_SETFL, flags | O_NONBLOCK);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);


    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        if (errno == EINPROGRESS)
        {
            fd_set write_fds;
            struct timeval tv;

            FD_ZERO(&write_fds);
            FD_SET(socket_desc, &write_fds);

            tv.tv_sec = 5;
            tv.tv_usec = 0;

            if (select(socket_desc + 1, NULL, &write_fds, NULL, &tv) > 0)
            {
                //socket ready
            }
            else
            {
                perror("[camera]Eroare la connect().\n");
                return errno;
            }
        }
        else
        {
            perror("[camera]Eroare la connect().\n");
            return errno;
        }
    }

    char message[256];
    strcpy(message, "camera");
    if (write(socket_desc, message, strlen(message)) < 0)
    {
        perror("[camera]Eroare la send().\n");
        return errno;
    }

    while (1)
    {
        char buffer[256];
        bzero(buffer, 256);
        int bytes_read = read(socket_desc, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            char *line = strtok(buffer, "\n");
            while (line != NULL)
            {
                std::cout << line << std::endl;
                int i, j;
                if (strstr(line, "vreau") != NULL)
                {
                    sscanf(line, "vreau %d %d", &i, &j);
                    parcare1[i][j] = 1;
                    client_slots[i][j] = 1;
                }
                else if (strstr(line, "eliberez") != NULL)
                {
                    sscanf(line, "eliberez %d %d", &i, &j);
                    parcare1[i][j] = 0;
                    client_slots[i][j] = 0;
                }
                line = strtok(NULL, "\n");
            }
        }
        else if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            perror("read");
        }
        int nr_switch = rand() % 5 + 1;
        for (int i = 0; i < nr_switch; i++)
        {
            int x = rand() % 6;
            int y = rand() % 24;
            if (!client_slots[x][y])
            {
                parcare1[x][y] = !parcare1[x][y];
            }
        }

        int length = 0;
        message[0] = '\0';
        std::ofstream fout("parking_state.txt", std::ios::out | std::ios::trunc);
        for (int i = 0; i < 6; ++i)
        {
            for (int j = 0; j < 24; ++j)
            {
                message[length++] = parcare1[i][j] ? '1' : '0';
                fout << (parcare1[i][j] ? '1' : '0');
            }
        }
        fout.close();
        message[length] = '\0';

        if (write(socket_desc, message, length) < 0)
        {
            perror("[camera]Eroare la send().\n");
            return errno;
        }
        //printf("%s\n", message);
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    close(socket_desc);
    return 0;
}