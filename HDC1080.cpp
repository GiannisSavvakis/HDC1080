//  HDC1080.cpp


#include "HDC1080.h"
#include <Wire.h>



HDC1080::HDC1080() {
    _address = 0x40;
    _period = HDC1080::MINIMUM_SAMPLING_PERIOD;
    _availability = HDC1080::AVAILABLE_NOTHING;
    _configurationRegister = 0;
}


void HDC1080::begin() {
    Wire.begin();

    _timer = millis();
    _configurationRegisterTimer = millis();

    _configurationRegister |= (1 << 12);
    
    Wire.beginTransmission(_address);
        Wire.write(HDC1080::CONFIGURATION_REGISTER_ADDR);
        Wire.write((byte)(_configurationRegister >> 8));
        Wire.write((byte)_configurationRegister);
    Wire.endTransmission();

    Wire.beginTransmission(_address);
        Wire.write(HDC1080::TEMPERATURE_REGISTER_ADDR);
    Wire.endTransmission();
}


void HDC1080::setResolutions(Resolution_t tr, Resolution_t hr) {
    _configurationRegister &= ~(0b111 << 8);

    _configurationRegister |= (tr << 10);
    _configurationRegister |= (hr << 8);

    _configurationRegisterTimer = millis() - HDC1080::CONFIGURATION_REGISTER_UPDATE_PERIOD;
}



void HDC1080::heatUp(byte seconds) {
    _heatUpPeriod = 1000 * seconds;
    this->startHeatingUp();
}

void HDC1080::startHeatingUp() {
    _configurationRegister |= (1 << 13);
}

void HDC1080::stopHeatingUp() {
    _configurationRegister &= ~(1 << 13);
}

bool HDC1080::isHeaterActive() {
    return (_configurationRegister & (1 << 13)) >> 13;
}



Availability_t HDC1080::available() {
    return _availability;
}



float HDC1080::getTemperatureCelsius() {
    if(_availability == HDC1080::AVAILABLE_TEMPERATURE || _availability == HDC1080::AVAILABLE_TEMPERATURE_AND_HUMIDITY) {
        _availability -= HDC1080::AVAILABLE_TEMPERATURE;
    }

    return ((float)_temperatureRaw / (1<<16))*165 - 40;
}

float HDC1080::getTemperatureFahrenheit() {
    return (this->getTemperatureCelsius()*1.8 + 32);
}

float HDC1080::getHumidity() {
    if(_availability == HDC1080::AVAILABLE_HUMIDITY || _availability == HDC1080::AVAILABLE_TEMPERATURE_AND_HUMIDITY) {
        _availability -= HDC1080::AVAILABLE_HUMIDITY;
    }
    
    return ((float)_humidityRaw / (1<<16)) * 100;
}


void HDC1080::update() {
    if((_heatUpPeriod > 0) && (millis() - _heatUpTimer >= _heatUpPeriod)) {
        _heatUpPeriod = 0;

        this->stopHeatingUp();
    }

    if(millis() - _configurationRegisterTimer >= HDC1080::CONFIGURATION_REGISTER_UPDATE_PERIOD) {
        Wire.beginTransmission(_address);
            Wire.write(HDC1080::CONFIGURATION_REGISTER_ADDR);
            Wire.write((byte)(_configurationRegister >> 8));
            Wire.write((byte)_configurationRegister);
        Wire.endTransmission();

        Wire.beginTransmission(_address);
            Wire.write(HDC1080::TEMPERATURE_REGISTER_ADDR);
        Wire.endTransmission();

        _timer = millis();
        _configurationRegisterTimer = _timer;
    }

    if(millis()-_timer >= _period) {
        Wire.requestFrom(_address, (byte)4);

        _temperatureRaw = 0;
        _humidityRaw = 0;
		uint16_t temp = 0, hum = 0;

        _temperatureRaw |= (byte)Wire.read();
        _temperatureRaw <<= 8;
        _temperatureRaw |= (byte)Wire.read();

        _humidityRaw |= (byte)Wire.read();
        _humidityRaw <<= 8;
        _humidityRaw |= (byte)Wire.read();

        _availability = HDC1080::AVAILABLE_TEMPERATURE_AND_HUMIDITY;

        Wire.beginTransmission(_address);
            Wire.write(HDC1080::TEMPERATURE_REGISTER_ADDR);
        Wire.endTransmission();

        _timer = millis();
    }
}



void HDC1080::setSamplingPeriod(int period) {
    _period = period;

    if(_period < HDC1080::MINIMUM_SAMPLING_PERIOD) {
        _period = HDC1080::MINIMUM_SAMPLING_PERIOD;
    }
}
