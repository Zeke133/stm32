#include <analogSensors.h>


InternalTemp::InternalTemp(ADC& adc, uint32_t chTemp)
    : adcRef(adc) {

    channelTemp = chTemp;

}

int32_t InternalTemp::getTemperatureC(void) {

    uint32_t val = adcRef.getValue(channelTemp);
    
    return ((V25 - val) * 10 / AvgSlope + 25);
}

void InternalTemp::calibrate(int32_t temp) {

    uint32_t val = adcRef.getValue(channelTemp);

    V25 = (temp - 25) * AvgSlope / 10 + val;
}


