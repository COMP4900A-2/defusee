#include <stdbool.h>

#define ATTACH_POINT	"environment"

#define NORTH 'N'
#define SOUTH 'S'
#define EAST 'E'
#define WEST 'W'
#define SENSOR_TRIGGER 1
#define SENSOR_ATTACH "sensor_attach"
#define NO_MINE 0
#define NEAR_MINE 1
#define WAIT 2
#define SENSOR 3

#define PORT (uint16_t) 3000
#define SERVER "10.0.0.183"

	struct sensor_response {
		char distance;
		char unsigned value; // 1 is a mine and 2 is an obstacle
	} typedef sensor_response;

	struct info {
		unsigned char x;
		unsigned char y;
		unsigned char direction;
		bool demine;

	} typedef Info;
