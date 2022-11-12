#define ATTACH_POINT	"environment"

#define NORTH 'N'
#define SOUTH 'S'
#define EAST 'E'
#define WEST 'W'

	struct sensor_response {
		char x;
		char y;// coordinate [x, y] of the objects found in the environment
		char value; // 1 is a mine and 2 is an obstacle
	} typedef sensor_response;

	struct info {
		char x;
		char y;
		char direction;
		sensor_response sr;
	} typedef Info;
