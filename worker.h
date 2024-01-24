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
    const CoordsMap& coords_to_thread;
    const VecMap& y_idx_to_thread;
    const ColSpMatrix& my_csc;
    const SpVector& my_v;
    SpVector& my_y;
    const int id;

    // Can add more fields here
    bool done_flag = false;
};