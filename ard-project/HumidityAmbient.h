#ifndef _HUMIDITY_AMBIENT_H
#define _HUMIDITY_AMBIENT_H

#include "DHT.h"

class HumidityAmbient {
private:
    int8_t humidity_max;
    int8_t humidity_min;
    int8_t lock;
    int8_t humidifier_sms;
    enum states {O, LOW_1, LOW_2, EMERGENCY};
    enum states STATE, NEXT_STATE;
public:
    HumidityAmbient();

    /**
     * @param humidity_max Highest accepted humidity
     * @param humidity_min Lowest accepted humidity
     */
    HumidityAmbient(int8_t humidity_max, int8_t humidity_min);

    //allowed getters and setters
    void setHumidityMax(int8_t humidity_max);
    void setHumidityMin(int8_t humidity_min);
    int8_t getState();
    int8_t getHumidityMax();
    int8_t getHumidityMin();
    int8_t getLock();
    int8_t getHumidifierSms();


    /**
     * Measures current temperature.
     * Switches states accordingly.
     *
     * @param dht Currently used DHT sensor
     * @return Measured humidity
     */
    int8_t readHumidity(DHT dht);

};

#endif
