#include "Arduino.h"
#include "HumidityAmbient.h"

#define LOCK_0 5
#define LOCK_LOW_1 15
#define LOCK_LOW_2 15
#define LOCK_EMERGENCY 15

#define HS_0 0
#define HS_LOW 2
#define HS_HIGH 0
#define HS_EMERGENCY 1

#define HUMIDIFIER_VALUE 2
#define SMS_VALUE 1

/**
 * Default constructor
 */
HumidityAmbient::HumidityAmbient() {
    HumidityAmbient(100, 0);
}

/**
 * @param humidity_max
 * @param humidity_min
 */
HumidityAmbient::HumidityAmbient(int8_t humidity_max, int8_t humidity_min) {
    if (humidity_min > humidity_max) {
        humidity_min = humidity_max;
    }

    this->humidity_max = humidity_max;
    this->humidity_min = humidity_min;
    this->lock = LOCK_0;
    this->humidifier_sms = HS_0;
}

/**
 * @param humidity_max
 */
void HumidityAmbient::setHumidityMax(int8_t humidity_max) {
    this->humidity_max = humidity_max;
}

/**
 * @param humidity_min
 */
void HumidityAmbient::setHumidityMin(int8_t humidity_min) {
    this->humidity_min = humidity_min;
}

int8_t HumidityAmbient::getState() {
    return this->STATE;
}

int8_t HumidityAmbient::getHumidityMax() {
    return this->humidity_max;
}

int8_t HumidityAmbient::getHumidityMin() {
    return this->humidity_min;
}

int8_t HumidityAmbient::getLock() {
    return this->lock;
}

int8_t HumidityAmbient::getHumidifierSms() {
    return this->humidifier_sms;
}


int8_t HumidityAmbient::readHumidity(DHT dht) {
    int8_t measured = dht.readHumidity();

    switch (STATE) {
        case O:
            if (measured < this->humidity_min) { //low humidity 1 -> humidifier TRUE, sms FALSE
                NEXT_STATE = LOW_1;
                this->lock = LOCK_LOW_1;
                this->humidifier_sms = HS_LOW;
            } else if (measured > this->humidity_max) { //high humidity E -> humidifier FALSE, sms TRUE
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->humidifier_sms = HS_HIGH + HS_EMERGENCY;
            } else {
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->humidifier_sms = HS_0;
            }
            break;
        case LOW_1:
            if (measured < this->humidity_min) { //low humidity 2 -> humidifier TRUE, sms FALSE
                NEXT_STATE = LOW_2;
                this->lock = LOCK_LOW_2;
                this->humidifier_sms = HS_LOW;
            } else { //humidity OK
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->humidifier_sms = HS_0;
            }
            break;
        case LOW_2:
            if (measured < this->humidity_min) { //low humidity E -> humidifier TRUE, sms TRUE
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->humidifier_sms = HS_LOW + HS_EMERGENCY;
            } else { //humidity OK
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->humidifier_sms = HS_0;
            }
            break;
        case EMERGENCY:
            if (measured > this->humidity_max) { //high humidity E -> humidifier FALSE, sms TRUE
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->humidifier_sms = HS_HIGH + HS_EMERGENCY;
            } else if (measured < humidity_min) { //low humidity E -> humidifier TRUE, sms TRUE
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->humidifier_sms = HS_LOW + HS_EMERGENCY;
            } else { //humidity OK
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->humidifier_sms = HS_0;
            }
            break;
    }

    STATE = NEXT_STATE;

    return measured;
}
