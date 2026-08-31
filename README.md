# OFM-ADCInput

Implementation of a knx ADC-Input module with up to 4 channels based on the [knx stack](https://github.com/OpenKNX/knx), a fork from [thelsing](https://github.com/thelsing/knx).

This is a library for integrating the I2C AS1115 16-bit ADC.

The module is based on an RP2040 microcontroller. However, using an ESP32 should not be a problem.

The following features have been implemented:

Voltage measurement from 0–12 V
Current measurement from 4–20 mA

Common sensors (e.g. SMT50, TF-135, etc.) have already been implemented directly.

