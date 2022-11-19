
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

	map[50][99] = 1;
	//detect that mine and destroy it.
	//create channel stuffname_attach_t *attach;
	int rcvid, rpid;
	sensor_response info;

	name_attach_t *attach;

	if((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {
		return EXIT_FAILURE;
	}

	while(1) {
		//wait for coordinate request or whatever
		rcvid = MsgReceive(attach->chid, &info, sizeof(info), NULL);
		char x = info.x;
		char y = info.y;

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
		info.value = map[x][y];
		info.x = x;
		info.y = y;
		// possibility: use iov? (assignment 6 as an example).
		MsgReply(rcvid, EOK, &info, sizeof(info));
		//algorith to find the mine/object
		//reply with a message;

	}
}
