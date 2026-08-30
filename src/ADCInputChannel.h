#pragma once
#include "OpenKNX.h"

#define HW42_0_12V 0
#define HW42_0_8V 1

class ADCInputChannel : public OpenKNX::Channel
{

  protected:
    void processInput();
    void processPeriodicSend();
    void sendState();
    void sendState2();
    void sendState3();
    void brokenCableDetection(float value);
    float calculateSensorValueLinearFunction(float a, float b);
    float calculateSensorValueLinearFunctionCur(float a, float b);
    float getPinInputVoltage(); // Voltage in mV
    float getPinInputCurrent(); // Voltage in mA
    float checkZero(float value);

    union InputADCValuesOLD
    {
        float ladcValue;
        // uint16_t ladcValueU16[ADC_ChannelCount];
        // uint8_t lsoilmoistureU8[ADC_ChannelCount];
    } valueOld;

    uint8_t value2;     // Value Füllstandsensor Prozent
    uint8_t valueOld2;  // Value Füllstandsensor Prozent
    uint16_t value3;    // Value Füllhöhe in mm
    uint16_t valueOld3; // Value Füllhöhe in mm

    uint8_t _channelIndex;
    long _adcValue;
    bool _hasFirstValue = false;
    bool _cableBroken = false;

    uint32_t _lastPeriodicSend = 0;
    uint32_t _lastPeriodicSend2 = 0;
    uint32_t _lastPeriodicSend3 = 0;

    /*
    bool _paramActive;
    uint8_t _paramOpen;
    uint8_t _paramClose;
    uint32_t _paramDebouncing;
    uint32_t _paramPeriodic;
    uint8_t _paramPulsing;

    int8_t _currentState = -1;
    int8_t _lastDebounceState = -1;
    int8_t _currentHardwareState = -1;

    uint32_t _lastDebounceTime = 0;

    */

  public:
    ADCInputChannel(uint8_t index);

    void setup() override;
    void loop() override;
    const std::string name() override;

    void getADC_value(long value);

    // void setHardwareState(bool state);
    bool isActive();
};
