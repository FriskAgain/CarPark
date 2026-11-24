#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <json_dto/pub.hpp>
#include <linux/gpio.h> // GPIO character device API
#include <sys/ioctl.h> // ioctl()
#include <fcntl.h> // open(), O_RDONLY
#include <unistd.h> // close()
#include <string.h> // memset()
#include <poll.h> // poll()
#include "Button.h"
#include <thread>
#include <chrono>
#include "Display.hpp"
#include "I2Cdriver.hpp"

#define myOLEDwidth 128
#define myOLEDheight 32
#define FULLSCREEN (myOLEDwidth * (myOLEDheight / 8))


// Callback for at gemme response
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// --- ID struct ---
// Brugt til at modtage forespørgsler med kun et ID fra klienten
struct ID_t{
    int id;

    ID_t() = default;
    ID_t(const int &ID) : id(ID) {}

    template <typename JSON_IO>
    void json_io(JSON_IO &io)
    {
        io & json_dto::mandatory("ID", id);
    }
};

struct response_t{
    int id;
    std::string message;

    template <typename JSON_IO>
    void json_io(JSON_IO &io){
        io & json_dto::mandatory("ID", id)
        & json_dto::mandatory("message",message);
    }
};

int main()
{
    I2CDriver i2cDriver;
    SSD1306 oled(myOLEDwidth, myOLEDheight, i2cDriver);
    uint8_t screenBuffer[FULLSCREEN];
    if (!oled.OLEDSetBufferPtr(myOLEDwidth, myOLEDheight, screenBuffer, sizeof(screenBuffer)))
     return -1;
    oled.OLEDbegin(0x3c); // initialize the OLED
    Display Display(oled);

    CURL* curlPark = curl_easy_init(); // Skal initialisere for at virke
    CURL* curlExit = curl_easy_init(); // Gøres for både UC2 og UC3

    // Henter ID og IP-addresse sat i config.txt
    std::ifstream config("config.txt");
    std::string line, ID_s, server_ip;
    if (config.is_open()) {
        while (std::getline(config, line)) {
            if (line.rfind("ID=", 0) == 0) {
                ID_s = line.substr(3);
            }
            if (line.rfind("ip=", 0) == 0) {
                server_ip = line.substr(3);
            }
        }
        config.close();
    }
    // Opretter ID struct for json senere, laver string om til int
    ID_t ID(std::stoi(ID_s));


    // Sætter IP-addressen til server
    const std::string urlPark = "http://" + server_ip + ":8080/park";
    const std::string urlExit = "http://" + server_ip + ":8080/exit";
    const std::string json_data = json_dto::to_json(ID);

    // Header der fortæller server hvad der bliver sendt
    // Server sender automatisk JSON tilbage
    std::string response_string;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Skrive "indstillingerne" til request
    // UC2
    curl_easy_setopt(curlPark, CURLOPT_URL, urlPark.c_str());
    curl_easy_setopt(curlPark, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curlPark, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curlPark, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curlPark, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curlPark, CURLOPT_CONNECTTIMEOUT, 5L);

    // UC3
    curl_easy_setopt(curlExit, CURLOPT_URL, urlExit.c_str());
    curl_easy_setopt(curlExit, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curlExit, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curlExit, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curlExit, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curlExit, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curlExit, CURLOPT_CONNECTTIMEOUT, 5L);

    std::cout << "JSON sendt: " << json_data << "\n";

    std::cout << "Sender til IP: " << server_ip << "... \n";

    // "Perform" UC2
    while(true){
        if(button() == 2){
            response_string.clear();
            CURLcode res = curl_easy_perform(curlPark);

            // Hvis det tager længere end 5 sekunder
            if (res == CURLE_OPERATION_TIMEDOUT) {
                std::cerr << "Connection timed out. \n";
            }
            
            // Hvis den aldrig når serveren
            if (res == CURLE_COULDNT_CONNECT){
                std::cerr << "Couldn't connect, check IP of server. \n";
            }
            
            // Forklarer hvad der (ellers) gik galt
            if (res != CURLE_OK) {
                std::cerr << "Curl error: " << curl_easy_strerror(res) << "\n";
            }

            // Hvis det bliver sendt (yay)
            else {
                long http_code = 0;
                curl_easy_getinfo(curlPark, CURLINFO_RESPONSE_CODE, &http_code);
                std::cout << "HTTP kode: " << http_code << "\n";
                std::cout << "Serverens svar: " << response_string << "\n";
            }
            //display.drawID(std::stoi(ID_s));
            Display.drawDisplay(response_string, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }

        // "Perform" UC3
        if(button() == 3){
            response_string.clear();
            CURLcode res = curl_easy_perform(curlExit);

            // Hvis det tager længere end 5 sekunder
            if (res == CURLE_OPERATION_TIMEDOUT) {
                std::cerr << "Connection timed out. \n";
            }
            
            // Hvis den aldrig når serveren
            if (res == CURLE_COULDNT_CONNECT){
                std::cerr << "Couldn't connect, check IP of server. \n";
            }
            
            // Forklarer hvad der (ellers) gik galt
            if (res != CURLE_OK) {
                std::cerr << "Curl error: " << curl_easy_strerror(res) << "\n";
            }

            // Hvis det bliver sendt (yay)
            else {
                long http_code = 0;
                curl_easy_getinfo(curlExit, CURLINFO_RESPONSE_CODE, &http_code);
                std::cout << "HTTP kode: " << http_code << "\n";
                std::cout << "Serverens svar: " << response_string << "\n";
            }
            //display.drawID(std::stoi(ID_s));
            Display.drawDisplay(response_string, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }

    }
    oled.OLEDPowerDown();
    //t1.join();
    curl_slist_free_all(headers);
    curl_easy_cleanup(curlPark);
    curl_easy_cleanup(curlExit);
    return 0;
}