#pragma once
#include "ADCInputChannel.h"
#include "OpenKNX/Module.h"
#include "hardware.h"
#include "ADS1X15.h"


#define ADS1115_MAX_CH 4


class ADCInputModule : public OpenKNX::Module
{

  public:
    const std::string name() override;
    const std::string version() override;
    void loop() override;
    void setup() override;

  private:
    void processInput();

    uint32_t _lastPollingTime = 0;
    bool _adcAvailable = false;

    ADCInputChannel* _channels[ADS1115_MAX_CH] = {};
};

extern ADCInputModule openknxADCInputModule;
