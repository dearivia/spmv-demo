#pragma once

#include <vector>
#include <functional>

#include "types.h"
#include "network.h"

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
    [[maybe_unused]] const CoordsMap& coords_to_thread;
    [[maybe_unused]] const VecMap& y_idx_to_thread;
    [[maybe_unused]] const ColSpMatrix& my_csc;
    [[maybe_unused]] const SpVector& my_v;
    [[maybe_unused]] SpVector& my_y;
    const int id;

    // Can add more fields here
    bool done_flag = false;
    std::map<int, int> y_updates_left; //idx, val
    std::map<int, double> partial_sums; //idx, val
};
