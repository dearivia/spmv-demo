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
    for (auto& y : y_idx_to_thread) { //fill out y_updates VecMap = std::map<int, int>; //idx of vector to where it's stored
        y_updates_left[y.first] = 0; //update index to 0
        partial_sums[y.first] = 0;
    }


    for (auto& col : my_csc){ //using ColSpMatrix = std::map<int, std::vector<std::pair<int,double>>>; //col_idx -> (row_idx,val)
//index into map, pull out second vector and loop through vector
        for (const auto& row : col.second) {
            y_updates_left[row.first]++; //loop through, if col, row exists, then update++
        }
    }
}

bool Worker::done() const {
    return done_flag;
}

void Worker::handle_message(Message msg) {
    fmt::print("Worker {} handling message {}\n", id, msg);
    switch (msg.type) {
        case 0: //case 0 go through vector, send workers messages of where it wants to multiply
            for (const auto& v : my_v){ //go through the vector SpVector = std::map<int,double>; //idx, val
                for (const auto& col_pair : my_csc) {
                  int row = col_pair.first; // Assume this is the row index from your `ColSpMatrix`
                  
                  std::pair<int, int> key = std::make_pair(row, col); // Construct the key
                  if (coords_to_thread.find(key) != coords_to_thread.end()) {
                      int m_worker = coords_to_thread.at(key); // Access the map with the correct key type
                      network.send(Message(1, m_worker, col, v.second)); // Send message 1 to multiply
                  }    
                }
            }
                //int m_worker = coords_to_thread.at(v.first); //finds M worker in column
                //network.send(Message(1, m_worker, v.first, v.second));//send message 1 to multiply
            break;
        case 1: //scenario 1: given vector, worker calculates csc, my_v, and sends it to worker in charge of y  
            for (const auto& row : my_csc.at(msg.coord)) { //idx, val
                double res = row.second*msg.payload;//result[row] += matrix[row][col]*vector[col]
                int y_worker = y_idx_to_thread.at(row.first);//the worker in charge of result[row]
                network.send(Message(2, y_worker, row.first, res));//send worker in charge of result to change y
                //add to my_y directly or add to partial here. 
            }
            //network.send(Message(1, (id + 1) % network.nthreads, 0, 0.0)); //raise done_flag! (aka sends this to the queue of jobs)
            break;
        case 2: //scenario 2: worker in charge of y receives the partial sums from the earlier multiplication for y
            partial_sums[msg.coord] += msg.payload;
            y_updates_left[msg.coord]--;
            if (y_updates_left[msg.coord]==0){
                my_y[msg.coord] = partial_sums[msg.coord];
            }
            //if everything is done, then set done value to be done, be careful of scenario 1
            //my_y[msg.coord] += msg.payload; message coordinate of y adds result to summation
            //increment how many updates had and check if updates = needed
            //wait until all summations are gotten before sending
            break;
        case 3: //scenario 3: when worker is done processing
            done_flag = true;
            break;
        default: //scenario 3: error
            fmt::print("Worker {} received unknown message type {}\n", id, msg.type);
    }
}
