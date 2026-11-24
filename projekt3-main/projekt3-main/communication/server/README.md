# PI-PARK Server REST API

**Forfatter:** Aske Pilegaard  
**Dato:** 24. november 2025

## Modulbeskrivelse
Serveren håndterer registrering og forespørgsel af køretøjer til PI-PARK systemet. Den bruger JSON-filer som databaser og returnerer svar i JSON-format. Serveren er bygget med C++ og RESTinio.


## Filstruktur
```
/server
├── build
├── external
├── src
│    ├── main.cpp           
│    ├── API.json          
│    └── Registered.json
├── CMakeLists.txt
└── README.md
```
Bemærk:  
> - `build/` genereres under bygning og vises ikke her.  
> - `external/` indeholder afhængigheder som RESTinio og json_dto.  
> - Fokus her er på egen kode.

---

## Funktioner
- Registrer nye køretøjer via API tjek
- Tjek eksisterende køretøjer via nummerplade
- Gemmer information i JSON-filer (Registered.json)
- Returnerer JSON-svar til klienter i form af statusbesked og ID

---

## API Endpoints

### 1. Tilføj køretøj (Test)
**Endpoint:** `/add`  
**HTTP metode:** `POST`  
**Input JSON:**
```json
{
    "LicensePlate": "ABC123",
    "Model": "Tesla Model 3",
    "Handicap": false,
    "Electric": true
}
```

**Output JSON:**
```json
{
    "id": 1,
    "message": "Vehicle registered successfully"
}
```

### 2. Tjek om bil er registreret via Registered.json / Registrer bil hvis ikke
**Endpoint:** `/register`  
**HTTP metode:** `POST`  
**Input JSON:**
```json
{
    "LicensePlate": "ABC123"
}
```

Serveren kan returnere tre typer svar:

**Output JSON:**
* Hvis køretøj allerede er registreret
```json
{
    "id": 0, "message": "Vehicle found, already registered"

}
```

* Hvis køretøj registreres
```json
{ 
    "id": 2, "message": "Vehicle Registered succesfully" 
}
```

* Hvis køretøj ikke findes i API
```json
{
    "id": 0, "message": "Vehicle not found!"
}
```

### 3. Parkering af bil via Parking.json og Registered.json
**Endpoint:** `/park`  
**HTTP metode:** `POST`  


### 4. Udgang fra parkering
**Endpoint:** `/exit`  
**HTTP metode:** `DELETE`  



## Serveropsætning
For at køre PI-PARK serveren skal det følgende være installeret:

**1. Krav:**
- C++17 compiler
- RESTinio
- nlohmann/json
- json_dto

**2. Byg og kør serveren:**
```bash
cmake -S . -B build
cmake --build build
./build/restinio_project
```

Standard adresse og port:
- Adresse: 0.0.0.0
- Port: 8080


## Eksempel på brug via klient med `Postman`
```http
POST /get HTTP/1.1
Host: localhost:8080
Content-Type: application/json

{"LicensePlate": "ABC123"}
```