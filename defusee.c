#include <stdio.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>

#include "defs.h"

void *sensor(void *);
void *movement(void *);

int main(void) {


	Info info;
	info.x = 50;
	info.y = 45;
	info.direction = SOUTH;

	pid_t pid_sensor, pid_movement;
	pthread_create(&pid_sensor, NULL, sensor, (void *) &info);
	pthread_create(&pid_movement, NULL, movement, (void *) &info);
	pthread_join(pid_movement, NULL);

}

void *sensor(void * arg) {
	Info *info = (Info *) arg;
	int coid = name_open(ATTACH_POINT,0);
	while(1) {
		MsgSend(coid, info, sizeof(*info), info, sizeof(*info));
	}
}

void *movement(void *arg) {
	Info *info = (Info *) arg;
	//wait for a command to move
	while(1) {
		// conditions here and also mutex and condvar stuff
		info->y += 1;
		sleep(1);
	}
}
