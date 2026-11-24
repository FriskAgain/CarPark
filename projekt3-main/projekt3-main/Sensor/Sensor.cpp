#include "Sensor.hpp"

Sensor::Sensor(int sensorInPinTrig, int sensorInPinEcho, int sensorOutPinTrig, int sensorOutPinEcho, int LEDInPinGreen, int LEDInPinRed, int LEDOutPinGreen, int LEDOutPinRed): sensorInPinTrig_(sensorInPinTrig), sensorInPinEcho_(sensorInPinEcho), sensorOutPinTrig_(sensorOutPinTrig), sensorOutPinEcho_(sensorOutPinEcho), LEDInPinGreen_(LEDInPinGreen), LEDInPinRed_(LEDInPinRed), LEDOutPinGreen_(LEDOutPinGreen), LEDOutPinRed_(LEDOutPinRed){
    memset(&req_in, 0, sizeof(req_in));
    memset(&req_out, 0, sizeof(req_out));
    memset(&data_in, 0, sizeof(data_in));
    memset(&data_out, 0, sizeof(data_out));

    chip_fd = open(GPIO_CHIP_Path, O_RDWR);

    if(chip_fd < 0)
    {
        perror("open");
        return;
    }

    req_out.flags = GPIOHANDLE_REQUEST_OUTPUT;
    req_out.lines = 6;

    int outGPIO[6] = {sensorInPinTrig_, sensorOutPinTrig_, LEDInPinGreen_, LEDInPinRed_, LEDOutPinGreen_, LEDOutPinRed_};
    for(int i = 0; i < 6 ; i++)
    {
        req_out.lineoffsets[i] = outGPIO[i];
    }


    strcpy(req_out.consumer_label, "GPIO write");

    int ret_val_out = ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req_out);
    if(ret_val_out < 0)
    {
        perror("ioctl");
        close(chip_fd);
        chip_fd = -1;
        return;
    }

    req_in.flags = GPIOHANDLE_REQUEST_INPUT;
    req_in.lines = 2;

    int EchoGPIO[2] = {sensorInPinEcho_, sensorOutPinEcho_};
    for(int i = 0; i < 2 ; i++)
    {
        req_in.lineoffsets[i] = EchoGPIO[i];
    }


    strcpy(req_in.consumer_label, "GPIO read");

    int ret_val_in = ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req_in);
    if(ret_val_in < 0)
    {
        perror("ioctl");
        close(chip_fd);
        chip_fd = -1;
        return;
    }
    std::cout << "=== OUT pins (trig/LED) ===" << std::endl;
for (unsigned int i = 0; i < req_out.lines; i++) {
    std::cout << "out index " << i
              << " -> BCM " << req_out.lineoffsets[i]
              << std::endl;
}

std::cout << "=== IN pins (echo) ===" << std::endl;
for (unsigned int i = 0; i < req_in.lines; i++) {
    std::cout << "in index " << i
              << " -> BCM " << req_in.lineoffsets[i]
              << std::endl;
}
std::cout << std::flush;
}

void Sensor::sendTrigger(int trig){


    data_out.values[trig] = 0;
    ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
    usleep(2);

    data_out.values[trig] = 1;
    ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
    usleep(10);

    data_out.values[trig] = 0;
    ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
    }



Sensor::~Sensor(){

    if(req_in.fd >= 0){
        close(req_in.fd);
    }

    if(req_out.fd >=0){
        close(req_out.fd);
    }

    if(chip_fd >= 0){
        close(chip_fd);
    }
}

long Sensor::sensorRead(int echo){
    auto startWait = std::chrono::high_resolution_clock::now();

    while (true) {
        ioctl(req_in.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data_in);
            if (data_in.values[echo] == 1) break;


        auto now = std::chrono::high_resolution_clock::now();
        if ((now - startWait) > std::chrono::milliseconds(100))
            return 0;

        usleep(10);
    }

    auto start = std::chrono::high_resolution_clock::now();

    while (true) {
        ioctl(req_in.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data_in);

            if (data_in.values[echo] == 0) break;

        usleep(10);
    }

    auto end = std::chrono::high_resolution_clock::now();

    long microSecs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return microSecs;
}

double Sensor::microsecondsToCM(long microSecs){
    double microCM = microSecs;
    return microCM / 29 / 2;
}

double Sensor::measureSensor(int trig, int echo){
    sendTrigger(trig);
    usleep(500);
    long us = sensorRead(echo);
    return microsecondsToCM(us);
}

void Sensor::sensorThread(){
    while (true) {

        double cm1 = measureSensor(trig_In, echo_In);
        usleep(10);
        double cm2 = measureSensor(trig_Out, echo_Out);

        if (cm1 < 10){
            std::cout << "Car at entrance, distance: " << cm1 << std::endl;
            sensorIn = true;
                if (sensorIn == true && Indgang == true) {
            data_out.values[trig_LEDInGreen] = 1;
            ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);

            data_out.values[trig_LEDInRed] = 0;
            ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
                }
        } else {
            sensorIn = false;
            data_out.values[trig_LEDInGreen] = 0;
            ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);

            data_out.values[trig_LEDInRed] = 1;
            ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
        }

        if (cm2 < 10) {
            std::cout << "Car at exit, distance: " << cm2 << std::endl;
            sensorOut = true;
                if (sensorOut == true && Udgang == true) {
            data_out.values[trig_LEDOutGreen] = 1;
            ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);

            data_out.values[trig_LEDOutRed] = 0;
            ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
                }
        } else {
            sensorOut = false;
            data_out.values[trig_LEDOutGreen] = 0;
            ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);

            data_out.values[trig_LEDOutRed] = 1;
            ioctl(req_out.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data_out);
        }


        usleep(1000000); // svarer til delay(1000)
    }

    }
