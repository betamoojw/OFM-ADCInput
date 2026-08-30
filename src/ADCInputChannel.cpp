#include "ADCInputChannel.h"

#define Channel_inaktiv 0
#define ADC_Wert 1
#define SMT50_Bodenfeuchte 2
#define SMT50_BodenTemperatur 3
#define ADC_4_20mA 150
#define ADC_4_20mA_Fuellstand 151

#define SensorType_voltage 0
#define SensorType_current 0
#define SensorType_temperature 1
#define SensorType_humidity 2
#define SensorType_co2 3
#define SensorType_lux 4
#define SensorType_pressure 5
#define SensorType_flow 6
#define SensorType_rain 7
#define SensorType_windspeed 8
#define SensorType_percent 9

// #define InputADC_Output

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
    if (!_hasFirstValue)
        return;

    processInput();
    // processPeriodicSend();
}

void ADCInputChannel::processInput()
{
    bool lSend = false;
    bool lSend2 = false;
    bool lSend3 = false;
    float lAbsolute;
    float lAbsolute2;
    float lAbsolute3;
    uint8_t meas_range;
    float percent;

    float absValue;
    // uint16_t lAbsoluteU16;
    // uint8_t lRelativU8;
    // uint8_t Dpt = 0;

    // uint32_t lCycle;

    union InputADCValues
    {
        float ladcValue;
        uint8_t percent;
        int16_t value;
        // uint16_t ladcValueU16;
        // uint8_t lsoilmoistureU8;
    } value;
    value.ladcValue = 0;

    if (ParamADC_CHSensorType == 0)
        return;

#ifdef InputADC_Output
    std::string debugOut = "";
#endif
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
                    // value.ladcValue = calculateSensorValueLinearFunction(ParamADC_CHGeradeM / 100.0, ParamADC_CHGeradeB / 100.0);
                    value.ladcValue = (ParamADC_CHGeradeM / 100.0) * getPinInputVoltage() + ParamADC_CHGeradeB / 100.0;
#ifdef InputADC_Output
                    debugOut = std::to_string(getPinInputVoltage()) + " | " +
                               std::to_string(ParamADC_CHGeradeM / 100.0) + " | " +
                               std::to_string(ParamADC_CHGeradeB / 100.0) + " | ";
#endif
                    // STEP 3: Check value Change "Absolut"
                    lAbsolute = ParamADC_CHSendenAbsolut;
                    break;
            }
            // STEP 3a: Check if Change detected
            if (lAbsolute > 0 && (abs(value.ladcValue - valueOld.ladcValue)) >= lAbsolute)
            {
                lSend = true;
                logDebugP("ABS");
#ifdef InputADC_Output
                debugOut += " Abs ";
#endif
            }

            // STEP 3b: Check if Change detected
            lAbsolute = ParamADC_CHSendenRelativ;
            if (lAbsolute > 0 && value.ladcValue > 0.2 && (abs(value.ladcValue - valueOld.ladcValue)) >= value.ladcValue / 100 * lAbsolute)
            {
                lSend = true;
                logDebugP("REL");
#ifdef InputADC_Output
                debugOut += " Rel ";
#endif
            }

            // STEP 4: Preset KO
            switch (ParamADC_CHSensorTypes)
            {
                case SensorType_voltage: // DPT9.020 (mV)
#ifdef InputADC_Output
                    logDebugP("mV: %s%i", debugOut.c_str(), value.ladcValue);
#endif

                    // we always store the new value in KO, even it it is not sent (to satisfy potential read request)
                    KoADC_ChannelOutput.valueNoSend(value.ladcValue, DPT_Value_Volt);
                    break;
                case SensorType_percent:
#ifdef InputADC_Output
                    logDebugP("prozent: %s%i", debugOut.c_str(), value.ladcValue);
#endif
                    // we always store the new value in KO, even it it is not sent (to satisfy potential read request)
                    KoADC_ChannelOutput.valueNoSend(value.ladcValue * 2.55, DPT_Percent_U8); //****************************************************** */
                    break;

                default:
#ifdef InputADC_Output
                    logDebugP("default: %s%i", debugOut.c_str(), value.ladcValue);
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
                debugOut = "Abs ";
#endif
            }
            // we always store the new value in KO, even it it is not sent (to satisfy potential read request)
            if (ParamADC_CHSMT50DPTType == 0)
            {
#ifdef InputADC_Output
                //   logDebugP("%sDPT5: %.3f", debugOut.c_str(), value.ladcValue);
#endif
                KoADC_ChannelOutput.valueNoSend((uint8_t)(value.ladcValue * 2.55), DPT_Percent_U8);
            }
            else
            {
#ifdef InputADC_Output
                logDebugP("%sDPT9: %.3f", debugOut.c_str(), value.ladcValue);
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

        /**********************************************************************************************************
         *              ADC 4-20mA                                                                                *
         *********************************************************************************************************/
        case ADC_4_20mA:

            // STEP 1: Get new Sensor value
            value.ladcValue = getPinInputCurrent();

            brokenCableDetection(value.ladcValue);

            // STEP 2: read Parameter DPT Format
            switch (ParamADC_CHSensortypStrom)
            {
                case SensorType_current: // DPT9.021 (mA)
                    // STEP 2a: Get Abs value
                    // we always store the new value in KO, even it it is not sent (to satisfy potential read request)
                    // logDebugP("val:     %f", value.ladcValue);
                    // logDebugP("val old: %f", valueOld.ladcValue);
                    KoADC_ChannelOutput.valueNoSend(value.ladcValue, DPT_Value_Curr);
                    lAbsolute = ParamADC_CHSendenAbsolut / 100.0; // Value in mA
                    break;

                case SensorType_percent:
                    percent = constrain((value.ladcValue - 4.0) * 6.25, 0.0, 100.0);
                    value.ladcValue = (uint8_t)percent;
                    KoADC_ChannelOutput.valueNoSend(value.ladcValue, Dpt(5, 1));
                    lAbsolute = ParamADC_CHSendenAbsolut;
                    break;

                default:
                    // STEP 2: Get new Sensor value (lineare Interpolation zwischen den Stützpunkten 4mA und 20mA)
                    value.ladcValue = ParamADC_CHValue4mA + (ParamADC_CHValue20mA - ParamADC_CHValue4mA) * (value.ladcValue - 4.0) / 16.0;
                    KoADC_ChannelOutput.valueNoSend(value.ladcValue, Dpt(9, 1));
                    if (ParamADC_CHSensortypStrom == SensorType_temperature || ParamADC_CHSensortypStrom == SensorType_flow)
                    {
                        lAbsolute = ParamADC_CHSendenAbsolut / 100.0;
                    }
                    else
                    {
                        lAbsolute = ParamADC_CHSendenAbsolut;
                    }
                    break;
            }

            // STEP 3a: Check if Change detected "Absolut"
            if (lAbsolute > 0 && (abs(value.ladcValue - valueOld.ladcValue)) >= lAbsolute)
            {
                lSend = true;
                logDebugP("Senden: abs");
            }

            // STEP 3b: Check if Change detected "Relativ"
            lAbsolute = ParamADC_CHSendenRelativ;
            if (lAbsolute > 0 && (abs(value.ladcValue - valueOld.ladcValue)) >= value.ladcValue / 100 * lAbsolute)
            {
                lSend = true;
                logDebugP("Senden: rel");
            }
            break;
        /**********************************************************************************************************
         *              ADC 4-20mA Fuellstand                                                                     *
         *********************************************************************************************************/
        case ADC_4_20mA_Fuellstand:

            // Measuremnet Range value
            meas_range = ParamADC_CHFuellstand_messbereich;

            // Berechnung mA
            value.ladcValue = getPinInputCurrent();
            brokenCableDetection(value.ladcValue);
            KoADC_ChannelOutput.valueNoSend(value.ladcValue, DPT_Value_Curr);

            // Berechnung Prozent (4mA = 0%, 20mA = 100%)
            percent = constrain((value.ladcValue - 4.0) * 6.25, 0.0, 100.0);
            value2 = (uint8_t)percent;
            // logDebugP("per: %f", percent);
            // logDebugP("val: %d", value2);
            KoADC_ChannelOutputA.valueNoSend(value2, Dpt(5, 1)); //****************************************************** */

            // Berechnung Füllhöhe in mm (meas_range in m, max 10m)
            value3 = (uint16_t)(percent / 100.0 * meas_range * 1000.0);
            KoADC_ChannelOutputB.valueNoSend(value3, Dpt(7, 11));

            // STEP 2a: Get Abs value
            lAbsolute = ParamADC_CHSendenAbsolut / 100.0; // Value in mA
            lAbsolute2 = ParamADC_CHSendenAbsolut2;       // Value in %
            lAbsolute3 = ParamADC_CHSendenAbsolut3;       // Value in mm

            // mA
            // STEP 3a: Check if Change detected
            if (lAbsolute > 0 && (abs(value.ladcValue - valueOld.ladcValue)) >= lAbsolute)
            {
                lSend = true;
                logDebugP("Send: ABS");
            }

            // STEP 3b: Check if Change detected
            lAbsolute = ParamADC_CHSendenRelativ;
            if (lAbsolute > 0 && (abs(value.ladcValue - valueOld.ladcValue)) >= value.ladcValue / 100 * lAbsolute)
            {
                lSend = true;
                logDebugP("Send: REL");
            }

            // Percent
            // STEP 3a: Check if Change detected
            if (lAbsolute2 > 0 && (abs(value2 - valueOld2)) >= lAbsolute2)
            {
                lSend2 = true;
                logDebugP("Send2: ABS");
            }

            // Water level
            // STEP 3a: Check if Change detected
            if (lAbsolute3 > 0 && (abs(value3 - valueOld3)) >= lAbsolute3)
            {
                lSend3 = true;
                logDebugP("Send3: ABS");
            }

            // STEP 3b: Check if Change detected
            lAbsolute3 = ParamADC_CHSendenRelativ2;
            if (lAbsolute3 > 0 && (abs(value3 - valueOld3)) > 0 && (abs(value3 - valueOld3)) >= value3 / 100.0 * lAbsolute3)
            {
                lSend3 = true;
                logDebugP("Send3: REL");
            }

            break;

        default:
#ifdef InputADC_Output
            v
#endif
                break;
    }

    // first run - send initial value once, instead of waiting for the next cycle/change
    if (_lastPeriodicSend == 0)
    {
        uint32_t now = millis();
        _lastPeriodicSend = now;
        _lastPeriodicSend2 = now;
        _lastPeriodicSend3 = now;

        sendState();
        if (ParamADC_CHSensorType == ADC_4_20mA_Fuellstand)
        {
            sendState2();
            sendState3();
        }

        valueOld.ladcValue = value.ladcValue;
        valueOld2 = value2;
        valueOld3 = value3;

        return;
    }

    if (delayCheck(_lastPeriodicSend, ParamADC_CHSendcycletime * 1000) && ParamADC_CHSendcycletime != 0)
    {
        _lastPeriodicSend = millis();
        logDebugP("send zyklisch");
        lSend = true;
    }
    if (delayCheck(_lastPeriodicSend2, ParamADC_CHSendcycletime2 * 1000) && ParamADC_CHSendcycletime2 != 0)
    {
        _lastPeriodicSend2 = millis();
        logDebugP("send2 zyklisch");
        lSend2 = true;
    }
    if (delayCheck(_lastPeriodicSend3, ParamADC_CHSendcycletime3 * 1000) && ParamADC_CHSendcycletime3 != 0)
    {
        _lastPeriodicSend3 = millis();
        logDebugP("send3 zyklisch");
        lSend3 = true;
    }

    if (lSend)
    {
        sendState();
        valueOld.ladcValue = value.ladcValue;
        lSend = false;
    }
    if (lSend2)
    {
        sendState2();
        valueOld2 = value2;
        lSend2 = false;
    }
    if (lSend3)
    {
        sendState3();
        valueOld3 = value3;
        lSend3 = false;
    }
}

void ADCInputChannel::getADC_value(long value)
{
    _adcValue = value;
    _hasFirstValue = true;

    // Hier muss senden bei Wertänderung drin sein !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void ADCInputChannel::brokenCableDetection(float value)
{
    if (value <= 2 && _cableBroken == false)
    {
        _cableBroken = true;
        KoADC_CableDetection.value(true, Dpt(1, 2));
    }
    else if (value > 3 && _cableBroken == true)
    {
        _cableBroken = false;
        KoADC_CableDetection.value(false, Dpt(1, 2));
    }
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
    logDebugP("sendState: %i, voltage: %.3f", _adcValue, getPinInputVoltage());
    KoADC_ChannelOutput.objectWritten();
}

void ADCInputChannel::sendState2()
{
    logDebugP("sendState2: %i, voltage: %.3f", _adcValue, getPinInputVoltage());
    KoADC_ChannelOutputA.objectWritten();
}

void ADCInputChannel::sendState3()
{
    logDebugP("sendStat3e: %i, voltage: %.3f", _adcValue, getPinInputVoltage());
    KoADC_ChannelOutputB.objectWritten();
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
    if (ParamADC_ADCHwVersion == HW42_0_12V)
    {
        return checkZero((_adcValue * 0.000375) / (ParamADC_CHVoltageCorrection / 100.0)); // 2.047 / 32767.0 * 6.0;  -> 6 ist der Fakter vom Spannungsteiler
    }
    else if (ParamADC_ADCHwVersion == HW42_0_8V)
    {
        return checkZero((_adcValue * 0.000249885555) / (ParamADC_CHVoltageCorrection / 100.0)); // 2.047 / 32767.0 * 4.0;  -> 4 ist der Fakter vom Spannungsteiler
    }
    else
    {
        return 0;
    }
}

// Voltage am PIN-INPUT in mV
float ADCInputChannel::getPinInputCurrent()
{
    return checkZero((_adcValue * (2.047 / 32767 * 10.0)) / (ParamADC_CHVoltageCorrection / 100.0)); // 2.047 / 32767.0 * 10.0;  -> 10.0 ist von 2V aud 20mA
}

float ADCInputChannel::calculateSensorValueLinearFunction(float a, float b)
{
    return (getPinInputVoltage() - b) / a;
}

float ADCInputChannel::calculateSensorValueLinearFunctionCur(float a, float b)
{
    return (getPinInputCurrent() - b) / a;
}

float ADCInputChannel::checkZero(float value)
{
    if (value > 0.07)
        return value;
    else
        return 0;
}