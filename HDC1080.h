//  HDC1080.h

#ifndef _HDC1080__H_
#define _HDC1080__H_



#include <Arduino.h>


typedef byte Availability_t;
typedef byte Resolution_t;


class HDC1080 {
    public:
        static const byte MINIMUM_SAMPLING_PERIOD = 10;

        static const Availability_t AVAILABLE_NOTHING = 0;
        static const Availability_t AVAILABLE_TEMPERATURE = 1;
        static const Availability_t AVAILABLE_HUMIDITY = 2;
        static const Availability_t AVAILABLE_TEMPERATURE_AND_HUMIDITY = HDC1080::AVAILABLE_TEMPERATURE + HDC1080::AVAILABLE_HUMIDITY;

        static const Resolution_t RESOLUTION_14BIT = 0;
        static const Resolution_t RESOLUTION_11BIT = 1;
        static const Resolution_t RESOLUTION_8BIT  = 2;

        HDC1080();

        void begin();

        void setResolutions(Resolution_t tr, Resolution_t hr);

        Availability_t available();

        void heatUp(byte seconds);
        void startHeatingUp();
        void stopHeatingUp();
        bool isHeaterActive();

        float getTemperatureCelsius();
        float getTemperatureFahrenheit();
        float getHumidity();

        void update();

        void setSamplingPeriod(int period);
    
    private:
        byte _address;
        unsigned long long int _timer, _heatUpTimer, _configurationRegisterTimer;
        int _period, _heatUpPeriod;
        uint16_t _temperatureRaw, _humidityRaw;
        uint16_t _configurationRegister;
        byte _availability;
    

        static const byte TEMPERATURE_REGISTER_ADDR     = 0x00;
        static const byte HUMIDITY_REGISTER_ADDR        = 0x01;
        static const byte CONFIGURATION_REGISTER_ADDR   = 0x02;
        static const byte MANUFACTURER_ID_REGISTER_ADDR = 0xFE;
        static const byte DEVICE_ID_REGISTER_ADDR       = 0xFF;


        static const int CONFIGURATION_REGISTER_UPDATE_PERIOD = 1000;
};



#endif