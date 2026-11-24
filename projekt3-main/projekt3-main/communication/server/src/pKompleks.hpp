#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <queue>
#include <climits>
using namespace std;

class pKompleks {
public:
    pKompleks(int parkingSlots_);
    void init(vector<int> plads, int normal, int el, int handy);

    const priority_queue <pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>>& get_normal() const;
    const priority_queue <pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>>& get_el() const;
    const priority_queue <pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>>& get_handy() const;

    // Test funktioner
    // Pop next available spot
    int pop_el();
    int pop_handy();
    int pop_normal();

    // Return spot back to queue when vehicle exits
    void push_el(int slotID);
    void push_handy(int slotID);
    void push_normal(int slotID);

private:
    vector<vector<pair<int, int>>> constructAdj();
    vector<int> dijkstra();
    int V;
    int parkingSlots;
    vector<vector<int>> edges;

    priority_queue <pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> normal_;
	priority_queue <pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> el_;
	priority_queue <pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> handy_;

    // Test
    vector<int> dist_;
};