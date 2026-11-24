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
        io & json_dto::mandatory("id", id)
        & json_dto::mandatory("message",message);
    }
};

int main()
{
    CURL* curl = curl_easy_init(); // Skal initialisere for at virke

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

    ID_t ID(std::stoi(ID_s));
    response_t resp;

    // Sætter IP-addressen til server
    const std::string url = "http://" + server_ip + ":8080/exit";
    const std::string json_data = json_dto::to_json(ID);

    // Header der fortæller server hvad der bliver sendt
    // Server sender automatisk JSON tilbage
    std::string response_string;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Skrive "indstillingerne" til request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
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

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return 0;
} //...psst, brug "g++ UC3_Exit.cpp -o UC3.exe -lcurl -I../external/json_dto" til at compile