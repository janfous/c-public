#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H

#include "DHT.h"

/**
 * @author fousjan1
 * @version 0.1.0
 * @since 0.1.0
 */
class Temperature {
private:

    int8_t temperature_max;
    int8_t temperature_min;
    int8_t temperature_last;
    int8_t lock;
    int8_t main_pin;
    int8_t backup_pin;
    int8_t main_backup_sms;
    enum states {O, LOW_1, LOW_2, HIGH_1, HIGH_2, EMERGENCY};
    enum states STATE, NEXT_STATE;

public:
    Temperature();

    /**
     * @param temperature_max Max. viable temperature
     * @param temperature_min Min. viable temperature
     * @param main_pin Main heat source pin
     * @param backup_pin Backup heat source pin
     */
    Temperature(int8_t temperature_max, int8_t temperature_min, int8_t main_pin, int8_t backup_pin);

    //allowed getters and setters
    /**
     * @param temperature_max Max. viable temperature
     */
    void setTemperatureMax(int8_t temperature_max);

    /**
     * @param temperature_min Min. viable temperature
     */
    void setTemperatureMin(int8_t temperature_min);

    /**
     * @return Max. temperature
     */
    int8_t getTemperatureMax();

    /**
     * @return Min. temperature
     */
    int8_t getTemperatureMin();

    /**
     * @return Lock duration
     */
    int8_t getLock();

    /**
     * @return Sum of Main(4) + Backup(2) + Sms (1)
     */
    int8_t getMainBackupSms();

    /**
     * Measures current temperature.
     * Switches states accordingly.
     *
     * @param dht Currently used DHT sensor
     * @return Measured temperature
     */
    int8_t readTemperature(DHT dht);

};

#endif
