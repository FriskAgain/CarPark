# PI-PARK Client

# Getting Started
**Forfatter:** Lasse Breuer
**Dato:** 11 december 2025

## Dependencies
* Have the ecesyshat libraries installed on your rasp. If not install this to your rasp:gitlab.au.dk/platforms/rpi5/ecesyshat/
* Have SPI and I2C enabled
* Have CURL installed, if not : sudo apt install curl libcurl4-openssl-dev
* Have rapidjson installed, if not : sudo apt install rapidjson-dev


## Executing program UC1
To compile UC1
```
//Be in folder src/
g++ UC1_Register.cpp -o UC1.exe -lcurl -I../external/json_dto
./UC1.exe

```

## Executing program UC2/UC3

```
Be in folder with the CMakeLists.txt
rm -r build/ (if it exists)
cmake -S . -B build/
cd build/
(Make a new config.txt:
ip:"server IP" 
ID:"Your ID")
make
./client.exe

```





# PI-Park oversigt
**Forfatter:** Aske Pilegaard
**Dato:** 24. november 2025

## Modulbeskrivelse
Klienten sender nummerpladeinformation til PI-PARK serverens /get-endpoint. Den benytter en lokal config.txt fil til to formål: at aflæse hvilken IP-adresse serveren kører på, og at gemme det aktuelle ID, som opdateres dynamisk ud fra serverens svar.
Klienten er skrevet i C++ og bruger libcurl til HTTP-kommunikation og json_dto til JSON-serialisering.

## Filstruktur
```
/client
└──src
    ├── main.cpp
    ├── config.txt
    └── (ekstern)
```

**config.txt format:**
```ini
ip=192.168.0.10
ID=0
```

## Funktionalitet
Klienten udfører følgende:
1. Beder brugeren indtaste en nummerplade.
2. Læser serverens IP fra config.txt.
3. Opretter JSON for nummerpladen med json_dto.
4. Sender en HTTP POST-request til serverens /get.
5. Modtager JSON-svar fra serveren.
6. Udskriver HTTP-status og serverens besked.
7. Opdaterer ID i config.txt baseret på serverens svar.

## Datastrukturer
**licensePlate_t**

Bruges til at sende JSON til serveren:
```json
{
  "LicensePlate": "ABC123",
  "Model": "",
  "Handicap": false,
  "Electric": false
}
```

**response_t**

struktur for serverens svar:
```json
{
  "id": 0,
  "message": "Vehicle found, already registered"
}
```

## Workflow

### Trin 1 - Brugerinput
Klienten beder brugeren om at indtaste en nummerplade:

```text
Please enter License Plate number:
```

### Trin 2 - Henter IP fra config.txt
Programmet læser IP fra en linje i filen:

```ini
ip=192.168.0.10
```

### Trin 3 - JSON serialisering
Klienten konverterer objektet:

```cpp
json_dto::to_json(LP);
```
→ til JSON-body.

### Trin 4 - Sender POST request til serveren
Klienten bruger libcurl til at sende en HTTP-request:

```http
POST /get HTTP/1.1
Host: <server-ip>:8080
Content-Type: application/json

{
  "LicensePlate": "ABC123"
}
```

### Trin 5 - Modtager svar
Klienten udskriver:
- HTTP-statuskode
- Serverens JSON-svar

```text
HTTP kode: 200
Serverens svar: {"id":0,"message":"Vehicle found, already registered"}
```

### Trin 6 - Opdaterer config.txt
Den eksisterende:
```ini
ID = 0
```
opdateres med det nye ID fra serverens svar.

---

## Fejl-håndtering
klienten håndterer fejl som:

| Situation                     | CURL fejltype            | Beskrivelse                                   |
|------------------------------|---------------------------|-----------------------------------------------|
| Timeout                      | `CURLE_OPERATION_TIMEDOUT` | Forbindelsen tog længere end 5 sekunder.      |
| Ingen forbindelse            | `CURLE_COULDNT_CONNECT`    | Klienten kunne ikke forbinde til serveren.    |
| Anden CURL fejl              | `CURLE_*`                 | Udskriver fejlbesked via `curl_easy_strerror` |

---

## Eksempel på brug

(HTTP-visning) først sendes en request fra klienten:

```http
POST /get HTTP/1.1
Host: 192.168.0.10:8080
Content-Type: application/json

{
  "LicensePlate": "ABC123"
}
```

Forventet serversvar:

```json
{
  "id": 1,
  "message": "Vehicle Registered successfully"
}
```

Efterfølgende opdateres ID:

```ini
ID = 1
```

---