#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <queue>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]) {
	int queue_size = 0, port = 0;

	switch (argc) {
	case 1:
		// default setup
		queue_size = 20;
		port = 5514;
		break;
	case 2:
		queue_size = atoi(argv[1]);
		port = 5514;
		break;
	case 3:
		queue_size = atoi(argv[1]);
		port = atoi(argv[2]);
		break;
	default:
		cout << "Invalid number of arguments, please enter between 0 and 2 arguments!" << endl;
		return 0;
	}

    queue<string> wall;   //stores wall message board

	int my_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (my_socket == -1) {
		cout << "Failed to create socket" << endl;
		return -1;
	}
	
	sockaddr_in connection;
	connection.sin_family = AF_INET;
	connection.sin_port = htons(port);
	inet_pton(AF_INET, "0.0.0.0", &connection.sin_addr);

	if (bind(my_socket, (sockaddr*)&connection, sizeof(connection)) == -1) {
		cout << "Failed to bind to IP/port" << endl;
		return -1;
	}
	
	// create socket for listening
	if (listen(my_socket, SOMAXCONN) == -1) {
		cout << "Failed to listen!" << endl;
		return -1;
	}

	// Accept input from connection
	sockaddr_in my_client;
	socklen_t client_size = sizeof(my_client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];

	while (true) {
		int client_socket = accept(my_socket, (sockaddr*)&my_client, &client_size);
		
        memset(host, 0, NI_MAXHOST);
		memset(svc, 0, NI_MAXSERV);

		int result = getnameinfo((sockaddr*)&my_client, sizeof(my_client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
	
		if (result) {
			cout << host << " connected on " << svc << endl;
		} else {
			inet_ntop(AF_INET, &my_client.sin_addr, host, NI_MAXHOST);
			cout << host << " connected on " << ntohs(my_client.sin_port) << endl;
		}
	
		char buf[512];
		while (true) {
            send(client_socket, "Wall Contents\n", 15, 0);
            send(client_socket, "-------------\n", 15, 0);
            if (wall.size() == 0)
                send(client_socket, "[NO MESSAGES - WALL EMPTY]\n\n", 28, 0);
            else {
                if ((int) wall.size() > queue_size)
                    wall.pop();
                
                queue<string> wall_copy = wall;
                while (!wall_copy.empty()) {
                    send(client_socket, wall_copy.front().c_str(), wall_copy.front().length(), 0);
                    wall_copy.pop();

                }
                send(client_socket, "\n", 2, 0);
            }

            send(client_socket, "Enter Command: ", 15, 0);

			// wipe buffer for use
			// wait for msg then display then echo it back
			memset(buf, 0, sizeof(buf));
			int my_bytes = recv(client_socket, buf, sizeof(buf), 0);
            if (my_bytes == 0) {
				cout << "Client disconnected" << endl;
				break;
			}

            if (string(buf, 0, my_bytes - 1).compare("post") == 0){ //post
                send(client_socket, "Enter name: ", 12, 0);
                memset(buf, 0, sizeof(buf));

			    my_bytes = recv(client_socket, buf, sizeof(buf), 0);
                if (my_bytes - 1 > 80)
                    send(client_socket, "Error: message is too long!\n\n", 29, 0);
                else {
                    int length = 80 - (int) my_bytes - 1;

                    string  name,
                            post_length_string = "Post [Max length " + to_string(length) + "]: ";

                    send(client_socket, post_length_string.c_str(), post_length_string.length(), 0);

                    name = string(buf, 0, my_bytes - 1);

			        my_bytes = recv(client_socket, buf, sizeof(buf), 0);
                    if (length - (int) my_bytes - 1 < 0)
                        send(client_socket, "Error: message is too long!\n\n", 29, 0);
                    else {
                        send(client_socket, "Successfully tagged the wall.\n\n", 31, 0);
                        wall.push(name + ": " + string(buf, 0, my_bytes));
                        cout << "queue" << name + ": " + string(buf, 0, my_bytes) << endl;
                    }
                }
            }
            else if (string(buf, 0, my_bytes - 1).compare("clear") == 0){   //clear
                while (!wall.empty()) {
                    wall.pop();
                }
                send(client_socket, "Wall cleared.\n\n", 16, 0);
            }
            else if (string(buf, 0, my_bytes - 1).compare("quit") == 0){    //quit
                send(client_socket, "Come back soon. Bye!.\n\n", 24, 0);
                close(client_socket);
				break;
            }
            else if (string(buf, 0, my_bytes - 1).compare("kill") == 0){    //kill
                send(client_socket, "Closing socket and terminating server. Bye!\n\n", 45, 0);
                close(client_socket);
                close(my_socket);
                exit(0);
            }
			
            cout << "Received: " << string(buf, 0, my_bytes) << endl;
		}

		// exit socket
		close(client_socket);
	}

	return 0;
}

