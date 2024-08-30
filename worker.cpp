// Everything in this file is fair game to be modified
#include "worker.h"
#include <fmt/core.h>

Worker::Worker(Network& _network, 
               const CoordsMap& _coords_to_thread, //go thru all the coords + check if this coord is updating a coordinate i own
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
    fmt::print("Worker {} initialized.\n", id);
    total_updates_left = 0;
    std::mutex update_mutex;

    for (auto& y : y_idx_to_thread) { //fill out y_updates VecMap = std::map<int, int>; //idx of vector to where it's stored
        y_updates_left[y.first] = 0; //update index to 0
        partial_sums[y.first] = 0.0;
        fmt::print("y_idx_to_thread: {} -> {}\n", y.first, y.second);
    }
    for (const auto& v : my_v){
        fmt::print("Worker {} owns my_v: {}\n", id, v.first);
    }
    for (const auto& y : my_y){
        fmt::print("Worker {} owns my_y: {}\n", id, y.first);
    }
    for (auto& coord: coords_to_thread){ //using ColSpMatrix = std::map<int, std::vector<std::pair<int,double>>>; //col_idx -> (row_idx,val)
//index into map, pull out second vector and loop through vector
        //int row_idx = coord.first.first; //row index of matrix element 
        int col_idx = coord.first.second;
        //int thread_owner = coord.second; //worker responsible for this matrix element

        if (my_y.find(col_idx) != my_y.end()) {
            y_updates_left[col_idx]++;
            //y_updates_left[row.first]++; //loop through, if col, row exists, then update++ 
            total_updates_left++;
        }
        //for (const auto& row : v.second) {
        //    std::lock_guard<std::mutex> lock(update_mutex);
        //    y_updates_left[row.first]++;
            //y_updates_left[row.first]++; //loop through, if col, row exists, then update++ 
       //     total_updates_left++;
       //     fmt::print("my_csc: ({}, {}) -> {}\n", col.first, row.first, row.second);
       // } //check if y_updates is the right amount ksjd
    }
    for (const auto& y_update : y_updates_left) {
        fmt::print("y_updates_left[{}]: {}\n", y_update.first, y_update.second);
    }
    fmt::print("Total updates initialized: {}\n", total_updates_left);
}

//the use of total-updates-left for each worker is the problem.
bool Worker::done() const {
    return done_flag;
}
void Worker::handle_message(Message msg) {
    fmt::print("Worker {} handling message {}\n", id, msg);
    switch (msg.type) {
        case 0: //case 0 go through vector, send workers messages of where it wants to multiply
            fmt::print("Message case 0 processing for worker {}.\n", id);
            for (const auto& v : my_v){ //go through the vector SpVector = std::map<int,double>; //idx, val works
                fmt::print("Worker {} processing vector element: ({}, {})\n", id, v.first, v.second); //col_idx i want, v.second = value
                //now i want to go thru coords_to_threads and send messages around
                for (const auto& coord_thread_pair : coords_to_thread) {
                    const auto& coord = coord_thread_pair.first; // `coord` is a pair<int, int> representing (row, col)
                    int m_worker = coord_thread_pair.second;     // Worker responsible for (row, col) in matrix

                    if (coord.first == v.first) { // `coord.first` is the row index of a matrix entry
                        int col = coord.second;
                        fmt::print("Worker {} sending message type 1 to worker {}\n", id, m_worker);
                        network.send(Message(1, m_worker, col, v.second));//send message 1 to multiply
                    }
                //for (const auto& [col, _] : my_csc) {
                
                  //if (coords_to_thread.find({v.first, col}) != coords_to_thread.end()) {
                //    int m_worker = coords_to_thread.at({v.first, col}); //finds M worker in column
               //     fmt::print("Worker {} sending message type 1 to worker {}\n", id, m_worker);
                //    network.send(Message(1, m_worker, col, v.second));//send message 1 to multiply
                  }
                
            }
                //int m_worker = coords_to_thread.at(v.first); //finds M worker in column
                //network.send(Message(1, m_worker, v.first, v.second));//send message 1 to multiply
            break;
        case 1: //scenario 1: given vector, worker calculates csc, my_v, and sends it to worker in charge of y  
            fmt::print("Message case 1 processing for worker {}.\n", id);
            if (my_csc.find(msg.coord) != my_csc.end()) {
              for (const auto& row : my_csc.at(msg.coord)) { //idx, val
                fmt::print("Worker {}: Processing row: ({}, {}) with payload {}\n", id, row.first, row.second, msg.payload);
                double res = row.second*msg.payload;//result[row] += matrix[row][col]*vector[col]
                if (y_idx_to_thread.find(row.first) != y_idx_to_thread.end()){
                  int y_worker = y_idx_to_thread.at(row.first);//the worker in charge of result[row]//the worker in charge of result[row]
                  fmt::print("Worker {} sending message type 2 to worker {}\n", id, y_worker);
                  network.send(Message(2, y_worker, row.first, res));//send worker in charge of result to change y
                }
              }
                  //add to my_y directly or add to partial here. 
            }
            //network.send(Message(1, (id + 1) % network.nthreads, 0, 0.0)); //raise done_flag! (aka sends this to the queue of jobs)
            break;
        case 2: //scenario 2: worker in charge of y receives the partial sums from the earlier multiplication for y
            fmt::print("Worker {} Message case 2 processing for coord {} with payload {}\n", id, msg.coord, msg.payload);
            //if (my_y.find(msg.coord) != my_y.end()) {
                //assert assumptions, check its coordinates we own assert()
                partial_sums[msg.coord] += msg.payload; //works
                //if (y_updates_left[msg.coord] > 0) {
                //    y_updates_left[msg.coord]--;
               //     fmt::print("Updates left for coord {}: {}\n", msg.coord, y_updates_left[msg.coord]);
                //} else {
                //    fmt::print("Warning: Updates left for coord {} was already zero or negative!\n", msg.coord);
                //}
                y_updates_left[msg.coord]--; //works
                fmt::print("Updates left for coord {}: {}\n", msg.coord, y_updates_left[msg.coord]);

                if (y_updates_left[msg.coord] == 0) { //works
                    my_y[msg.coord] = partial_sums[msg.coord]; //works
                    //y_updates_left.erase(msg.coord);
                    fmt::print("Updated my_y[{}] to {}\n", msg.coord, partial_sums[msg.coord]); 
                } 

                total_updates_left--; //works
                fmt::print("Total updates left for worker {}: {}\n", id, total_updates_left); //works
            
                //if (total_updates_left > 0) {
                //    total_updates_left--;
                //    fmt::print("Total updates left: {}\n", total_updates_left);
                //} else {
                //    fmt::print("Error: Attempted to decrement total_updates_left below zero!\n");
                //}
                
                //if (y_updates_left[msg.coord]<0){
                //    fmt::print("Warning: y updates left is negative for coord {}\n", msg.coord);
                //    my_y[msg.coord] = partial_sums[msg.coord]; 
                //}
                
                if (total_updates_left == 0){
                    fmt::print("All updates processed for worker {}.\n", id);
                    //done_flag = true;
                    for (int i = 0; i < network.nthreads; ++i) {
                        network.send(Message(3, i, 0, 0.0));  // Send type 3 message to all workers, # of workers = int nthreads
                    }
                } 
        
            //} else {
            //    fmt::print("Warning: y_updates_left not found or invalid for coord{}\n", msg.coord);
            //}
            //if everything is done, then set done value to be done, be careful of scenario 1
            //my_y[msg.coord] += msg.payload; message coordinate of y adds result to summation
            //increment how many updates had and check if updates = needed
            //wait until all summations are gotten before sending
            break;
        case 3: //scenario 3: when worker is done processing
            fmt::print("Worker {} received completion message from another worker.\n", id);
            completed_workers++;
            if (completed_workers == network.nthreads) {  // All other workers have reported completion
                fmt::print("All workers have completed. Worker {} setting done_flag to true.\n", id);
                done_flag = true;
            }
            break;
        default: //scenario 3: error
            fmt::print("Worker {} received unknown message type {}\n", id, msg.type);
    }
}
