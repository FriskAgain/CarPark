#include "Car.h"
#include <string>
#include <string.h>
#include <cstdlib>
#include <fstream>   
#include <iostream>
using namespace std;



Cars::Cars(string model_, bool EV_, int ID_, string licensplate_)
{
    model = model_;
    EV = EV_;
    ID = ID_;
    licensplate = licensplate_;
}

void Cars::InitializeDatabase()
{
    Cars bob("BMW i4", true, 1526, "abc 123");
    Cars bob2("Audi A4", false, 1657, "hi 154");
    Cars bob3("Skoda octavia", false, 6548, "hz 456");
    Cars bob4("Hyundai Ioniq 6", true, 1101, "LB 1101");

    database.push_back(bob);
    database.push_back(bob2);
    database.push_back(bob3);
    database.push_back(bob4);
}


bool Cars::CompareDatabase(Cars &c)
{
    for(int i = 0; i < database.size(); i++)
    {
        if(c.licensplate == database.at(i).licensplate)
        {
            c.model = database.at(i).model;
            c.EV = database.at(i).EV;
            c.ID = database.at(i).ID;
            return true;
        }
    }

    return false;   
}



void Cars::handyCapCheck(string s)
{
    if(s != "y" && s != "Y")
    {
        handycapped = false;
    }
    else
    {
        handycapped = true;
    }
}



string Cars::print()
{
    string EVs;
    if(EV == true)
    {        EVs = "Yes";     
    }
    else
    {EVs = "No";
    }

    string handy;
    if(handycapped == true)
    {handy = "yes";}
    else{handy = "no";}


    return {"Model: " + model + "\n" + "Is an EV: " + EVs + "\n" 
            + "ID: " + to_string(ID) + "\n" + "Licensplate: " + licensplate + "\nHandycapped: " + handy};

} 


void Cars::setLincensplate(string s)
{
    licensplate = s;
}


void Cars::setModel(string s)
{
    model = s;
}

void Cars::setEV(string s)
{
   if(s != "y")
   {
    EV = false;
   } 
   else
   {
    EV = true;
   }

}

void Cars::setID()
{
    ID = rand() % 9000 + 1000;
}



void Cars::send_data_to_rasp_S(Cars user)
{
    ofstream outputFile("database.txt");
    if (outputFile.is_open())
    {
        outputFile << user.print();
    }
    outputFile.close();
    cout << "Data saved" << endl;
}