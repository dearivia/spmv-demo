// Everything in this file is fair game to be modified

#include "worker.h"

#include <fmt/core.h>

Worker::Worker(Network& _network, 
               const CoordsMap& _coords_to_thread, 
               const VecMap& _y_idx_to_thread,
               const ColSpMatrix& _my_csc, 
               const SpVector& _my_v, 
               SpVector& _my_y, 
               int _my_id) :
    network(_network),
    coords_to_thread(_coords_to_thread),
    y_idx_to_thread(_y_idx_to_thread),
    my_csc(_my_csc), 
    my_v(_my_v), 
    my_y(_my_y), 
    id(_my_id) {
}

bool Worker::done() const {
    return done_flag;
}

// This is purely a demo placeholder
void Worker::handle_message(Message msg) {
    fmt::print("Worker {} handling message {}\n", id, msg);
    switch (msg.type) {
        case 0:
            network.send(Message(1, (id + 1) % network.nthreads, 0, 0.0));
            break;
        case 1:
            done_flag = true;
            break;
        default:
            fmt::print("Worker {} received unknown message type {}\n", id, msg.type);
    }
}