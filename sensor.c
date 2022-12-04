/*
 * Sensor.c
 *
 *  Created on: Nov 19, 2022
 *      Author: williamwright
 */

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

	//defusee will call spawnvp to spawn the sensor and send its pid.
	//the sensor will create the shareable memory and send the handle to defusee
	//defusee will then access the memory sent by sensore.
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
		rcvid = MsgReceive(attach->chid,&info, sizeof(Info), NULL);
		MsgReply(rcvid, EOK, NULL, 0);
		MsgSend(env_coid, &info, sizeof(Info), resp, sizeof(sensor_response));
	}
	munmap(resp, __PAGESIZE);
	//shm_unlink("/sensor");


}

