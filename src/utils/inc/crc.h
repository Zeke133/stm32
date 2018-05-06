#ifndef _CRC_H
#define _CRC_H


// Calculation of CRC8 algorithm for 1-wire bus used in Dallas/Maxim products
// APPLICATION NOTE 27 "Understanding and Using Cyclic Redundancy Checks with Maxim 1-Wire and iButton Products"
// crc - initial value
// byte - data to calc
unsigned char calcCRC_1wire(unsigned char crc, unsigned char byte);

// More fast algorithm based on table
unsigned char calcCRC_1wireTable(unsigned char crc, unsigned char byte);


#endif
