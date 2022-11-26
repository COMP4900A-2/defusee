
#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include "defs.h"

int main() {

	char map[100][100];

	for(int i=0; i < 100; i++) {
		for(int j=0; j < 100; j++) {
			map[i][j] = 0;
		}
	}

	map[50][50] = 1;
	//detect that mine and destroy it.
	//create channel stuffname_attach_t *attach;
	int rcvid;
	Info info;
	sensor_response response;

	name_attach_t *attach;

	if((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {
		return EXIT_FAILURE;
	}

	while(1) {
		//wait for coordinate request or whatever
		rcvid = MsgReceive(attach->chid, &info, sizeof(Info), NULL);
		printf("direction: %c\n", info.direction);
		printf("robot y: %d\n", info.y);
		printf("robot x: %d\n", info.x);
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

	}
}
