#include <linux/gpio.h> // GPIO character device API
#include <sys/ioctl.h> // ioctl()
#include <fcntl.h> // open(), O_RDONLY
#include <unistd.h> // close()
#include <string.h> // memset()
#include <poll.h> // poll()
#include <iostream>
#include <unistd.h>
#include <string>
#include "Button.h"

// Define path to GPIO chip device
#define GPIO_CHIP_PATH "/dev/gpiochip0"

// Define GPIO lines
#define GPIO_BTN_A	27
#define GPIO_BTN_B	22

#define GPIO_BTN_UP	12
#define GPIO_BTN_DOWN	16
#define GPIO_BTN_LEFT	5
#define GPIO_BTN_RIGHT	6

void test()
{
    while(1)
    {
        std::cout << "test" << std::endl;
        sleep(0.5);
    }
}


void sendind(bool b, std::string s)
{
    //SEND BOOL AND STRING TO SERVER
}

void sendud(bool b, std::string s)
{
    //SEND BOOL AND STRING TO SERVER
}

int button() {
static bool lastParkButton = false; // Skal gøres static ELLERS virker det ikke fordi "client.cpp" 
static bool lastExitButton = false; // kalder button() igen og igen der resetter det.
    while(1)
    {
        // Åbner file descriptor, bør nok sættes udenfor while-loop for optimering
        int chip_fd = open(GPIO_CHIP_PATH, O_RDONLY);
    
        if (chip_fd < 0) {
            perror("open");
            return EXIT_FAILURE;
        }
    
        // Request structure to get handle to GPIO line
        struct gpiohandle_request req;
    
        // Polulate structure to be safe
        memset(&req, 0, sizeof(req));
    
        // Specify GPIO lines we want to access
        req.lineoffsets[0] = GPIO_BTN_A;
        req.lineoffsets[1] = GPIO_BTN_B;

        // Access all lines as inputs
        req.flags = GPIOHANDLE_REQUEST_INPUT;
        // Number of lines we want to access
        req.lines = 2;
        // Set a name to be shown when gpioinfo is called
        strcpy(req.consumer_label, "GPIO read example");
    
        // Use ioctl to get GPIO line handle from kernel
        int ret_val = ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
        if (ret_val < 0) {
            perror("ioctl");
            close(chip_fd);
            return EXIT_FAILURE;
        }
    
        // Structure to store values read from GPIO lines
        struct gpiohandle_data data;
    
        // Use ioctl to get GPIO line handle from kernel
        ret_val = ioctl(req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
        if (ret_val < 0) {
            perror("ioctl");
            close(req.fd);
            close(chip_fd);
            return EXIT_FAILURE;
        }

        bool parkButton = data.values[0] == 0;
        bool exitButton = data.values[1] == 0;
    
        if(parkButton && !lastParkButton)
        {
            lastParkButton = true;
            lastExitButton = exitButton;
            close(req.fd);
            close(chip_fd);
            return 2;
        }

        if(exitButton && !lastExitButton)
        {
            lastExitButton = true;
            lastParkButton = parkButton;
            close(req.fd);
            close(chip_fd);
            return 3;
        }

        lastParkButton = parkButton;
        lastExitButton = exitButton;

        close(req.fd);
        close(chip_fd);
        sleep(0.01);

    }

	return EXIT_SUCCESS;
}
