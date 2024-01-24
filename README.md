# Simple SpMV Implementation Challenge

The goal of this challenge is to implement a sparse-matrix times vector
product in a multithreaded "message passing" setting.

For terminology puruposes, we'll write this computation as `y = M @ v`.

### Code Overview

Building with `scons` will produce an executable called `spmv`.
To call `spmv`, provide it with the path to a sparse matrix in
matrix-market format (`*.mtx`) and a number of threads.

Example:
```
./spmv /data/sanchez/users/axelf/scratch/b1_ss/b1_ss.mtx 4
```

Right now, it won't do much. In the future (hopefully), it will
multiply the matrix by a vector of all ones.

The code in `spmv.cpp` should not be touched. At a high level,
it reads in the matrix, partitions it among the `nthreads` threads being
run (in some unspecified order that you should not depend on!) then
spins up `nthreads` `Workers`. Each `Worker` is continuously running this code:

```
void thread_func(Network& network, Worker& worker, int my_id) {
    while (!worker.done()) {
        auto msg = network.recv(my_id);
        assert(msg.dest == my_id);
        worker.handle_message(msg);
    }
}
```

Namely: while it is not done, it is handling incoming messages over the
network. It is your responsibility to
implement the `done()` and `handle_message()`!

**You should write code only in `worker.h` and `worker.cpp`.**

A few details. The worker constructor (that you can modify) is as follows:

```
    Worker(Network& network, 
           const CoordsMap& coords_to_thread, 
           const VecMap& y_idx_to_thread,
           const ColSpMatrix& my_csc, 
           const SpVector& my_v, 
           SpVector& my_y, 
           int my_id);
```

Don't modify the *type* of the constructor-- doing so would require
changing the code in `spmv.cpp` which is not allowed.

Each worker receives some part of `M`, some part of `v`, and some part
of the result vector `y`. The datatypes are all defined in
`types.h`. Importantly, the worker is also told where *other* parts of
`M` and `y` reside in `coords_to_thread` and `y_idx_to_thread`.

Finally, here is the type of `Message`:

```
struct Message {
    int type;
    int dest;
    int coord;
    double payload;
    Message(int _type, int _dest, int _coord, double _payload) : 
        type(_type), dest(_dest), coord(_coord), payload(_payload) {}
};
```

The network will route a message to the thread whose `id` is equal to `dest`.
You can use the remaining fields however you want-- the network will
not touch them.

To kick-start computation, each thread is *automatically* sent a single
message of `type` 0.