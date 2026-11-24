// THIS SHIT IS NOT IN USE, BUT IN CASE WE NEED IT LATER!!!!!!!!!!!!!!!!!!!!!!!









    /* struct vehicle_t
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

    // json_dto bruger denne metode til at beskrive, hvilke felter der hører til hvilke JSON-keys
    template <typename JSON_IO>
    void json_io(JSON_IO &io)
    {
        io & json_dto::mandatory("LicensePlate", m_licensePlate)
           & json_dto::mandatory("Model", m_model)
           & json_dto::mandatory("Handicap", m_handicap)
           & json_dto::mandatory("Electric", m_electric);
    }

    std::string m_licensePlate;
    std::string m_model;
    bool m_handicap;
    bool m_electric;
};
 */


int main(){

     //vehicle_t toyota("TD216642", "Toyota AYGO", false, false);
    return 0;
}