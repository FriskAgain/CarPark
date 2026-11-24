#pragma once
#include "SSD1306_OLED.hpp"
#include "I2Cdriver.hpp"
#include <string>
#include <vector>

class Display {
public:
    Display(SSD1306 &oledRef);
    void drawDisplay(std::string s, int i);
    void displayThread();
 
private:
    SSD1306 &oled;
};
