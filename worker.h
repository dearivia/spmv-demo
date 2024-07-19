#pragma once

#include <vector>
#include <functional>
#include <map>
#include <utility>

#include "types.h"
#include "network.h"

//using ColSpMatrix = std::map<int, std::vector<std::pair<int, double>>>;
//using SpVector = std::map<int, double>;
//using CoordsMap = std::map<std::pair<int, int>, int>;
//using VecMap = std::map<int, int>;

class Worker {
public:
    Worker(Network& network, 
           const CoordsMap& coords_to_thread, 
           const VecMap& y_idx_to_thread,
           const ColSpMatrix& my_csc, 
           const SpVector& my_v, 
           SpVector& my_y, 
           int my_id);

    bool done() const;
    void handle_message(Message msg);
private:
    Network& network;
    const CoordsMap& coords_to_thread;
    const VecMap& y_idx_to_thread;
    const ColSpMatrix& my_csc;
    const SpVector& my_v;
    SpVector& my_y;
    const int id;

    // Can add more fields here
    bool done_flag = false;
    int total_updates_left;
    std::map<int, int> y_updates_left; //idx, val
    std::map<int, double> partial_sums; //idx, val
    int total_updates_left;
};
