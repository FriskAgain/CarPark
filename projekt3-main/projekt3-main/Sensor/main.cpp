#include "Sensor.hpp"

int main() {


    int sensorInTrig = 23;
    int sensorInEcho = 24;

    int sensorOutTrig = 25;
    int sensorOutEcho = 8;

    int LEDInGreen = 4;
    int LEDInRed = 17;

    int LEDOutGreen = 5;
    int LEDOutRed = 6;

    Sensor sensors(sensorInTrig, sensorInEcho, sensorOutTrig, sensorOutEcho, LEDInGreen, LEDInRed, LEDOutGreen, LEDOutRed);

    sensors.sendTrigger(Sensor::trig_In);
    usleep(500); // lille pause
    long us = sensors.sensorRead(Sensor::echo_In);
    std::cout << "Raw echo pulse: " << us << " µs" << std::endl;

        double cm = sensors.measureSensor(Sensor::trig_In, Sensor::echo_In);
        //std::cout << "Distance: " << cm << " cm" << std::endl;
    std::thread t(&Sensor::sensorThread, &sensors);
    t.join();

    return 0;
}
