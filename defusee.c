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
		int x = getchar();
		printf("%d\n", x);
		sleep(1);
		MsgSend(coid, info, sizeof(*info), info, sizeof(*info));
	}
}

void *movement(void *arg) {
	Info *info = (Info *) arg;
	//wait for a command to move
	while(1) {
		// 1. getchar() to get '/033'
		// if it is: we discard the next character, and we only care about the A,B,C,D,E.
		//if it's not, we ignore it
		info->y += 1;
		sleep(1);
	}
}
