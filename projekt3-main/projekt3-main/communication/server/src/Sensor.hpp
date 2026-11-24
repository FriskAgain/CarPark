#pragma once

#include <linux/gpio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

extern std::atomic<bool> Indgang;
extern std::atomic<bool> Udgang;



#define GPIO_CHIP_Path "/dev/gpiochip0"

class Sensor
{
private:
    struct gpiohandle_request req_in;
    struct gpiohandle_request req_out;
    struct gpiohandle_data data_in;
    struct gpiohandle_data data_out;

    int chip_fd = -1;

    double micros;

    int trig;

    int echo;


    bool sensorIn = false;
    bool sensorOut = false;
    
    int sensorInPinTrig_;
    int sensorInPinEcho_;

    int sensorOutPinTrig_;
    int sensorOutPinEcho_;

    int LEDInPinGreen_;
    int LEDInPinRed_;

    int LEDOutPinGreen_;
    int LEDOutPinRed_;

    public:
    Sensor(int sensorInPinTrig, int sensorInPinEcho, int sensorOutPinTrig, int sensorOutPinEcho, int LEDPInPinGreen, int LEDInPinRED, int LEDOutPinGreen, int LEDOutPinRed);
    ~Sensor();
    double measureSensor(int trig, int echo);
    void sendTrigger(int trig);
    long sensorRead(int echo);
    double microsecondsToCM(long microSecs);
    void sensorThread();
    bool GetSensorIn();
    bool GetSensorUd();

    enum outPins {
        trig_In = 0,
        trig_Out = 1,
        trig_LEDInGreen = 2,
        trig_LEDInRed = 3,
        trig_LEDOutGreen = 4,
        trig_LEDOutRed = 5,
    };

    enum inPins {
        echo_In = 0,
        echo_Out = 1,
    };
};




