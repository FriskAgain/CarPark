#pragma once
#include <string>
#include <vector>
using namespace std;


class Cars
{
private:
        string model;
        string licensplate;
        bool handycapped = false;
        bool EV = false;
        bool normal = true;
        int ID;
public:
    Cars(string model, bool EV, int ID, string licensplate);
    bool CompareDatabase(Cars &c);
    vector<Cars> database;
    void InitializeDatabase();
    string print();
    void handyCapCheck(string s);
    void setEV(string s);
    void setModel(string s);
    void setID();
    void setLincensplate(string s);
    void send_data_to_rasp_S(Cars c);
};