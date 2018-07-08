#ifndef _ANALOG_SENSORS_H
#define _ANALOG_SENSORS_H

#include <adc.h>

// Does not calibrated properly, so need to be veryfied and may be not usefull at all

class InternalTemp {

friend int main(void);
    
public:

    int32_t getTemperatureC(void);
    void calibrate(int32_t tempC);

    uint16_t getV25(void) {
        return V25;
    }

    // delete copy constructor and assignment operator
    InternalTemp(const InternalTemp&) = delete;
    InternalTemp& operator=(const InternalTemp&) = delete;

private:

    InternalTemp(ADC& adcRef, uint32_t chTemp);

    const ADC& adcRef;
    uint32_t channelTemp;

    uint16_t V25 = 1700;
    uint16_t AvgSlope = 5;

    // Average slope 4.0 4.3 4.6 mV/°C
    // Voltage at 25 °C 1.34 1.43 1.52 V

    // Temperature (in °C) = {(V25 - VSENSE) / Avg_Slope} + 25.
    // Where,
    // V25 = VSENSE value for 25° C and
    // Avg_Slope = Average Slope for curve between Temperature vs. VSENSE (given in
    // mV/° C or µV/ °C).
    // Refer to the Electrical characteristics section for the actual values of V25 and
    // Avg_Slope.

};


#endif

