#include <Wire.h>
#include <Servo.h>
#include <DHT.h>
#include <EEPROM.h>

#include "Arduino.h"
#include "Temperature.h"
#include "HumidityAmbient.h"
#include "HumiditySoil.h"

//DHT sensor
#define DHT_PIN 27
#define DHT_TYPE DHT11

//humidity sensor count
#define HUMIDITY_SOIL_CNT 2

//temperature
#define TEMPERATURE_MAX 35
#define TEMPERATURE_MIN 20
#define MAIN_HEAT_PIN 8
#define BACKUP_HEAT_PIN 9

//ambient humidity
#define HUMIDITY_A_MAX 70
#define HUMIDITY_A_MIN 50

//soil humidity 0
#define HUMIDITY_S_MIN_0 60
#define SOIL_SENSOR_PIN_0 0
#define SERVO_PIN_0 6

//soil humidity 1
#define HUMIDITY_S_MIN_1 30
#define SOIL_SENSOR_PIN_1 1
#define SERVO_PIN_1 7

//humidifier pump
#define PUMP_PIN_1 2
#define PUMP_PIN_2 3
#define HUMIDIFIER_DURATION 10000

//soil pump
#define SOIL_PUMP_PIN_1 4
#define SOIL_PUMP_PIN_2 5
#define SOIL_PUMP_DURATION 15000

//sms
#define SMS_TEMPERATURE 4
#define SMS_HUMIDITY_AMBIENT 2
#define SMS_HUMIDITY_SOIL 1
#define SMS_TXD 10
#define SMS_RXD 11

//servo states
#define SERVO_OPEN 180
#define SERVO_CLOSED 0

//create necessary objects
DHT dht(DHT_PIN, DHT_TYPE);
Temperature temperature(TEMPERATURE_MAX, TEMPERATURE_MIN, MAIN_HEAT_PIN, BACKUP_HEAT_PIN);
HumidityAmbient humidity_ambient(HUMIDITY_A_MAX, HUMIDITY_A_MIN);
HumiditySoil humidity_soil_0(HUMIDITY_S_MIN_0, SOIL_SENSOR_PIN_0);
HumiditySoil humidity_soil_1(HUMIDITY_S_MIN_1, SOIL_SENSOR_PIN_1);
Servo servo_0;
Servo servo_1;

//humidity array setup
HumiditySoil humidity_soil_array[HUMIDITY_SOIL_CNT] = {humidity_soil_0, humidity_soil_1};
Servo servo_array[HUMIDITY_SOIL_CNT] = {servo_0, servo_1};

//temperature locks
uint8_t lock_temperature = temperature.getLock();
unsigned int temperature_last_millis = 0;
unsigned int temperature_current_millis = 0;

//ambient humidity locks
uint8_t lock_humidity_ambient = humidity_ambient.getLock();
unsigned int h_amb_last_millis = 0;
unsigned int h_amb_current_millis = 0;

//soil humidity locks
uint8_t lock_humidity_soil[HUMIDITY_SOIL_CNT] = {humidity_soil_0.getLock(), humidity_soil_1.getLock()};
unsigned long int h_soil_last_millis[HUMIDITY_SOIL_CNT] = {0, 0};
unsigned long int h_soil_current_millis[HUMIDITY_SOIL_CNT] = {0, 0};

//humidifier timer
int humidifier_millis_current = 0;
int humidifier_millis_last = 0;

//soil pump timer
int soil_pump_millis_current = 0;
int soil_pump_millis_last = 0;

//sms and pump flags
int8_t sms_status = 0;
bool run_humidifier = false;
bool run_humidifier_long = false;
bool run_soil_pump = false;

void setup() {

    //attach servos
    servo_0.attach(SERVO_PIN_0);
    servo_1.attach(SERVO_PIN_1);

    pinMode(MAIN_HEAT_PIN, OUTPUT);
    pinMode(BACKUP_HEAT_PIN, OUTPUT);

    pinMode(PUMP_PIN_1, OUTPUT);
    pinMode(PUMP_PIN_2, OUTPUT);

    pinMode(SOIL_PUMP_PIN_1, OUTPUT);
    pinMode(SOIL_PUMP_PIN_2, OUTPUT);

    dht.begin();

    Serial.begin(9600);
}

//void sendSms() {
//
//}

//run humidifier pump
void runHumidifier() {
    humidifier_millis_current = millis();

    int duration = HUMIDIFIER_DURATION;
    if (run_humidifier_long) duration *= 2;

    //turn on pump
    digitalWrite(PUMP_PIN_1, HIGH);
    digitalWrite(PUMP_PIN_2, LOW);

    //wait for $duration
    if (humidifier_millis_current - humidifier_millis_last > duration) {

        //turn off pump and reset flags
        digitalWrite(PUMP_PIN_1, LOW);
        digitalWrite(PUMP_PIN_2, LOW);

        run_humidifier = false;
        run_humidifier_long = false;
    }

}

void runSoilPump() {
    soil_pump_millis_current = millis();

    //turn on pump
    digitalWrite(SOIL_PUMP_PIN_1, HIGH);
    digitalWrite(SOIL_PUMP_PIN_2, LOW);

    //wait for duration
    if (soil_pump_millis_current - soil_pump_millis_last > SOIL_PUMP_DURATION) {

        //turn off pump and reset flag
        digitalWrite(SOIL_PUMP_PIN_1, LOW);
        digitalWrite(SOIL_PUMP_PIN_2, LOW);

        run_soil_pump = false;
    }

}

void readTemperature() {
    // lock handling
    temperature_current_millis = millis();

    unsigned int lock_millis = lock_temperature * 60 * 1000;

    if (temperature_current_millis - temperature_last_millis > lock_millis) {

        temperature_last_millis = millis();

        //read temperature
        int temp = temperature.readTemperature(dht);
        lock_temperature = temperature.getLock();
        uint8_t mbs = temperature.getMainBackupSms();

        //detect main, backup and sms states
        //stored in MBS as sum of Main(4) + Backup(2) + Sms(1)
        bool main = mbs & 4;
        bool backup = mbs & 2;
        bool sms = mbs & 1;

        //turn on/off heat sources
        digitalWrite(MAIN_HEAT_PIN, main);
        digitalWrite(BACKUP_HEAT_PIN, backup);
        if (sms) {
            sms_status += SMS_TEMPERATURE;
        }
    }
}

void readHAmbient() {
    // lock handling
    h_amb_current_millis = millis();

    unsigned int lock_millis = lock_humidity_ambient * 60 * 1000;

    if (h_amb_current_millis - h_amb_last_millis > lock_millis) {

        h_amb_last_millis = millis();

        //read humidity
        humidity_ambient.readHumidity(dht);
        lock_humidity_ambient = humidity_ambient.getLock();
        uint8_t hum_sms = humidity_ambient.getHumidifierSms();

        //detect main, backup and sms states
        //stored in hum_sms as Humidifier(2) + Sms(1)
        bool humidifier = hum_sms & 2;
        bool sms = hum_sms & 1;

        //if humidity too low, set flag for humidifier to turn on
        if (humidifier) {
            run_humidifier = true;
            run_humidifier_long = (humidity_ambient.getState() > 1); // if state LOW_2 || Emergency, set flag for longer humidifier run
        }

        if (sms) {
            sms_status += SMS_HUMIDITY_AMBIENT;
        }
    }
}

void readHSoil() {
    //iterate over soil sensor arrays
    for (int i = 0; i < HUMIDITY_SOIL_CNT; i++) {
        servo_array[i].write(SERVO_CLOSED);

        //lock handling
        h_soil_current_millis[i] = millis();

        unsigned long int lock_millis = lock_humidity_soil[i] * 60 * 1000;

        if (h_soil_current_millis[i] - h_soil_last_millis[i] > lock_millis) {

            h_soil_last_millis[i] = millis();

            //read humidity for each sensor
            humidity_soil_array[i].readHumidity();
            lock_humidity_soil[i] = humidity_soil_array[i].getLock();
            uint8_t hum_sms = humidity_soil_array[i].getHumidifierSms();

            bool humidifier = hum_sms & 2;
            bool sms = hum_sms & 1;

            if (humidifier) {
                //if humidity too low, open valve
                servo_array[i].write(SERVO_OPEN);
                run_soil_pump = true;
            }

            if (sms) {
                sms_status += SMS_HUMIDITY_SOIL;
            }
        }
    }
}

void loop() {
    sms_status = 0;

    readTemperature();  
    readHAmbient();
    readHSoil();
//
//    if (sms_status > 0) {
//        sendSms();
//    }
//
    if (run_humidifier) {
        runHumidifier();
    }

    if (run_soil_pump) {
        runSoilPump();
    }
}
