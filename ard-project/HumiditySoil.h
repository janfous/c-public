#ifndef _HUMIDITY_SOIL_H
#define _HUMIDITY_SOIL_H

class HumiditySoil {
private:
    int8_t humidity_min;
    int8_t humidity_current;
    uint8_t lock;
    int8_t humidifier_sms;
    int8_t sensor_pin;
    enum states {O, LOW_1, LOW_2, EMERGENCY};
    enum states STATE, NEXT_STATE;
public:
    HumiditySoil();
    HumiditySoil(int8_t humidity_min, int8_t sensor_pin);

    //allowed getters and setters
    void setHumidityMin(int8_t humidity_min);
    int8_t getHumidityMin();
    int8_t getHumidityCurrent();
    uint8_t getLock();
    int8_t getHumidifierSms();
    int8_t getHumidifierPin();

    /**
     * Reads humidity from $sensor_pin
     * Switches states accordingly
     *
     * @return Measured humidity
     */
    int8_t readHumidity();

};

#endif
