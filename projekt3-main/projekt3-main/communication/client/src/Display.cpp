#include "Display.hpp"
#include <iostream>
#include <string.h>

Display::Display(SSD1306 &oledRef) : oled(oledRef){}


void Display::drawDisplay(std::string s, int i) {
    oled.OLEDclearBuffer();

    
    if(i == 1)
    {
    oled.OLEDclearBuffer();
    oled.setTextColor(WHITE);
    oled.setTextSize(2);
    oled.setCursor(0, 0);
    oled.println(s);
    // Send buffer to OLED
    oled.OLEDupdate();
    }

    else if(i == 0)
    {
        // Clear display buffer first
        oled.OLEDclearBuffer();
        oled.setTextColor(WHITE);
        oled.setTextSize(2);
        oled.setCursor(0, 0);
        if(s[19] == 78)  //N
        {
        oled.println("No");
        oled.print("Spots");
        oled.OLEDupdate();
        }
        else if(s[19]== 73)   //I
        {
        oled.println("ID");
        oled.print("not found");
        oled.OLEDupdate();
        }
        else if(s[19]== 89) //Y   
        {
        oled.println("Already");
        oled.print("parked");
        oled.OLEDupdate();
        }
        else if(s[19]== 80) //P   
        {
        oled.println("Go to");
        oled.print("entrance");
        oled.OLEDupdate();
        }
        else if(s[19]== 84) //T   
        {
        oled.println("Bye bye!");
        oled.print("See ya!");
        oled.OLEDupdate();
        }
        else if(s[19]== 86) //V   
        {
        oled.println("Park");
        oled.print("first!!!");
        oled.OLEDupdate();
        }
        else if(s[19]== 68) //D   
        {
        oled.println("Go to");
        oled.print("exit");
        oled.OLEDupdate();
        }
        else
        {
        char id = s[6];
        char spot = s[46];
        int id_ = id - '0';
        int spot_ = spot - '0';
        //std::cout << s << std::endl;
        // Convert safely
        oled.print("ID: ");
        oled.println(id_);
       // std::string b = spot_;
        oled.print("Spot:");
        oled.print(spot_);
        // Send buffer to OLED
        oled.OLEDupdate();
        } 
    
        
    }
    
}
