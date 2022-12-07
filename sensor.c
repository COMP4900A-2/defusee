#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <process.h>
#include <string.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "defs.h"

int main(int argc, char **argv) {

	int fd, rcvid;
	Info info;
	sensor_response *resp;
	name_attach_t *attach;

	attach = name_attach(NULL, SENSOR_ATTACH, 0);

	if(attach == NULL) {
		perror("attach():");
	}
	if((fd = shm_open("/sensor_memory", O_CREAT|O_RDWR, 0600)) == -1) {
		perror("shm_open()");
		return EXIT_FAILURE;
	}
	if(ftruncate(fd, __PAGESIZE) == -1) {
		perror("ftruncate()");
		return EXIT_FAILURE;
	}
	if((resp = (sensor_response *)mmap(NULL, __PAGESIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
		perror("mmap()");
		return EXIT_FAILURE;
	}
	close(fd);
	printf("memory setting done\n");
	int env_coid = name_open(ATTACH_POINT, 0);
	printf("name open done\n");
	while(1) {
		//wait for a trigger from defusee
		rcvid = MsgReceive(attach->chid,&info, sizeof(Info), NULL);
		//send message to environment to update sensor_response
		MsgSend(env_coid, &info, sizeof(Info), resp, sizeof(sensor_response));
		//reply back to defusee to confirm that it is done
		MsgReply(rcvid, EOK, NULL, 0);
	}
	munmap(resp, __PAGESIZE);
	shm_unlink("/sensor_memory");
	name_detach(attach, 0);


}

