#include <stdio.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>
#include <fcntl.h>
#include <termios.h>

#include "defs.h"

void *sensor(void *);
void *movement(void *);

int main(void) {

	struct termios attr;
	fcntl(0, F_SETFL, O_NONBLOCK);
	tcgetattr(0, &attr);
	attr.c_lflag &= ~ICANON;
	attr.c_lflag &= ECHO;
	tcsetattr(0, TCSANOW, &attr);
	//this step spawns the sensor "notice me sensor"
	spawnlp(P_NOWAIT, "/tmp/sensor", "sensor", NULL);
	sleep(1);

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
	int sensor_coid = name_open(SENSOR_ATTACH, 0);
	if(sensor_coid == -1) {
		perror("name_open():");
		exit(-1);
	}
	while(1) {
		MsgSend(sensor_coid,info, sizeof(Info), NULL, 0);
		printf("msgSent\n");
		usleep(1000000);
	}
}

void *movement(void *arg) {
	Info *info = (Info *) arg;
	//wait for a command to move
	while(1) {
		char x = getchar();
		if(x == 'w' || x == 'W') info->y--;
		else if(x == 's' || x == 'S') info->y++;
		else if(x == 'a' || x == 'A') info->x--;
		else if(x == 'd' || x == 'D') info->x++;
		while(getchar() != -1);
		sleep(1);
	}
}
