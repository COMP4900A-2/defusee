
#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include "defs.h"

void printMap(char **map, int x, int y);

int main() {

	typedef union
	{
		struct _pulse pulse;
	    Info info;
	} myMessage_t;

	myMessage_t message;

	char map[50][50];

	for(int i=0; i < 50; i++) {
		for(int j=0; j < 50; j++) {
			map[i][j] = 0;
		}
	}

	map[25][25] = 1;
	map[15][23] = 1;
	map[12][11] = 1;
	map [13][0] = 1;
	//detect that mine and destroy it.
	//create channel stuffname_attach_t *attach;
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
				break;
			case WEST:
				break;
			}
			continue;
		}
		info = message.info;
		map[info.x][info.y] = 3;

		unsigned char x = info.x;
		unsigned char y = info.y;

		while(y < 100 && x < 100 && y >= 0 && x >= 0) {
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
		response.distance = -1;
		if (response.value) {
			int x_diff = info.x - x; //[info.x = 50, x = 50] => 0
			int y_diff = info.y - y; //[info.y = 45, y = 50] => 45 - 50 = -5
			response.distance = abs((int) (x_diff + y_diff)); //5

		}
		// possibility: use iov? (assignment 6 as an example).
		MsgReply(rcvid, EOK, &response, sizeof(response));
		for(int i=0; i<50; i++) {
			for(int j=0; j<50; j++) {
				printf("%d ", map[i][j]);
			}
			printf("\n");
		}

	}


}
