#include <stdio.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <sched.h>

void *sensor(void *);

int main(void) {

	puts("Hello World!!!"); /* prints Hello World!!! */
	return EXIT_SUCCESS;

	pthread_t sensor_thread_id = pthread_create(NULL, NULL, sensor, (void *) TheStruct);
	pthread_join(sensor_thread_id, NULL);
}

void *sensor(void * arg) {
	//attach to name
	int coid = name_open("environment",0);
	while(1) {
	}
}
