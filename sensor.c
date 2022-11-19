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

	if(argc < 2) {
		return EXIT_FAILURE;
	}
	printf(argv[1]);
	//defusee will call spawnvp to spawn the sensor and send its pid.
	//the sensor will create the shareable memory and send the handle to defusee
	//defusee will then access the memory sent by sensore.
	int fd;
	struct _pulse pulse;
	sensor_response *resp;
	name_attach_t *attach;
	attach = name_attach(NULL, SENSOR_ATTACH, 0);
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
	int env_coid = name_open(ATTACH_POINT, 0);
	while(1) {
		MsgReceive(attach->chid, &pulse, sizeof(pulse), NULL);
		MsgSend(env_coid, &pulse.value.sival_int, sizeof(int), resp, sizeof(sensor_response));
		printf("value: %d\n", resp->value);
		printf("x: %d\n", resp->x);
		printf("y: %d\n", resp->y);
	}
	munmap(resp, __PAGESIZE);
	//shm_unlink("/sensor");


}

