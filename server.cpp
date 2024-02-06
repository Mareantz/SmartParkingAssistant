
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include <mutex>
#include <condition_variable>

#define PORT 2308

bool parcare[6][24];
std::mutex guard;
int camera;
bool camera_active = false;

std::condition_variable flag;

struct ClientData
{
	int clientSocket;
	std::atomic<bool> clientConnected;
	struct
	{
		short int i=-1;
		short int j=-1;
	}parkingSlot;
};

void timer_thread()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(10));
		flag.notify_all();
	}
}

std::string loc_parcare(int i, int j)
{
	std::string loc;
	if (i == 0)
		loc = "A";
	else if (i == 1)
		loc = "B";
	else if (i == 2)
		loc = "C";
	else if (i == 3)
		loc = "D";
	else if (i == 4)
		loc = "E";
	else if (i == 5)
		loc = "F";
	loc += std::to_string(j + 1);
	return loc;
}



void *handle_commands(ClientData *arg)
{
	ClientData *data = (ClientData *)arg;
	int client = data->clientSocket;
	char message[256];
	char response[256] = "";
	while (data->clientConnected)
	{
		bzero(message, 256);
		std::cout<<"[server]Asteptam mesajul...\n";
		fflush(stdout);

		if (read(client, message, 256) <= 0)
		{
			perror("[server]Eroare la read() de la client.\n");
			data->clientConnected = false;
			return NULL;
		}

		if (strstr(message, "vreau") != NULL)
		{
			{
				std::cout<<"Am primit cerere de parcare\n";
				int i, j;
				guard.lock();
				do
				{
					i = rand() % 6;
					j = rand() % 24;
				} while (parcare[i][j] == 1);
				parcare[i][j] = 1;
				guard.unlock();
				flag.notify_all();
				sprintf(response, "vreau %d %d\n", i, j);
				if (write(camera, response, strlen(response)) <= 0)
				{
					std::cout<<"test\n";
					perror("[server]Eroare la write() catre camera.\n");
					continue;
				}
				// strcpy(response, "Ai primit locul de parcare: ");
				// strcat(response, loc_parcare(i, j).c_str());
				data->parkingSlot.i = i;
				data->parkingSlot.j = j;
			}
		}

		else if (strstr(message, "eliberez") != NULL)
		{
			std::cout<<"Eliberez locul de parcare\n";
			guard.lock();
			parcare[data->parkingSlot.i][data->parkingSlot.j] = 0;
			sprintf(response, "eliberez %d %d\n", data->parkingSlot.i, data->parkingSlot.j);
			if (write(camera, response, strlen(response)) <= 0)
			{
				perror("[server]Eroare la write() catre camera.\n");
				continue;
			}
			data->parkingSlot.i = -1;
			data->parkingSlot.j = -1;
			guard.unlock();
			flag.notify_all();
			//strcpy(response, "Ai eliberat locul de parcare cu succes!");
		}
		std::cout<<"[server]Mesajul a fost trasmis cu succes.\n";
	}
	close(client);
	return NULL;
}

void *get_parking_status(ClientData *arg)
{
	int client = *(int *)arg;
	char message[256];
	while (1)
	{
		bzero(message, 256);

		if (read(client, message, 256) <= 0)
		{
			perror("[server]Eroare la read() de la client.\n");
			camera_active = false;
			camera = -1;
			close(client);
			return NULL;
		}

		message[strlen(message)] = '\0';
		int index = 0;

		guard.lock();
		for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < 24; ++j)
			{
				if (index >= strlen(message))
				{
					break;
				}
				parcare[i][j] = (message[index++] == '1');
			}
		}
		guard.unlock();
		flag.notify_all();
		std::cout<<"inca primesc\n";
		std::unique_lock<std::mutex> lock(guard);
		flag.wait(lock);
		lock.unlock();
	}
	// close(client);
	return NULL;
}

void *send_parking_status(ClientData *arg)
{
	ClientData *data = (ClientData *)arg;
	int client = data->clientSocket;
	int counter = 0;
	char message[192];
	while (data->clientConnected)
	{
		bzero(message, 192);
		int length = 0;
		message[0] = '\0';
		guard.lock();
		for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < 24; ++j)
			{
				message[length++] = parcare[i][j] ? '1' : '0';
			}
		}
		guard.unlock();
		message[length] = '\0';

		char response[256];

		sprintf(response, "%s %d %d", message, data->parkingSlot.i, data->parkingSlot.j);
		if (write(client, response, strlen(response)) < 0)
		{
			perror("[client]Eroare la write().\n");
			data->clientConnected = false;
			close(client);
			std::cout << "Client disconnected" << std::endl;
		}
		//std::cout<<response<<std::endl;
		flag.notify_all();
		std::unique_lock<std::mutex> lock(guard);
		flag.wait(lock);
		lock.unlock();
	}
	close(client);
	delete data;
	return NULL;
}

int main()
{
	srand(time(NULL));
	struct sockaddr_in server;
	struct sockaddr_in from;

	int socket_desc;

	if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("[server]Eroare la socket().\n");
		return errno;
	}

	bzero(&server, sizeof(server));
	bzero(&from, sizeof(from));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{
		perror("[server]Eroare la bind().\n");
		return errno;
	}
	if (listen(socket_desc, 1) == -1)
	{
		perror("[server]Eroare la listen().\n");
		return errno;
	}

	std::thread timer(timer_thread);
	timer.detach();

	while (1)
	{
		ClientData *client = new ClientData;
		socklen_t length = sizeof(from);
		std::cout<<"[server]Asteptam la portul "<<PORT<<"...\n";
		fflush(stdout);

		std::thread thread, camera_thread, info_thread;

		client->clientSocket = accept(socket_desc, (struct sockaddr *)&from, &length);
		client->clientConnected = true;

		if (client->clientSocket < 0)
		{
			perror("[server]Eroare la accept().\n");
			continue;
		}

		char buffer[256];
		bzero(buffer, 256);
		if (read(client->clientSocket, buffer, 256) <= 0)
		{
			perror("[server]Eroare la read() de la client.\n");
			continue;
		}

		if (strstr(buffer, "camera") != NULL)
		{
			if (!camera_active)
			{
				camera_active = true;
				camera = client->clientSocket;
				camera_thread = std::thread(get_parking_status, client);
				camera_thread.detach();
			}
		}
		else
		{
			thread = std::thread(handle_commands, client);
			info_thread = std::thread(send_parking_status, client);
			thread.detach();
			info_thread.detach();
		}
	}
	return 0;
}