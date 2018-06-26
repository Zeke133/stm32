#ifndef _OUTPUT_INTERFACE_H
#define _OUTPUT_INTERFACE_H

class OutputInterface {
    
public:

    virtual void send(char byte);
    virtual void send(const char * string);

};


#endif
