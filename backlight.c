#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "errno.h"

int read_value(char *fn) {
	char buf[16];

	int fd = open(fn, O_RDONLY);
	if(fd < 0) {
		fprintf(stderr, "open(%s) for reading failed: %s\n", fn, strerror(errno));
		return -1;
	}

	memset(&buf, 0, sizeof(buf));

	ssize_t len = read(fd, &buf, sizeof(buf) - 1);
	if(len < 0) {
		fprintf(stderr, "read(%s) failed: %s\n", fn, strerror(errno));
		return -1;
	}

	int value = atoi((char*)&buf);

	close(fd);
	return value;
}

int write_value(char *fn, int value) {
	char buf[16];

	int fd = open(fn, O_WRONLY);
	if(fd < 0) {
		fprintf(stderr, "open(%s) for writing failed: %s\n", fn, strerror(errno));
		return -1;
	}

	int len = snprintf((char*)&buf, 15, "%d", value);

	if(write(fd, &buf, len) != len) {
		fprintf(stderr, "write(%s) failed: %s\n", fn, strerror(errno));
		return -1;
	}

	close(fd);
	return 0;
}


int main(int argc, char* argv[]) {
	// read current and max brightness
	int actual_brightness = read_value("/sys/class/backlight/intel_backlight/actual_brightness");
	int max_brightness = read_value("/sys/class/backlight/intel_backlight/max_brightness");
	if((actual_brightness < 0) || (max_brightness < 0)) {
		return -1;
	}

	// calculate 5% difference
	int diff_brightness = max_brightness * 0.05;
	if(diff_brightness == 0) {
		diff_brightness = 1;
	}

	// new brightness based on command name
	int new_br = actual_brightness;
	if(strcmp(argv[0], "backlight_up") == 0) {
		new_br += diff_brightness;
	}
	if(strcmp(argv[0], "backlight_down") == 0) {
		new_br -= diff_brightness;
	}

	// new brightness as parameter
	if(argc == 2) {
		new_br = atoi(argv[1]);
	}

	// update brightness
	if(new_br > max_brightness) {
		new_br = max_brightness;
	}
	if(new_br < 0) {
		new_br = 0;
	}
	if(new_br != actual_brightness) {
		if(write_value("/sys/class/backlight/intel_backlight/brightness", new_br) != 0) {
			return -1;
		}
	}

	printf("%d -> %d [%d]\n", actual_brightness, new_br, max_brightness);
	return 0;
}
