#ifndef _I_PROFILER_H
#define _I_PROFILER_H

#include <stdint.h>

class IProfiler {

public:

    virtual void startProfiling(void) = 0;
    virtual uint32_t getExecutionTicks(void) = 0;

};

#endif

