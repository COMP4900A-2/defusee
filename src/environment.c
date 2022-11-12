
#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#define ATTACH_POINT      "environment"

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
	int rcvid, rpid;

	if((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {
		return EXIT_FAILURE;
	}

	while(1) {
		//wait for coordinate request or whatever

		//reply with a message;

	}
}
