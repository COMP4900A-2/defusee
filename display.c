
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <ncurses.h>


#include "defs.h"


int main() {

int socket_desc, client_socket, client_size;
struct sockaddr_in server_addr, client_addr;
char client_message[512];

*client_message = 0;

socket_desc = socket(AF_INET, SOCK_STREAM, 0);

server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
server_addr.sin_addr.s_addr = inet_addr(SERVER);

if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
	perror("bind():");
}

if(listen(socket_desc, 1) < 0){
	perror("listen():");
}

client_size = sizeof(client_addr);
client_socket = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

//initscr();
//clear();

while(1) {
	int length = recv(client_socket, client_message, sizeof(client_message), 0);
	printf("%s\n", client_message);
	*client_message = 0;
}





}
