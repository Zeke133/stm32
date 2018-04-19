#include "inc/convertation.h"


const char * itoa(unsigned int val, int base) {

	const char * symbols = "0123456789ABCDEF";
	static char buf[11];
	int i;

	for(i = 0; i < 10; i++) {
		buf[i] = '0';
	}

	buf[i--] = 0;

	while(1) {

		if(i < 0) return buf;

		buf[i] = symbols[val % base];
		val /= base;

		if(val == 0) return &buf[i];
		i--;
	}
	return "ERROR!";
}

