#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <json_dto/pub.hpp>

// Callback for at gemme response
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

struct licensePlate_t{
    std::string licensePlate;
    std::string model;
    bool handicap;
    bool electric;

    licensePlate_t() = default;

    licensePlate_t(const std::string &plate) : licensePlate(plate) {}

    template <typename JSON_IO>
    void json_io(JSON_IO &io)
    {
        io & json_dto::mandatory("LicensePlate", licensePlate);
        io & json_dto::optional("Model", model, "");
        io & json_dto::optional("Handicap", handicap, false);
        io & json_dto::optional("Electric", electric, false);
    }
};

struct response_t{
    int id;
    std::string message;

    template <typename JSON_IO>
    void json_io(JSON_IO &io){
        io & json_dto::mandatory("id", id)
        & json_dto::mandatory("message",message);
    }
};

int main()
{
    CURL* curl = curl_easy_init(); // Skal initialisere for at virke

    std::string licensePlate;
    std::cout << "Please enter License Plate number: \n";
    std::cin >> licensePlate;
    licensePlate_t LP(licensePlate);
    response_t resp;

    // Henter IP-addressen sat i config.txt
    std::ifstream config("config.txt");
    std::string line, server_ip;
    if (config.is_open()){
        while(std::getline(config, line)){
            if(line.find("ip=") == 0){
                server_ip = line.substr(3);
                break;
            }
        }
        config.close();
    }

    // Sætter IP-addressen til server
    const std::string url = "http://" + server_ip + ":8080/register";
    const std::string json_data = json_dto::to_json(LP);

    // Header der fortæller server hvad der bliver sendt
    // Server sender automatisk JSON tilbage
    std::string response_string;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Skrive "indstillingerne" til request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

    std::cout << "Sender til IP: " << server_ip << "... \n";

    // Store røde knap der "sender" til server
    CURLcode res = curl_easy_perform(curl);

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
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        std::cout << "HTTP kode: " << http_code << "\n";
        std::cout << "Serverens svar: " << response_string << "\n";
    }

    // Gemmer ID:
    resp = json_dto::from_json<response_t>(response_string);
    std::ifstream infile("config.txt");
    std::ofstream outfile("config_tmp.txt");

    if (infile.is_open() && outfile.is_open()) {
        while (std::getline(infile, line)) {
            if (line.find("ID=") == 0) {
                outfile << "ID=" << resp.id << "\n";  // Indskriv ID
            } else {
                outfile << line << "\n";  // Kopier alle linjer
            }
        }
        infile.close();
        outfile.close();
        std::rename("config_tmp.txt", "config.txt");  // Erstat gamle config fil!
    }  

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return 0;
} //...psst, brug "g++ UC1_Register.cpp -o UC1.exe -lcurl -I../external/json_dto" til at compile