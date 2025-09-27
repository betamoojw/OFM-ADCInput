#include "ADCInputModule.h"

#if defined(OPENKNX_ADC_ADS_WIRE) && defined(OPENKNX_ADC_ADS_ADDR) && defined(OPENKNX_ADC_ADS_SDA) && defined(OPENKNX_ADC_ADS_SCL) && defined(OPENKNX_ADC_ADS_GAIN) && ADC_ChannelCount > 0


ADS1115 I2CADS1115(OPENKNX_ADC_ADS_ADDR, &OPENKNX_ADC_ADS_WIRE);

enum State {
  Set = 1,
  Read = 2,
};
State ADC_State = Set;

enum ADC_enum_CH { 
  ADC_1,
  ADC_2,
  ADC_3,
  ADC_4,
};
ADC_enum_CH ADC_CH = ADC_1;

const std::string ADCInputModule::name() { return "ADCInput"; }

const std::string ADCInputModule::version() { return MODULE_ADCInput_Version; }

void ADCInputModule::setup() 
  {

  OPENKNX_ADC_ADS_WIRE.setSDA(OPENKNX_ADC_ADS_SDA);
  OPENKNX_ADC_ADS_WIRE.setSCL(OPENKNX_ADC_ADS_SCL);
  OPENKNX_ADC_ADS_WIRE.begin();  
  
  I2CADS1115.begin();
  if (I2CADS1115.isConnected()) {
    // I2C_adc.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV
    // I2C_adc.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV
    // I2C_adc.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV
    // I2C_adc.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV
    // I2C_adc.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV
    // I2C_adc.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit =
    // 0.125mV
    I2CADS1115.setGain(OPENKNX_ADC_ADS_GAIN);
    // ADC_to_voltage_factor = ADS1115_TOP.toVoltage(); //  voltage factor
    logInfoP("Analog-Input successfully initialized");
  } else {
    logInfoP("Analog-Input could not be initialized (hardware ADS1115 missing?)");
  }


  for (uint8_t i = 0; i < MIN(ADC_ChannelCount,AS1115_MAX_CH); i++) {
    _channels[i] = new ADCInputChannel(i);
    _channels[i]->setup();
  }
}

void ADCInputModule::loop() {

  if (delayCheck(_lastPollingTime, 100)) {
    _lastPollingTime = delayTimerInit();

    processInput();

    for (uint8_t i = 0; i < MIN(ADC_ChannelCount,AS1115_MAX_CH); i++)
      _channels[i]->loop();
  }
}

void ADCInputModule::processInput() {
  // no hw state available


  // Process für ADS1115
  switch (ADC_State) {
  case Set:
    switch (ADC_CH) {
    case ADC_1:
      I2CADS1115.requestADC(0); //  CH1 -> liegt auch ADC1
      break;
    case ADC_2:
      I2CADS1115.requestADC(1); //  CH2 -> liegt auch ADC2
      break;
    case ADC_3:
      I2CADS1115.requestADC(2); //  CH3 -> liegt auch ADC3
      break;
    case ADC_4:
      I2CADS1115.requestADC(3); // CH4  > liegt auch ADC4
      break;
    default:
      logDebugP("Wrong StateADC TOP CH");
      break;
    }
    ADC_State = Read;
    break;
  
    case Read:
    if (I2CADS1115.isBusy() == false) {
      switch (ADC_CH) {
      case ADC_1: // CH1
        _channels[0]->getADC_value(I2CADS1115.getValue());
        ADC_CH = ADC_2;
        break;
      case ADC_2: // CH2
        _channels[1]->getADC_value(I2CADS1115.getValue());
        ADC_CH = ADC_3;
        break;
      case ADC_3: // CH3
        _channels[2]->getADC_value(I2CADS1115.getValue());
        ADC_CH = ADC_4;
        break;
      case ADC_4: // HSS
        _channels[3]->getADC_value(I2CADS1115.getValue());
        ADC_CH = ADC_1;
        ADC_State = Set; // Needed because of the Return true
        // return true;
        break;

      default:
        logDebugP("Wrong StateADC TOP CH");
        break;
      }
      ADC_State = Set;
    }
    break;

  default:
    logDebugP("Wrong StateADC TOP");
    break;
  }
}

ADCInputModule openknxADCInputModule;

#endif