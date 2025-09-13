#include "ADCInputChannel.h"

#define Channel_inaktiv 0
#define ADC_Wert 1
#define SMT50_Bodenfeuchte 2
#define SMT50_BodenTemperatur 3

#define SensorType_voltage 0
#define SensorType_temperature 1
#define SensorType_humidity 2
#define SensorType_co2 3
#define SensorType_lux 4
#define SensorType_pressure 5
#define SensorType_flow 6
#define SensorType_rain 7
#define SensorType_windspeed 8
#define SensorType_percent 9

ADCInputChannel::ADCInputChannel(uint8_t index) { _channelIndex = index; }

const std::string ADCInputChannel::name() { return "ADCInput"; }

void ADCInputChannel::setup()
{
    if (ParamADC_CHSensorType == 0)
        return;

    // KoBI_ChannelOutput.valueNoSend(false, DPT_Switch);

    // Debug
    logTraceP("paramSensorTyp: %i", ParamADC_CHSensorType);
    logTraceP("paramSendCycle: %i", ParamADC_CHSendcycletime);
    logTraceP("paramSendABS: %i", ParamADC_CHSendenAbsolut);
    logTraceP("paramSendREL: %i", ParamADC_CHSendenRelativ);
    logTraceP("paramVoltKorr: %i", ParamADC_CHVoltageCorrection);
}

void ADCInputChannel::loop()
{

    processInput();
    // processPeriodicSend();
}

void ADCInputChannel::processInput()
{
    bool lSend = false;
    float lAbsolute;
    float absValue;
    // uint16_t lAbsoluteU16;
    // uint8_t lRelativU8;
    // uint8_t Dpt = 0;

    // uint32_t lCycle;

    union InputADCValues
    {
        float ladcValue;
        // uint16_t ladcValueU16;
        // uint8_t lsoilmoistureU8;
    } value;
    value.ladcValue = 0;

    if (ParamADC_CHSensorType == 0)
        return;

    // Auswertung
    switch (ParamADC_CHSensorType)
    {
        /**********************************************************************************************************
         *              ADC Voltage                                                                        *
         *********************************************************************************************************/
        case ADC_Wert:
            // STEP 1: read Parameter DPT Format
            switch (ParamADC_CHSensorTypes)
            {
                case SensorType_voltage: // DPT9.020 (mV)
                    // STEP 2: Get new Sensor value
                    value.ladcValue = getPinInputVoltage() * 1000.0;
                    // STEP 2a: Get Abs value
                    lAbsolute = ParamADC_CHSendenAbsolut; // Value in mV

                    break;

                default:
                    // STEP 2: Get new Sensor value
                    //value.ladcValue = calculateSensorValueLinearFunction(ParamADC_CHGeradeM / 100.0, ParamADC_CHGeradeB / 100.0);
                    value.ladcValue = (ParamADC_CHGeradeM / 100.0)*getPinInputVoltage() + ParamADC_CHGeradeB / 100.0;
#ifdef InputADC_Output
                    SERIAL_PORT.print(value.ladcValue);
                    SERIAL_PORT.print(" | ");
                    SERIAL_PORT.print(ParamADC_CHGeradeM / 100.0);
                    SERIAL_PORT.print(" | ");
                    SERIAL_PORT.print(ParamADC_CHGeradeB / 100.0);
                    SERIAL_PORT.print(" | ");
#endif
                    // STEP 3: Check value Change "Absolut"
                    lAbsolute = ParamADC_CHSendenAbsolut;
                    break;
            }
            // STEP 3a: Check if Change detected
            if (lAbsolute > 0 && (abs(value.ladcValue - valueOld.ladcValue)) >= lAbsolute)
            {
                lSend = true;
#ifdef InputADC_Output
                SERIAL_PORT.print(" Abs ");
#endif
            }

            // STEP 3b: Check if Change detected
            lAbsolute = ParamADC_CHSendenRelativ;
            if (lAbsolute > 0 && value.ladcValue > 0.2 && (abs(value.ladcValue - valueOld.ladcValue)) >= value.ladcValue / 100 * lAbsolute)
            {
                lSend = true;
#ifdef InputADC_Output
                SERIAL_PORT.print(" Rel ");
#endif
            }

            // STEP 4: Preset KO
            switch (ParamADC_CHSensorTypes)
            {
                case SensorType_voltage: // DPT9.020 (mV)
#ifdef InputADC_Output
                    SERIAL_PORT.println(value.ladcValue);
#endif

                    // we always store the new value in KO, even it it is not sent (to satisfy potential read request)
                    KoADC_ChannelOutput.valueNoSend(value.ladcValue, DPT_Value_Volt);
                    break;
                case SensorType_percent:
#ifdef InputADC_Output
                    SERIAL_PORT.println(value.ladcValue);
#endif
                    // we always store the new value in KO, even it it is not sent (to satisfy potential read request)
                    KoADC_ChannelOutput.valueNoSend(value.ladcValue * 2.55, DPT_Percent_U8); //****************************************************** */
                    break;

                default:
#ifdef InputADC_Output
                    SERIAL_PORT.println(value.ladcValue);
#endif
                    // we always store the new value in KO, even it it is not sent (to satisfy potential read request)
                    KoADC_ChannelOutput.valueNoSend(value.ladcValue, DPT_Value_Temp); //**************  DPT ???  ************ */
                    break;
            }
            break;
        /**********************************************************************************************************
         *              SMT50 Bodenfeuchte                                                                        *
         *********************************************************************************************************/
        case SMT50_Bodenfeuchte:

            value.ladcValue = calculateSensorValueLinearFunction(0.06, 0);

            // senden bei Wertänderung Absolut
            lAbsolute = ParamADC_CHSendenAbsolut; // Value in %
            if (lAbsolute > 0 && roundf(abs(value.ladcValue - valueOld.ladcValue)) >= lAbsolute)
            {
                lSend = true;
#ifdef InputADC_Output
                SERIAL_PORT.print("Abs ");
#endif
            }
            // we always store the new value in KO, even it it is not sent (to satisfy potential read request)
            if (ParamADC_CHSMT50DPTType== 0)
            {
#ifdef InputADC_Output
                SERIAL_PORT.print("DPT5: ");
                SERIAL_PORT.println(value.ladcValue);
#endif
                KoADC_ChannelOutput.valueNoSend((uint8_t)(value.ladcValue * 2.55), DPT_Percent_U8);
            }
            else
            {
#ifdef InputADC_Output
                SERIAL_PORT.print("DPT9: ");
                SERIAL_PORT.println(value.ladcValue);
#endif
                KoADC_ChannelOutput.valueNoSend(value.ladcValue, DPT_Value_Humidity);
            }
            break;
        /**********************************************************************************************************
         *              SMT50 BodenTemperatur                                                                     *
         *********************************************************************************************************/
        case SMT50_BodenTemperatur:

            // value.ladcValue = calculateSensorValue(channel, 0, 450); // x1 = 0.  x2 = 450°C
            value.ladcValue = calculateSensorValueLinearFunction(0.01, 0.5);

            // senden bei Wertänderung Absolut
            lAbsolute = ParamADC_CHSendenAbsolut / 10.0; // Value in 0.1°C
            if (lAbsolute > 0 && (abs(value.ladcValue - valueOld.ladcValue)) >= lAbsolute)
            {
                lSend = true;
            }
            // senden bei Wertänderung Relativ
            lAbsolute = ParamADC_CHSendenRelativ;
            absValue = abs(value.ladcValue);
            if (lAbsolute > 0 && (absValue / 100 * lAbsolute) > 0.1 && abs(absValue - abs(valueOld.ladcValue)) >= (absValue / 100 * lAbsolute))
            {
                lSend = true;
            }

            // we always store the new value in KO, even it it is not sent (to satisfy potential read request)
            KoADC_ChannelOutput.valueNoSend(value.ladcValue, DPT_Value_Temp);

            break;

        default:
#ifdef InputADC_Output
            SERIAL_PORT.print("Wrong ADC Type Parameter");
#endif
            break;
    }

    // first run - skip
    if (_lastPeriodicSend == 0)
    {
        _lastPeriodicSend = millis();
        return;
    }

    if (delayCheck(_lastPeriodicSend, ParamADC_CHSendcycletime * 1000))
    {
        _lastPeriodicSend = millis();
        lSend = true;
    }

    if (lSend)
    {
        sendState();
        valueOld.ladcValue = value.ladcValue;
        lSend = false;
    }
}

void ADCInputChannel::getADC_value(long value)
{
    _adcValue = value;

    // Hier muss senden bei Wertänderung drin sein !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void ADCInputChannel::processPeriodicSend()
{

    uint32_t paramValue = ParamADC_CHSendcycletime * 1000;

    // periodic send is disabled
    if (paramValue == 0)
        return;

    // first run - skip
    if (_lastPeriodicSend == 0)
    {
        _lastPeriodicSend = millis();
        return;
    }

    if (delayCheck(_lastPeriodicSend, paramValue))
    {
        _lastPeriodicSend = millis();
        sendState();
    }
}

void ADCInputChannel::sendState()
{

    Serial.print("----> ADC_");
    Serial.print(_channelIndex);
    Serial.print(": ");
    Serial.print(_adcValue);
    Serial.print(": ");
    Serial.println(getPinInputVoltage());

    logDebugP("sendState: %i", _adcValue);

    KoADC_ChannelOutput.objectWritten();
}

bool ADCInputChannel::isActive()
{
    if (ParamADC_CHSensorType > 0)
        return true;
    else
        return false;
}

// Voltage am PIN-INPUT in mV
float ADCInputChannel::getPinInputVoltage()
{
    return checkZero((_adcValue * 0.000375) / (ParamADC_CHVoltageCorrection / 100.0)); // 2.047 / 32767.0 * 6.0;
}

float ADCInputChannel::calculateSensorValueLinearFunction(float a, float b)
{
    return (getPinInputVoltage() - b) / a;
}

float ADCInputChannel::checkZero(float value)
{
    if (value > 0.07)
        return value;
    else
        return 0;
}