#include "Arduino.h"
#include "Temperature.h"

#define LOCK_0 1
#define LOCK_LOW_1 15
#define LOCK_LOW_2 10
#define LOCK_HIGH_1 15
#define LOCK_HIGH_2 10
#define LOCK_EMERGENCY 15

#define DEFAULT_MAIN 4
#define DEFAULT_BACKUP 5
#define DEFAULT_MIN 0
#define DEFAULT_MAX 100

#define OK_MBS 4
#define LOW_MBS 6
#define HIGH_MBS 0

#define MAIN_VALUE 4
#define BACKUP_VALUE 2
#define SMS_VALUE 1

Temperature::Temperature() {
    Temperature(DEFAULT_MAX, DEFAULT_MIN, DEFAULT_MAIN, DEFAULT_BACKUP);
}


Temperature::Temperature(int8_t temperature_max, int8_t temperature_min, int8_t main_pin, int8_t backup_pin) {
    if (temperature_min > temperature_max) {
        temperature_min = temperature_max;
    }

    this->temperature_max = temperature_max;
    this->temperature_min = temperature_min;
    this->temperature_last = 0;
    this->main_pin = main_pin;
    this->backup_pin = backup_pin;

    STATE = O;
    this->lock = 1;
    this->main_backup_sms = OK_MBS;
}

void Temperature::setTemperatureMax(int8_t temperature_max) {
    this->temperature_max = temperature_max;
}

void Temperature::setTemperatureMin(int8_t temperature_min) {
    this->temperature_min = temperature_min;
}

int8_t Temperature::getTemperatureMax() {
    return this->temperature_max;
}

int8_t Temperature::getTemperatureMin() {
    return this->temperature_min;
}

int8_t Temperature::getLock() {
    return this->lock;
}

int8_t Temperature::getMainBackupSms() {
    return this->main_backup_sms;
}

int8_t Temperature::readTemperature(DHT dht) {
    int8_t measured = dht.readTemperature();

    switch (STATE)  {
        case O: //state - OK
            if (measured < this->temperature_min) { //temperature too low -> LOW_1
                NEXT_STATE = LOW_1;
                this->lock = LOCK_LOW_1;
                this->main_backup_sms = LOW_MBS;
            } else if (measured > this->temperature_max) { //temperature too high -> HIGH_1
                NEXT_STATE = HIGH_1;
                this->lock = LOCK_HIGH_1;
                this->main_backup_sms = HIGH_MBS;
            } else { //temperature OK -> 0
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->main_backup_sms = OK_MBS;
            }
            break;
        case LOW_1: //state - LOW_1
            if (measured < this->temperature_min) { //temperature too low
                if (measured <= temperature_last) { //not rising -> LOW_2
                    NEXT_STATE = LOW_2;
                    this->lock = LOCK_LOW_2;
                    this->main_backup_sms = LOW_MBS;
                } else { //rising -> stay in LOW_1
                    NEXT_STATE = LOW_1;
                    this->lock = LOCK_LOW_1;
                    this->main_backup_sms = LOW_MBS;
                }
            } else if (measured > this->temperature_max) { //temperature too high -> HIGH_1
                NEXT_STATE = HIGH_1;
                this->lock = LOCK_HIGH_1;
                this->main_backup_sms = HIGH_MBS;
            } else { //temperature OK -> 0
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->main_backup_sms = OK_MBS;
            }
            break;
        case LOW_2: //state - LOW_2
            if (measured < this->temperature_min) { //temperature too low -> EMERGENCY
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->main_backup_sms = LOW_MBS + SMS_VALUE;
            } else if (measured > this->temperature_max) { //temperature too high -> HIGH_1
                NEXT_STATE = HIGH_1;
                this->lock = LOCK_HIGH_1;
                this->main_backup_sms = HIGH_MBS;
            } else { //temperature OK -> 0
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->main_backup_sms = OK_MBS;
            }
            break;
        case HIGH_1: //state - HIGH_1
            if (measured < this->temperature_min) { //temperature too low -> LOW_1
                NEXT_STATE = LOW_1;
                this->lock = LOCK_LOW_1;
                this->main_backup_sms = LOW_MBS;
            } else if (measured > this->temperature_max) { //temperature too high
                if (measured >= temperature_last) { //still rising -> HIGH_2
                    NEXT_STATE = HIGH_2;
                    this->lock = LOCK_HIGH_2;
                    this->main_backup_sms = HIGH_MBS;
                } else { //dropping -> stay in HIGH_1
                    NEXT_STATE = HIGH_1;
                    this->lock = LOCK_HIGH_1;
                    this->main_backup_sms = HIGH_MBS;
                }
            } else { //temperature OK -> 0
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->main_backup_sms = OK_MBS;
            }
            break;
        case HIGH_2:
            if (measured > this->temperature_max) {
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->main_backup_sms = HIGH_MBS + SMS_VALUE;
            } else if (measured < this->temperature_min) {
                NEXT_STATE = LOW_1;
                this->lock = LOCK_LOW_1;
                this->main_backup_sms = LOW_MBS;
            } else {
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->main_backup_sms = OK_MBS;
            }
            break;
        case EMERGENCY:
            if (measured > this->temperature_max) {
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->main_backup_sms = HIGH_MBS + SMS_VALUE;
            } else if (measured < temperature_min) {
                NEXT_STATE = EMERGENCY;
                this->lock = LOCK_EMERGENCY;
                this->main_backup_sms = LOW_MBS + SMS_VALUE;
            } else {
                NEXT_STATE = O;
                this->lock = LOCK_0;
                this->main_backup_sms = OK_MBS;
            }
            break;
    }

    STATE = NEXT_STATE;
    this->temperature_last = measured;

    return measured;
}
