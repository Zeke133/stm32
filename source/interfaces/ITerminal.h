#ifndef _I_TERMINAL_H
#define _I_TERMINAL_H

// using
#include <ITextOutput.h>
#include <stdint.h>

/*
Interface for text terminal.
*/
class ITerminal : public ITextOutput {

public:

    virtual uint8_t getWidth(void) const = 0;
    virtual uint8_t getHeight(void) const = 0;

    virtual void clearRow(uint8_t row) = 0;
    virtual void setCursor(uint8_t row, uint8_t column) = 0;

    // pretendents
    // void clear(void);
    // void home(void);

};

#endif
