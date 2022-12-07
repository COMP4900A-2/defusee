#include <stdbool.h>

#define ATTACH_POINT	"environment"

#define NORTH 'N'
#define SOUTH 'S'
#define EAST 'E'
#define WEST 'W'
#define SENSOR_TRIGGER 1
#define SENSOR_ATTACH "sensor_attach"

//states
#define NO_MINE 0
#define NEAR_MINE 1
#define WAIT 2
#define SENSOR 3


	struct sensor_response {
		unsigned char distance;
		unsigned char value; // 1 is a mine and 2 is an obstacle
	} typedef sensor_response;

	struct info {
		unsigned char x;
		unsigned char y;
		unsigned char direction;
		sensor_response *response;
	} typedef Info;
