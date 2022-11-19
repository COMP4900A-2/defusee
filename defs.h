#define ATTACH_POINT	"environment"

#define NORTH 'N'
#define SOUTH 'S'
#define EAST 'E'
#define WEST 'W'
#define SENSOR_TRIGGER 1
#define SENSOR_ATTACH "sensor_attach"

	struct sensor_response {
		unsigned int x;
		unsigned int y;// coordinate [x, y] of the objects found in the environment
		char value; // 1 is a mine and 2 is an obstacle
	} typedef sensor_response;

	struct info {
		unsigned int x;
		unsigned int y;
		char direction;
		sensor_response sr;
	} typedef Info;
