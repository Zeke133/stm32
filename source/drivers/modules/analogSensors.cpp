#include <analogSensors.h>


InternalTemp::InternalTemp(ADC& adc, uint32_t chTemp)
    : adcRef(adc) {

    channelTemp = chTemp;

}

int32_t InternalTemp::getTemperatureC(void) {

    uint32_t val = adcRef.getValue(channelTemp);
    
    return ((V22 - val) / AvgSlope + 22);
}

void InternalTemp::calibrate(int32_t temp) {

    uint32_t val = adcRef.getValue(channelTemp);

    V22 = temp * (AvgSlope + 22) + val;
}


