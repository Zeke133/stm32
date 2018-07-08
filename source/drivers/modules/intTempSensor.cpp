#include <intTempSensor.h>


InternalTemp::InternalTemp(ADC& adc, uint32_t chTemp)
    : adcRef(adc) {

    channelTemp = chTemp;

}

int32_t InternalTemp::getTemperatureC(void) {

    uint32_t val = adcRef.getValue(channelTemp);
    
    return ((V25 - val) / AvgSlope + 25);
}

void InternalTemp::calibrate(int32_t temp) {

    uint32_t val = adcRef.getValue(channelTemp);

    V25 = temp * (AvgSlope + 25) + val;
}


