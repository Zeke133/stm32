#include <oStream.h>

OStream::OStream(Usart& driver)
    : usart(driver) {
    
    //
}

OStream& OStream::operator<<(enum OutSet manipulator) {
    if (manipulator == OutSet::dec) outputNumbersBase = 10;
    else if (manipulator == OutSet::hex) outputNumbersBase = 16;
    return *this;
};

OStream& OStream::operator<<(char symbol) {
    usart.putc(symbol);
    return *this;
};

OStream& OStream::operator<<(const char * cString) {
    usart.puts(cString);
    return *this;
};

OStream& OStream::operator<<(uint8_t num) {
    usart.puts(itoa(num, outputNumbersBase));
    return *this;
};
OStream& OStream::operator<<(uint16_t num) {
    usart.puts(itoa(num, outputNumbersBase));
    return *this;
};
OStream& OStream::operator<<(int16_t num) {
    if (num < 0) usart.putc('-');
    usart.puts(itoa(num, outputNumbersBase));
    return *this;
};
OStream& OStream::operator<<(uint32_t num) {
    usart.puts(itoa(num, outputNumbersBase));
    return *this;
};
OStream& OStream::operator<<(int32_t num) {
    if (num < 0) usart.putc('-');
    usart.puts(itoa(num, outputNumbersBase));
    return *this;
};


