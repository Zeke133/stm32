#include <oStream.h>



OStream& OStream::operator<<(enum OutSet manipulator) {
    if (manipulator == OutSet::dec) outputNumbersBase = 10;
    else if (manipulator == OutSet::hex) outputNumbersBase = 16;
    return *this;
};

OStream& OStream::operator<<(char symbol) {
    device.send(symbol);
    return *this;
};

OStream& OStream::operator<<(const char * cString) {
    device.send(cString);
    return *this;
};

OStream& OStream::operator<<(unsigned char num) {
    device.send(itoa(num, outputNumbersBase));
    return *this;
};

OStream& OStream::operator<<(short num) {
    device.send(itoa(num, outputNumbersBase));
    return *this;
};

OStream& OStream::operator<<(int num) {
    device.send(itoa(num, outputNumbersBase));
    return *this;
};