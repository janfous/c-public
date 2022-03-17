#include "Arduino.h"
#include "HumiditySoil.h"

#define LOCK_0 60
#define LOCK_LOW_1 30
#define LOCK_LOW_2 30
#define LOCK_EMERGENCY 30

#define HS_0 0
#define HS_LOW 2
#define HS_EMERGENCY 1

#define HUMIDIFIER_VALUE 2
#define SMS_VALUE 1

#define MAX_MEASURED 1023.0

HumiditySoil::HumiditySoil() {
    HumiditySoil(50, 0);
}

HumiditySoil::HumiditySoil(int8_t humidity_min, int8_t sensor_pin) {
    this->humidity_min = humidity_min;
    this->lock = LOCK_0;
    this->humidifier_sms = HS_0;
    this->sensor_pin = sensor_pin;
}

void HumiditySoil::setHumidityMin(int8_t humidity_min) {
    this->humidity_min = humidity_min;
}

int8_t HumiditySoil::getHumidityMin() {
    return this->humidity_min;
}

int8_t HumiditySoil::getHumidityCurrent() {
    return this->humidity_current;
}

uint8_t HumiditySoil::getLock() {
    return this->lock;
}

int8_t HumiditySoil::getHumidifierSms() {
    return this->humidifier_sms;
}

int8_t HumiditySoil::getHumidifierPin() {
    return this->sensor_pin;
}

int8_t HumiditySoil::readHumidity() {
    float measured = 0;
    //measure 100x and calculate average
    for (int i = 0; i < 100; i++) {
        measured += analogRead(this->sensor_pin);
    }

    measured /= MAX_MEASURED;

    int8_t humidity = (int8_t) (100 - measured); //humidity = 100 - (avg of 100 measurements)

    switch (STATE) {
        case O:
            if (humidity < this->humidity_min) { //humidity low -> LOW_1
                NEXT_STATE = LOW_1;
                this->lock = LOCK_LOW_1;
                this->humidifier_sms = HS_LOW;
            } else { //humidity OK -> 0
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->humidifier_sms = HS_0;
            }
            break;
        case LOW_1:
            if (humidity < this->humidity_min) { //humidity low -> LOW_2
                NEXT_STATE = LOW_2;
                this->lock = LOCK_LOW_2;
                this->humidifier_sms = HS_LOW;
            } else { //humidity OK -> 0
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->humidifier_sms = HS_0;
            }
            break;
        case LOW_2:
            if (humidity < this->humidity_min) { //humidity low -> EMERGENCY
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->humidifier_sms = HS_LOW + HS_EMERGENCY;
            } else { //humidity OK -> 0
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->humidifier_sms = HS_0;
            }
            break;
        case EMERGENCY:
            if (humidity < this->humidity_min) { //humidity l0w -> stay in EMERGENCY
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->humidifier_sms = HS_LOW + HS_EMERGENCY;
            } else { //humidity OK -> 0
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->humidifier_sms = HS_0;
            }
            break;
    }

    return humidity;

}
