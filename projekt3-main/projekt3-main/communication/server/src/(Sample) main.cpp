#include <restinio/all.hpp>
#include <json_dto/pub.hpp>
#include <iostream>
#include <fstream>
#include <mutex>
#include <vector>
#include <string>
#include <atomic>
#include <thread>
#include "json.hpp"
#include "pKompleks.hpp"
#include "Sensor.hpp"
using namespace std;
using json = nlohmann::json;

std::atomic<bool> Indgang{false};
std::atomic<bool> Udgang{false};

bool sensorInd2;
bool sensorud2;

    int sensorInTrig = 23;
    int sensorInEcho = 24;

    int sensorOutTrig = 25;
    int sensorOutEcho = 8;

    int LEDInGreen = 4;
    int LEDInRed = 17;

    int LEDOutGreen = 5;
    int LEDOutRed = 6;

int Check(string plate, int &id)
{
    ifstream Database("/home/au763527/projekt3/communication/server/src/API.json");              //Read API file
    ifstream Registered("/home/au763527/projekt3/communication/server/src/Registered.json");     //Read Registered file
    json oldCars;                               //Make oldCars a json
    json cars;                                  //Make cars a json
    Registered >> oldCars;                      //Oldcars are in Registered
    Database >> cars;                           //cars are in Database
    id = 0;
    for (auto& item : oldCars.items())          //Loop through all items(Cars)
    {
        int currentID = item.value()["ID"];     //Check ID of car

        if (currentID > id)                     //Compare ID
        {
            id = currentID;                     //Update ID
        }
    }

    if(oldCars.contains(plate))             //If already registered in database
    {
        cout << "This car is already registered" << endl;
        return 0;
    }
    else if(cars.contains(plate))
    {
        id = id+1;                          //Inkrementer ID
        oldCars[plate] = {                  //Kopier data fra API filen til Registered filen
            {"model", cars[plate]["model"]},
            {"handicap", cars[plate]["handicap"]},
            {"EV", cars[plate]["electric"]},
            {"ID", id}
        }; 
        cout << "New car registered: " << plate << "\n" <<  oldCars[plate].dump(2) << endl; //Print
        ofstream out("/home/au763527/projekt3/communication/server/src/Registered.json");    //Write it into Database file
        out << oldCars.dump(2);
        out.close();                        //Close file
        return 1;

    }
    else{cout << "Invalid licensplate" << endl;}    //If it does not exist anywhere
    return -1;
}

int Park(int id, pKompleks& parkering) {

    // Åbner JSON filer
    ifstream Registered ("/home/au763527/projekt3/communication/server/src/Registered.json");   // Read Registered.json
    ifstream Parked ("/home/au763527/projekt3/communication/server/src/Parked.json");           // Read Parked.json

    if (!Registered.is_open()) {
    cout << "Failed to open Registered.json!" << endl;
    return -1;
    }

    if (sensorInd2 == false) {
        return -6;
    }

    // Laver JSON objekter og fylder dem med data fra filerne
    json RegisteredCars;
    json ParkedCars;
    Registered >> RegisteredCars;
    Parked >> ParkedCars;

    // Tjekker om bilen allerede er parkeret
    for(auto& item : ParkedCars.items()) {
        if (item.value()["ID"] == id) {
            cout << "The car is already parked" << std::endl;
            return 0;
        }
    }

    // Finder bilen i Registered.json og kopiere dataen
    std::string plate = "";
    json carData;

    for (auto& item : RegisteredCars.items()) {
        if (item.value()["ID"] == id) {
            plate = item.key();
            carData = item.value();
            break;
        }
    }

    if (plate == "") {
        cout << "Car with ID " << id << " is NOT registered" << endl;
        return -2;
    }
// 3. Find en parkeringsplads
    int spot;

    if(carData["handicap"] == true) {
        if (parkering.get_handy().empty()) {
            cout << "No handicap spots available!" << endl;
            spot = parkering.pop_normal();
        }
        else {
            spot = parkering.pop_handy();
        }
    } 
    else if(carData["EV"] == true) {
        if (parkering.get_el().empty()) {
            cout << "No EV spots available!" << endl;
            spot = parkering.pop_normal();
        }
        else {
            spot = parkering.pop_el();
        }
    } 
    else {
        spot = parkering.pop_normal();
    }
    
    if (spot == -1) {
        cout << "No parking spots available!" << endl;
        return -1;
    }

    // 4. Tilføj data til Parked.json
    ParkedCars[plate] = {
        {"EV", carData["EV"]},
        {"ID", id},
        {"handicap", carData["handicap"]},
        {"model", carData["model"]},
        {"spot", spot}
    };

    // 5. Udskriv til filen
    ofstream out("/home/au763527/projekt3/communication/server/src/Parked.json");
    out << ParkedCars.dump(2);
    out.close();

    cout << "Car parked: " << plate << " at spot " << spot << endl;
    return spot;

}

int Exit(int id, pKompleks& parkering) {
    
    // Åbner JSON fil
    ifstream Parked ("/home/au763527/projekt3/communication/server/src/Parked.json");           // Read Parked.json

    // Laver JSON objekt og fylder den med data fra filen
    json ParkedCars;
    Parked >> ParkedCars;

    if (sensorud2 == false) {
        return -6;
    }

    // Tjekker om bilen er der
    string plate = "";
    for (auto& item : ParkedCars.items()) {
        if (item.value()["ID"] == id) {
            plate = item.key();
            break;
        }
    }

    // Hvis bilen ikke er parkeret
    if (plate == "") {
        cout << "Car with ID " << id << " is NOT parked" << endl;
        return -1;
    }

    // Test
    int spot = ParkedCars[plate]["spot"];
    bool isEV = ParkedCars[plate]["EV"];
    bool isHandy = ParkedCars[plate]["handicap"];

    // Return the spot to the appropriate queue
    if (isHandy)
        parkering.push_handy(spot);
    else if (isEV)
        parkering.push_el(spot);
    else
        parkering.push_normal(spot);

    // Remove the car
    ParkedCars.erase(plate);

    // Save updated file
    ofstream out("/home/au763527/projekt3/communication/server/src/Parked.json");
    out << ParkedCars.dump(2);

    cout << "Car with ID " << id << " has exited the parking lot." << endl;
    return 1;    

}

// --- Vehicle struct ---
struct vehicle_t
{
    vehicle_t() = default;

    vehicle_t(std::string licensePlate,
              std::string model,
              bool handicap = false,
              bool electric = false)
        : m_licensePlate{std::move(licensePlate)},
          m_model{std::move(model)},
          m_handicap{handicap},
          m_electric{electric}
    {}

    template <typename JSON_IO>
    void json_io(JSON_IO &io)
    {
        io & json_dto::mandatory("LicensePlate", m_licensePlate)
           & json_dto::optional("Model", m_model, "")
           & json_dto::optional("Handicap", m_handicap, false)
           & json_dto::optional("Electric", m_electric, false);
    }

    std::string m_licensePlate;
    std::string m_model;
    bool m_handicap{false};
    bool m_electric{false};
};

// --- License Plate struct ---
// Brugt til at modtage forespørgsler med kun en nummerplade fra klienten
struct licensePlate_t{
    std::string licensePlate;

    licensePlate_t() = default;
    licensePlate_t(const std::string &plate) : licensePlate(plate) {}

    template <typename JSON_IO>
    void json_io(JSON_IO &io)
    {
        io & json_dto::mandatory("LicensePlate", licensePlate);
    }
};

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


// --- JSON response struct ---
// Skabelon for svar tilbage til klienten
struct response_t
{
    int id;
    std::string message;

    template <typename JSON_IO>
    void json_io(JSON_IO &io)
    {
        io & json_dto::mandatory("id", id)
           & json_dto::mandatory("message", message);
    }
};

// --- In-memory vehicle collection ---
using parking_request_t = std::vector<vehicle_t>;

// --- Vehicle HTTP handler ---
class vehicle_handler_t
{
public:
    explicit vehicle_handler_t(parking_request_t &vehicles)
        : m_vehicles(vehicles)
    {}

    auto on_vehicle_add(const restinio::request_handle_t &req,
                        restinio::router::route_params_t)
    {
        auto resp = init_resp(req->create_response());

        try
        {
            auto vehicle = json_dto::from_json<vehicle_t>(req->body());
            m_vehicles.push_back(vehicle);
            int id = add_vehicle_to_file(vehicle);

            response_t r{id, "Vehicle registered successfully"};

            resp.set_body(json_dto::to_json(r))
                .append_header("Content-Type", "application/json; charset=utf-8");
        }
        catch (const std::exception &ex)
        {
            mark_as_bad_request(resp);
            resp.set_body(std::string{"Error: "} + ex.what());
        }

        return resp.done();
    }



private:
    parking_request_t &m_vehicles;
    mutable std::mutex m_file_mutex;
    int m_next_id{1};

    int add_vehicle_to_file(const vehicle_t &veh)
    {
        std::lock_guard<std::mutex> lock(m_file_mutex);
        std::ofstream file("vehicles.txt", std::ios::app);
        if (!file.is_open())
            throw std::runtime_error("Unable to open vehicles.txt");

        int id = m_next_id++;
        file << "________________ \n" << "ID: " << id << "\n" << "Licenseplate: " << veh.m_licensePlate << "\n" << "Car model: " << veh.m_model
             << "\n" << "Handicap Status: " << veh.m_handicap << "\n" << "EV: " << veh.m_electric << "\n" << "________________ \n";
        return id;
    }

    template <typename RESP>
    static RESP init_resp(RESP resp)
    {
        resp.append_header("Server", "RESTinio Vehicle Server /v1.0")
            .append_header_date_field()
            .append_header("Content-Type", "application/json; charset=utf-8");
        return resp;
    }

    static void mark_as_bad_request(auto &resp)
    {
        resp.header().status_line(restinio::status_bad_request());
    }
};

// --- License Plate HTTP handler ---
// Håndterer forespørgsler der kun indeholder en nummerplade
class licensePlate_handler_t
{
public:
    // Use Case 1: Opretning og tjek af biler
    // Hovedlogik for håndtering af forespørgsler med nummerplade vedrørende opretning og tjek af biler
    auto on_register_vehicle(const restinio::request_handle_t &req,
                        restinio::router::route_params_t)
    {
        auto resp = init_resp(req->create_response());

        try
        {
            // Henter nummerpladen fra forespørgslen og tjekker den op imod API og Registered filer
            auto plate = json_dto::from_json<licensePlate_t>(req->body());
            string plateToCheck = plate.licensePlate;
            std::cout << "Body = " << req->body() << "" << std::endl;
            cout << plate.licensePlate << endl;
            int id = 0;
            int checkID = Check(plateToCheck, id);

            // Responderer tilbage til klienten baseret på resultatet af tjekket
            if(checkID == 0)
            {
                response_t t = {0, "Vehicle found, already registered"};    
                resp.set_body(json_dto::to_json(t))
                    .append_header("Content-Type", "application/json; charset=utf-8");
            }

            else if(checkID == 1)
            {
                response_t t = {id, "Vehicle Registered succesfully"};    
                resp.set_body(json_dto::to_json(t))
                    .append_header("Content-Type", "application/json; charset=utf-8");
            }
            else
            {
                response_t t = {0, "Vehicle not found!"};  
                resp.set_body(json_dto::to_json(t))
                    .append_header("Content-Type", "application/json; charset=utf-8");  
            }
            
        }

        // Håndtering af fejl under processen
        catch (const std::exception &ex)
        {
            mark_as_bad_request(resp);
            resp.set_body(std::string{"Error: "} + ex.what());
        }

        return resp.done();
    }

private:

    // Respons logik
    template <typename RESP>
    static RESP init_resp(RESP resp)
    {
        resp.append_header("Server", "RESTinio Vehicle Server /v1.0")
            .append_header_date_field()
            .append_header("Content-Type", "application/json; charset=utf-8");
        return resp;
    }

    // Fejl håndtering
    static void mark_as_bad_request(auto &resp)
    {
        resp.header().status_line(restinio::status_bad_request());
    }
};

class ID_handler_t
{
public:
    // Test
    ID_handler_t(pKompleks& p) : parkering(p) {}
    // Use Case 2: Parkering
    // Hovedlogik for håndtering af forespørgsler med ID vedrørende parkering af biler
    auto on_parking_request(const restinio::request_handle_t &req,
                        restinio::router::route_params_t)
    {
        auto resp = init_resp(req->create_response());

        try
        {
            if( sensorInd2 == true)
            {
                Indgang = true;
                // Henter ID'et fra forespørgslen
                auto id_request = json_dto::from_json<ID_t>(req->body());
                int idToCheck = id_request.id;
                std::cout << "Body = " << req->body() << "" << std::endl;
                cout << id_request.id << endl;
    
                // Her kan du tilføje logik til at håndtere parkering baseret på ID'et
    
                int spot = Park(idToCheck, parkering);
    
                if(spot == -1) {
                    response_t t = {idToCheck, "No parking spots are available"};    
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");
                }
                else if (spot == -2) {
                    response_t t = {idToCheck, "ID is not registered"};    
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");
                }
                else if (spot == 0) {
                    response_t t = {idToCheck, "car is already parked"};    
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");
                }
                else {
                    response_t t = {idToCheck, std::string("Car is now parked at spot: ") + std::to_string(spot)};    
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");
                }
            }
            else {
                // Henter ID'et fra forespørgslen
                auto id_request = json_dto::from_json<ID_t>(req->body());
                int idToCheck = id_request.id;
                std::cout << "Body = " << req->body() << "" << std::endl;
                cout << id_request.id << endl;
    
                // Her kan du tilføje logik til at håndtere parkering baseret på ID'et
    
                int spot = Park(idToCheck, parkering);

                if(spot == 0) {
                    response_t t = {0, "car is already parked"};
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");
                }
                else if (spot = -6) {
                    response_t t = {0, "Please drive to the entrance"};    
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");
                }
            }

        }

        // Håndtering af fejl under processen
        catch (const std::exception &ex)
        {
            mark_as_bad_request(resp);
            resp.set_body(std::string{"Error: "} + ex.what());
        }

        return resp.done();
    }

    // Use Case 3: Udgang
    // Hovedlogik for håndtering af forespørgsler med ID vedrørende udgang af biler
    auto on_exit_request(const restinio::request_handle_t &req,
                        restinio::router::route_params_t)
    {
        auto resp = init_resp(req->create_response());

        try
        {
            if(sensorud2 == true)
            {
                Udgang = true;
                // Henter ID'et fra forespørgslen
                auto id_request = json_dto::from_json<ID_t>(req->body());
                int idToCheck = id_request.id;
                std::cout << "Body = " << req->body() << "" << std::endl;
                cout << id_request.id << endl;
    
                // Logik til håndtering af udgang
                int exitStatus = Exit(idToCheck, parkering);
    
                if(exitStatus == 1)
                {
                    response_t t = {idToCheck, "Thanks for visiting, drive safe!"};    
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");
                }
                else
                {
                    response_t t = {idToCheck, "Vehicle not parked"};  
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");  
                }
            }
            else {
                // Henter ID'et fra forespørgslen
                auto id_request = json_dto::from_json<ID_t>(req->body());
                int idToCheck = id_request.id;
                std::cout << "Body = " << req->body() << "" << std::endl;
                cout << id_request.id << endl;
    
                // Logik til håndtering af udgang
                int exitStatus = Exit(idToCheck, parkering);

                if(exitStatus == -6)
                {
                    response_t t = {0, "Drive to the exit"};    
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");
                }
                else
                {
                    response_t t = {0, "Vehicle not parked"};  
                    resp.set_body(json_dto::to_json(t))
                        .append_header("Content-Type", "application/json; charset=utf-8");  
                }
            }
        }

        // Håndtering af fejl under processen
        catch (const std::exception &ex)
        {
            mark_as_bad_request(resp);
            resp.set_body(std::string{"Error: "} + ex.what());
        }

        return resp.done();
    }

private:

    //Test
    pKompleks& parkering;
    // Slut Test

    // Respons logik
    template <typename RESP>
    static RESP init_resp(RESP resp)
    {
        resp.append_header("Server", "RESTinio Vehicle Server /v1.0")
            .append_header_date_field()
            .append_header("Content-Type", "application/json; charset=utf-8");
        return resp;
    }

    // Fejl håndtering
    static void mark_as_bad_request(auto &resp)
    {
        resp.header().status_line(restinio::status_bad_request());
    }

};

// --- Server handler ---
// Sætter router op for serveren samt definerer endpoints og deres tilhørende handlers
auto server_handler()
{
    using router_t = restinio::router::express_router_t<>;
    auto router = std::make_unique<router_t>();

    static parking_request_t vehicles;

    // Logik for håndtering af tilføjelse af køretøjer /add endpoint
    auto handler = std::make_shared<vehicle_handler_t>(vehicles);

    using namespace std::placeholders;
    router->http_post("/add", std::bind(&vehicle_handler_t::on_vehicle_add, handler, _1, _2));

    // Logik for håndtering af nummerplade forespørgsler /get endpoint
    auto plate_handler = std::make_shared<licensePlate_handler_t>();
    
    router->http_post("/register", std::bind(&licensePlate_handler_t::on_register_vehicle, plate_handler, _1, _2));
    
    // Logik for håndtering af ID forespørgsler /park og /exit endpoints
    //auto id_handler = std::make_shared<ID_handler_t>();

    // Test
    vector<int> parkingSlots = {1, 2, 3, 4};
    static pKompleks parkering(size(parkingSlots));
    parkering.init(parkingSlots, 2, 1, 1);

    auto id_handler = std::make_shared<ID_handler_t>(parkering);
    // Slut Test

    // HTTP POST endpoints for parkering af bil
    router->http_post("/park", std::bind(&ID_handler_t::on_parking_request, id_handler, _1, _2));

    // HTTP Delete endpoints for udgang af bil
    router->http_delete("/exit", std::bind(&ID_handler_t::on_exit_request, id_handler, _1, _2));
    

    return router;
}


void Server()
{

    try
    {
        // Definerer server traits
        using traits_t = restinio::traits_t<
            restinio::asio_timer_manager_t,
            restinio::single_threaded_ostream_logger_t,
            restinio::router::express_router_t<>
        >;

        // Starter serveren på angivet adresse og port med defineret request handler
        restinio::run(
            restinio::on_this_thread<traits_t>()
                .address("0.0.0.0")
                .port(8080)
                .request_handler(server_handler())
        );
    }
    
    // Håndtering af fejl under server opstart
    catch (const std::exception &ex)
    {
        std::cerr << "Server error: " << ex.what() << std::endl;
    }


}

void SensorThread()
{
    Sensor sensors(sensorInTrig, sensorInEcho, sensorOutTrig, sensorOutEcho, LEDInGreen, LEDInRed, LEDOutGreen, LEDOutRed);
    while(1)
    {
        sensors.sensorThread();
        sensorInd2 = sensors.GetSensorIn();
        sensorud2 = sensors.GetSensorUd(); 
    }
}

// --- Main ---
// Starter serveren og håndterer eventuelle fejl under opstart
int main()
{
   
    thread server(Server);
    thread sensor(SensorThread);


    server.join();
    sensor.join();

    return 0;
}