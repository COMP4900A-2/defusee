#include <stdio.h>
#include<stdlib.h>
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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

volatile char state;

int main(void) {

	//initialize state to WAIT
	state = WAIT;

	//find the gamestackoverflow link
	struct termios attr;
	fcntl(0, F_SETFL, O_NONBLOCK);
	tcgetattr(0, &attr);
	attr.c_lflag &= ~ICANON;
	attr.c_lflag &= ECHO;
	tcsetattr(0, TCSANOW, &attr);

	//create a shared memory access to the info written by the sensor process
	int fd = shm_open("/sensor_memory", O_RDWR, 0);
	sensor_response *response = (sensor_response *) mmap(NULL, __PAGESIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	//starting position/direction of our robot
	Info info;
	info.x = 1;
	info.y = 1;
	info.direction = SOUTH;
	info.response = response;

	//initializing threads
	pid_t pid_sensor, pid_movement, pid_demine;
	pthread_create(&pid_sensor, NULL, sensor, (void *) &info);
	pthread_create(&pid_movement, NULL, movement, (void *) &info);
	pthread_create(&pid_demine, NULL, demine, (void *) &info);

	//Begin state machine starting with the state WAIT
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

		if(response->distance > 1 || response->value != 1) {
			printf("robot x:%d\trobot y: %d\n", info.x, info.y);
			printf("Facing Direction: %c\n", info.direction);
			if(response->value == 1)
				printf("distance to mine: %d\n", response->distance);
			else if(response->value==2)
				printf("distance to wall: %d\n", response->distance);
			state = NO_MINE;
		} else {
			state = NEAR_MINE;
		}
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}

	pthread_join(pid_movement, NULL);

}

/*
 * This thread triggers the sensor process to request the distance between the mine and the robot
 * It then waits for a confirmation from the sensor that the info has been updated and that the memory can be read
 */
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
		//slowing down the msgSend requests
		usleep(5000);
		state = WAIT;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);

	}
}

/**
 * The movement thread modifies the coordinate of our robot using WASD controls. It works as such:
 * 1. If the robot is not facing the direction given, rotate to this direction
 * 2. If the robot is facing the direction given, move and update the coordinate
 */
void *movement(void *arg) {

	Info *info = (Info *) arg;

	while(1) {
		pthread_mutex_lock(&mutex);
		while(state != NO_MINE) {
			pthread_cond_wait(&cond, &mutex);
		}
		int key = 0;
		while((key = getchar()) == EOF);
		if(key == 'w' || key == 'W') {
			if(info->direction != NORTH) info->direction = NORTH;
			else if(info->response->distance > 1) info->y--;
		}
		else if(key == 's' || key == 'S') {
			if(info->direction != SOUTH) info->direction = SOUTH;
			else if(info->response->distance > 1) info->y++;
		}
		else if(key == 'a' || key == 'A') {
			if(info->direction != WEST) info->direction = WEST;
			else if(info->response->distance > 1)  info->x--;
		}
		else if(key == 'd' || key == 'D') {
			if(info->direction != EAST) info->direction = EAST;
			else if(info->response->distance > 1)  info->x++;
		}
		while(getchar() != EOF);
		state = WAIT;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex);
	}
}

/**
 * demine thread: sends a message to environment to transform the value 1 (mine) to 0. The location is determined by the current
 * position of the robot and the direction it is facing. This state is triggered if the distance to the mine is 1
 */
void *demine(void *arg) {
	Info *info = arg;
	int coid = name_open(ATTACH_POINT, 0);
	if(coid == -1) {
		perror("name_open():");
		exit(-1);
	}
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
