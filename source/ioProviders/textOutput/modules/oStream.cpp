#include <oStream.h>

OStream::OStream(IWriter& driver)
    : device(driver) {
    
    //
}

OStream& OStream::operator<<(enum OutSet manipulator) {
    if (manipulator == OutSet::dec) outputNumbersBase = 10;
    else if (manipulator == OutSet::hex) outputNumbersBase = 16;
    return *this;
};

OStream& OStream::operator<<(char symbol) {
    device.putc(symbol);
    return *this;
};

OStream& OStream::operator<<(const char * cString) {
    device.puts(cString);
    return *this;
};

OStream& OStream::operator<<(uint8_t num) {
    device.puts(itoa(num, outputNumbersBase));
    return *this;
};
OStream& OStream::operator<<(uint16_t num) {
    device.puts(itoa(num, outputNumbersBase));
    return *this;
};
OStream& OStream::operator<<(int16_t num) {
    if (num < 0) device.putc('-');
    device.puts(itoa(num, outputNumbersBase));
    return *this;
};
OStream& OStream::operator<<(uint32_t num) {
    device.puts(itoa(num, outputNumbersBase));
    return *this;
};
OStream& OStream::operator<<(int32_t num) {
    if (num < 0) device.putc('-');
    device.puts(itoa(num, outputNumbersBase));
    return *this;
};


