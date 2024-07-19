#pragma once

#include <vector>
#include "channel.h"

class Network {
public:
    Network(int _nthreads) : nthreads(_nthreads), queues(_nthreads) {
    }

    void send(Message msg) {
        queues.at(msg.dest).push_back(msg);
    }

    Message recv(int thread_id) {
        return queues.at(thread_id).pop_front();
    }

    const int nthreads;

private:
    std::vector<Channel<Message>> queues;
};

//run and debug