
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#include "defs.h"

#define X_DIM 20
#define Y_DIM 20

int main() {

	typedef union
	{
		struct _pulse pulse;
	    Info info;
	} myMessage_t;

	myMessage_t message;

	char map[X_DIM][Y_DIM];

	//initial our 2d map to 0
	for(int i=0; i < X_DIM; i++) {
		for(int j=0; j < Y_DIM; j++) {
			map[i][j] = 0;
		}
	}

	//set our mines
	map[15][10] = 1;
	map[10][7] = 1;
	map[0][1] = 1;
	map [2][6] = 1;

	int rcvid;
	sensor_response response;

	name_attach_t *attach;

	if((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {
		return EXIT_FAILURE;
	}

	Info info;

	while(1) {
		//wait for coordinate request or whatever
		rcvid = MsgReceive(attach->chid, &message, sizeof(myMessage_t), NULL);

		if(rcvid == 0) {
			int direction = message.pulse.value.sival_int;
			switch (direction) {
			case NORTH:
				map[info.x][info.y - 1] = 0;
				break;
			case SOUTH:
				map[info.x][info.y + 1] = 0;
				break;
			case EAST:
				map[info.x + 1][info.y] = 0;
				break;
			case WEST:
				map[info.x - 1][info.y] = 0;
				break;
			}
			continue;
		}
		info = message.info;

		unsigned char x = info.x;
		unsigned char y = info.y;

		while(y < Y_DIM - 1 && x < X_DIM - 1 && y > 0 && x > 0) {
			switch (info.direction) {
			case NORTH:
				y -= 1;
				break;
			case SOUTH:
				y += 1;
				break;
			case EAST:
				x += 1;
				break;
			case WEST:
				x -= 1;
				break;
			}
			if (map[x][y]) break;
		}
		response.value = map[x][y];
		response.distance = 255;
		if (response.value) {
			int x_diff = info.x - x; //[info.x = 50, x = 50] => 0
			int y_diff = info.y - y; //[info.y = 45, y = 50] => 45 - 50 = -5
			response.distance = abs((int) (x_diff + y_diff)); //5

		}

		MsgReply(rcvid, EOK, &response, sizeof(response));

	}


}
