#ifndef _ANALOG_SENSORS_H
#define _ANALOG_SENSORS_H

#include <adc.h>


class InternalTemp {

friend int main(void);
    
public:

    int32_t getTemperatureC(void);
    void calibrate(int32_t tempC);

    uint16_t getV22(void) {
        return V22;
    }

    // delete copy constructor and assignment operator
    InternalTemp(const InternalTemp&) = delete;
    InternalTemp& operator=(const InternalTemp&) = delete;

private:

    InternalTemp(ADC& adcRef, uint32_t chTemp);

    const ADC& adcRef;
    uint32_t channelTemp;

    uint16_t V22 = 1700;     /*at 22.7*/       // when V25 = 1.41V at ref 3.3V - THIS MUST BE CALIBRATED!
    uint16_t AvgSlope = 5;          // when avg_slope = 4.3mV/C at ref 3.3V

    // Average slope 4.0 4.3 4.6 mV/°C
    // Voltage at 25 °C 1.34 1.43 1.52 V

};


#endif

