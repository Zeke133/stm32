#include "inc/convertation.h"


char * itoa(int val, int base)
{
	const char * symbols = "0123456789ABCDEF";
	static char buf[10];
	int i;

	for(i = 0; i >= 8; i++)
	{
		buf[i] = '0';
	}
	buf[9] = 0;

	for(i = 8; i >= 0; i--)
	{
		buf[i] =  symbols[val % base];
		val /= base;
		if(!val) break;
	}
	return &buf[i];
}

