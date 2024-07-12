#include <thread>
#include <fmt/core.h>
#include <matrixmarket.hpp>
#include <cstdlib>
#include <cassert>
#include <vector>

#include "types.h"
#include "worker.h"
#include "network.h"
#include "linalg.h"

void thread_func(Network& network, Worker& worker, int my_id) {
    while (!worker.done()) {
        auto msg = network.recv(my_id);
        assert(msg.dest == my_id);
        worker.handle_message(msg);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fmt::print("Usage: {} <matrix> <nthreads>\n", argv[0]);
        return 1;
    }
    fmt::print("{}\n", argv[1]);
    auto csc_mtx = MatrixMarket::read_csc<int,double>(argv[1]);
    int nthreads = std::stoi(argv[2]);

    Network network(nthreads);

    std::vector<double> v(csc_mtx.num_cols, 1.0);
    std::vector<double> y(csc_mtx.num_rows, 0.0);

    std::vector<ColSpMatrix> matrix_parts(nthreads);
    std::vector<SpVector> v_parts(nthreads);
    std::vector<SpVector> y_parts(nthreads);

    CoordsMap coords_to_thread;

    for (int col_ptr = 0; col_ptr < csc_mtx.num_cols; col_ptr++) {

        auto col_start = csc_mtx.col_offsets.at(col_ptr);
        auto col_end = csc_mtx.col_offsets.at(col_ptr + 1);
        
        for (int r = col_start; r < col_end; r++) {
            auto row = csc_mtx.row_indices.at(r);
            auto val = csc_mtx.values.at(r);
            auto thread_id = r % nthreads;
            matrix_parts[thread_id][col_ptr].push_back({row, val});
            coords_to_thread[{row, col_ptr}] = thread_id;
        }
    }

    for (unsigned int i = 0; i < v.size(); i++) {
        auto thread_id = i % nthreads;
        v_parts[thread_id][i] = v.at(i);
    }

    VecMap y_idx_to_thread;

    for (unsigned int i = 0; i < y.size(); i++) {
        auto thread_id = i % nthreads;
        y_parts[thread_id][i] = y.at(i);
        y_idx_to_thread[i] = thread_id;
    }

    std::vector<Worker> workers;
    for (int i = 0; i < nthreads; i++) {
        workers.emplace_back(network, 
                             coords_to_thread, 
                             y_idx_to_thread,
                             matrix_parts[i], 
                             v_parts[i], 
                             y_parts[i], 
                             i);
    }


    std::vector<std::thread> threads;
    for (int i = 0; i < nthreads; i++) {
        threads.emplace_back(thread_func, std::ref(network), std::ref(workers.at(i)), i);
    }

    for (int i = 0; i < nthreads; i++) {
        auto msg = Message(0, i, 0, 0.0);
        network.send(msg);
    }

    for (auto& t : threads) {
        t.join();
    }

    for (auto& y_part : y_parts) {
        for (auto& [idx, val] : y_part) {
            y.at(idx) = val;
        }
    }

    auto y_ref = spmv(csc_mtx, v);
    auto res_vector = vec_sub(y, y_ref);
    auto res_norm = vec_norm(res_vector);
    auto y_norm = vec_norm(y_ref);

    fmt::print("Residual norm: {}, relative err: {}\n", res_norm, res_norm / y_norm);

}
