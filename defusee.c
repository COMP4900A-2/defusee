#include <stdio.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>

#include "defs.h"

void *sensor(void *);
void *movement(void *);
void *demine(void *);
void *rotate(void *);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

volatile char state;


int main(void) {

	state = WAIT;
	//find the gamestackoverflow link
	struct termios attr;
	fcntl(0, F_SETFL, O_NONBLOCK);
	tcgetattr(0, &attr);
	attr.c_lflag &= ~ICANON;
	attr.c_lflag &= ECHO;
	tcsetattr(0, TCSANOW, &attr);
	//this step spawns the sensor "notice me sensor"
	//mmap the response memory
	int fd = shm_open("/sensor_memory", O_RDWR, 0);
	sensor_response *response = (sensor_response *) mmap(NULL, __PAGESIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);



	Info info;
	info.x = 12;
	info.y = 12;
	info.direction = SOUTH;

	pid_t pid_sensor, pid_movement, pid_demine;
	pthread_create(&pid_sensor, NULL, sensor, (void *) &info);
	pthread_create(&pid_movement, NULL, movement, (void *) &info);
	pthread_create(&pid_demine, NULL, demine, (void *) &info);

	struct sched_param params;
	pthread_attr_t attributes;

	pthread_attr_init(&attributes);
	pthread_attr_setinheritsched(&attributes, PTHREAD_EXPLICIT_SCHED);
	params.sched_priority = 1;
	pthread_attr_setschedpolicy(&attributes, SCHED_RR);
	pthread_attr_setschedparam(&attributes, &params);


	//while loop that reads memory and changes priority of thread
	while(1) {
		pthread_mutex_lock(&mutex);
		while(state != WAIT) {
			pthread_cond_wait(&cond, &mutex);
		}

		state = SENSOR;

		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);

		pthread_mutex_lock(&mutex);
		while(state != WAIT) pthread_cond_wait(&cond, &mutex);

		if(response->distance > 1) {
			printf("robot x:%d\trobot y: %d\n", info.x, info.y);
			printf("Facing Direction: %c\n", info.direction);
			printf("distance to mine: %d\n", response->distance);
			state = NO_MINE;
		} else {
			info.demine = true;
			state = NEAR_MINE;
		}
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}

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
		pthread_mutex_lock(&mutex);
		while(state != SENSOR) {
			pthread_cond_wait(&cond, &mutex);
		}
		MsgSend(sensor_coid,info, sizeof(Info), NULL, 0);
		usleep(1000000);
		state = WAIT;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);

	}
}

void *movement(void *arg) {

	Info *info = (Info *) arg;

	while(1) {
		pthread_mutex_lock(&mutex);
		while(state != NO_MINE) {
			pthread_cond_wait(&cond, &mutex);
		}
		char key = 0;
		key = getchar();
		if(key == 'w' || key == 'W') {
			if(info->direction != NORTH) info->direction = NORTH;
			else info->y--;
		}
		else if(key == 's' || key == 'S') {
			if(info->direction != SOUTH) info->direction = SOUTH;
			else info->y++;
		}
		else if(key == 'a' || key == 'A') {
			if(info->direction != SOUTH) info->direction = WEST;
			else info->x--;
		}
		else if(key == 'd' || key == 'D') {
			if(info->direction != EAST) info->direction = EAST;
			else info->x++;
		}
		while(getchar() != -1);
		state = WAIT;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
		usleep(500000);
	}
}

/**
 * Ok so the default sensor is something, but then it goes above and beyond if something happens
 */
void *demine(void *arg) {
	Info *info = arg;
	int coid = name_open(ATTACH_POINT, 0);
	while(1) {
		pthread_mutex_lock(&mutex);
		while(state != NEAR_MINE) {
			pthread_cond_wait(&cond, &mutex);
		}
		printf("Demining...\n");
		MsgSendPulse(coid, 1, 2, info->direction);
		sleep(3);
		printf("Demining successful\n");
		state = WAIT;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}
}

void *rotate(void *arg) {
	Info *info = arg;

}
