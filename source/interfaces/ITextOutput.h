#ifndef _I_TEXT_OUTPUT_H
#define _I_TEXT_OUTPUT_H

/*
Interface for text output.
*/
class ITextOutput {

public:

    virtual void putc(char byte) = 0;
    virtual void puts(const char *str) = 0;

};

#endif
