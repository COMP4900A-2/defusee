
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

//map size
#define X_DIM 10
#define Y_DIM 10

int main() {

	//we can either receive a message to scan the map, or a pulse to demine the mine
	typedef union
	{
		struct _pulse pulse;
	    Info info;
	} myMessage_t;

	myMessage_t message;
	Info info;
	char map[Y_DIM][X_DIM] ={
			{2,2,2,2,2,2,2,2,2,2},
			{2,0,0,0,0,0,0,0,0,2},
			{2,0,0,0,0,0,0,0,0,2},
			{2,0,0,2,2,2,2,0,0,2},
			{2,1,0,2,0,1,2,0,0,2},
			{2,0,0,2,0,2,2,0,0,2},
			{2,0,0,0,0,0,0,0,0,2},
			{2,0,0,0,0,0,0,0,0,2},
			{2,0,0,0,0,1,0,0,0,2},
			{2,2,2,2,2,2,2,2,2,2},
			};

	int rcvid;
	sensor_response response;

	name_attach_t *attach;

	if((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {
		return EXIT_FAILURE;
	}

	while(1) {
		//wait for coordinate request or whatever
		rcvid = MsgReceive(attach->chid, &message, sizeof(myMessage_t), NULL);

		if(rcvid == 0) {
			int direction = message.pulse.value.sival_int;
			switch (direction) {
			case NORTH:
				map[info.y - 1][info.x] = 0;
				break;
			case SOUTH:
				map[info.y + 1][info.x] = 0;
				break;
			case EAST:
				map[info.y][info.x + 1] = 0;
				break;
			case WEST:
				map[info.y][info.x - 1] = 0;
				break;
			}
			continue;
		}
		info = message.info;

		unsigned char x = info.x;
		unsigned char y = info.y;

		//calculate the distance between the robot and the mine depending on its facing direction
		// 255 is recorded if no mine is found
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
			if (map[y][x]) break;
		}
		response.value = map[y][x];
		response.distance = 255;
		if (response.value) {
			int x_diff = info.x - x; //[info.x = 50, x = 50] => 0
			int y_diff = info.y - y; //[info.y = 45, y = 50] => 45 - 50 = -5
			response.distance = abs((int) (x_diff + y_diff)); //5

		}

		//reply back to the sensor
		MsgReply(rcvid, EOK, &response, sizeof(response));

	}
	name_detach(attach, 0);


}
