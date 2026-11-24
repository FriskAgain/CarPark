#include "pKompleks.hpp"

// Laver firkant fra 0(start) til 1 - 2 - 3 - ... - n (parkeringspladser) og tilbage til 1 (udgangen)
pKompleks::pKompleks(int parkingSlots_) : parkingSlots(parkingSlots_) {
    V = parkingSlots_ + 1; //+1 til udgangen (dijkstra start)
    for (int i = 0; i < V - 1; ++i) {
        edges.push_back({ i, i + 1, 1 });
    }
    edges.push_back({ parkingSlots_, 1, 1 });
}

// Peger begge veje , da grafen er undirected (u på v og v på u)
//Hj�lpe funktion: Laver en adjacency-list (en liste af par - nabo edge (v) og v�gt)
vector<vector<pair<int, int>>> pKompleks::constructAdj() {
    vector<vector<pair<int, int>>> adj(V);
    for (auto& e : edges) {
        int u = e[0], v = e[1], w = e[2];
        adj[u].push_back({ v, w });
        adj[v].push_back({ u, w }); // if undirected
    }
    return adj;
}

vector<int> pKompleks::dijkstra() {
    int src = 0;
    vector<vector<pair<int, int>>> adj = constructAdj();
    vector<int> dist(V, INT_MAX);
    dist[src] = 0;

    //{distance, node}
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    pq.push({ 0, src });

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // Skip outdated distances
        if (d > dist[u]) continue;

        for (auto& [v, w] : adj[u]) {
            if (dist[v] > d + w) {
                dist[v] = d + w;
                pq.push({ dist[v], v });
            }
        }
    }

    return dist;
}

//Normal, handy og el er antal pladser
/*
void pKompleks::init(vector<int> parkingSlots, int normal, int el, int handy) {
    vector<int> dist = dijkstra();
    
    // Create priority queue of {distance, slotID}
    int id = 1;
    int i{};
    for (i = 0; i < handy;  i++) {
        handy_.push({ dist[id], id });
		id++;
    }
    

    for (i = 0; i < normal; i++)
    {
		normal_.push({ dist[id], id });
        id++;
    }
    for (i = 0; i < el; i++)
    {
        el_.push({ dist[id], id });
        id++;
    }



    cout << "Parking slots sorted by distance:\n";
    while (!handy_.empty()) {
        auto [d, id] = handy_.top();
        handy_.pop();
        cout << "Handy slot ID: " << id << ", Distance: " << d << endl;
    }
    while (!el_.empty()) {
        auto [d, id] = el_.top();
        el_.pop();
        cout << "El slot ID: " << id << ", Distance: " << d << endl;
    }
    while (!normal_.empty()) {
        auto [d, id] = normal_.top();
        normal_.pop();
        cout << "Normal slot ID: " << id << ", Distance: " << d << endl;
    }
}
*/

const priority_queue <pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> &pKompleks::get_normal() const {
    return normal_;
}
const priority_queue <pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> &pKompleks::get_el() const {
    return el_;
}
const priority_queue <pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> &pKompleks::get_handy() const {
    return handy_;
}

// Test funktioner
int pKompleks::pop_normal() {
    if(normal_.empty()) return -1;
    int id = normal_.top().second;
    normal_.pop();
    return id;
}

int pKompleks::pop_el() {
    if(el_.empty()) return -1;
    int id = el_.top().second;
    el_.pop();
    return id;
}

int pKompleks::pop_handy() {
    if(handy_.empty()) return -1;
    int id = handy_.top().second;
    handy_.pop();
    return id;
}

void pKompleks::init(vector<int> parkingSlots, int normal, int el, int handy) {
    dist_ = dijkstra(); // save distances

    int id = 1; // Prøver at ændre fra 1 til 0
    for (int i = 0; i < handy; i++) {
        handy_.push({ dist_[id], id });
        id++;
    }
    for (int i = 0; i < normal; i++) {
        normal_.push({ dist_[id], id });
        id++;
    }
    for (int i = 0; i < el; i++) {
        el_.push({ dist_[id], id });
        id++;
    }
}

void pKompleks::push_normal(int slotID) {
    normal_.push({dist_[slotID], slotID});
}

void pKompleks::push_el(int slotID) {
    el_.push({dist_[slotID], slotID});
}

void pKompleks::push_handy(int slotID) {
    handy_.push({dist_[slotID], slotID});
}
