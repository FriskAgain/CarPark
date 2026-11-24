#include <iostream>
#include <string.h>
#include <vector>
#include "Car.h"
#include <unistd.h>
using namespace std;


int main()
{
    
    Cars user("x",false,0,"x");
    user.InitializeDatabase();
    
    string licensplate;
    string hanycapped;
    
    int check = 0;
    while(check==0)
    {
    cout << "enter your lincensplate:" << endl;
    getline(cin,licensplate);

    user.setLincensplate(licensplate);
    
    if(user.CompareDatabase(user) == true)
    {
        cout << "Are you handycapped? y/n" << endl;
        getline(cin, hanycapped);
       user.handyCapCheck(hanycapped);
       cout << user.print() << endl;
        break;
    }
    else
    {
        cout << "Data not found" << endl;
        cout << "Set data maually? y/n" << endl;
    }
        string s;
        getline(cin,s);
        if(s != "y")
        {
            cout << "retry pls" << endl;
        }
        if(s == "y" || s == "Y")
        {
            string Model;
            string EV;
            string licensplate;
            cout << "Model:" << endl;
            getline(cin,Model);
            cout << "EV? y/n" << endl;
            getline(cin, EV);
            cout << "Lincensplate:" << endl;
            getline(cin,licensplate);

            user.setEV(EV);
            user.setModel(Model);
            user.setID();
            user.setLincensplate(licensplate);

            cout << "Are you handycapped? y/n" << endl;
            getline(cin, hanycapped);
            user.handyCapCheck(hanycapped);

            cout << user.print() << endl;
            user.database.push_back(user);
            check = 1;
        }
    }

    

    cout << "Saving data..." << endl;
    usleep(500000);
    cout << ".";
    usleep(500000);
    cout << ".";
    usleep(500000);
    cout << ".";
    usleep(500000);
    cout << "." ;
    usleep(500000);
    cout << "." ;
    usleep(500000);
    cout << "." ;
    usleep(500000);
    cout << "." ;



    user.send_data_to_rasp_S(user);


    string i;
    cout << "Enter l to exit" << endl;
    cin >> i;

    return 0;
}