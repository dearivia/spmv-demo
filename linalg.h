#pragma once 

#include <vector>
#include <cassert>
#include <cmath>

std::vector<double> spmv(const MatrixMarket::CSCMatrix<int,double>& csc_mtx,
                         const std::vector<double>& v) {

    std::vector<double> y(csc_mtx.num_rows, 0.0);

    for (int col_ptr = 0; col_ptr < csc_mtx.num_cols; col_ptr++) {

        auto col_start = csc_mtx.col_offsets.at(col_ptr);
        auto col_end = csc_mtx.col_offsets.at(col_ptr + 1);
        
        for (int r = col_start; r < col_end; r++) {
            auto row = csc_mtx.row_indices.at(r);
            auto val = csc_mtx.values.at(r);
            y.at(row) += val * v.at(col_ptr);
        }
    }

    return y;
}

std::vector<double> vec_sub(const std::vector<double>& a, 
                            const std::vector<double>& b) {
    assert(a.size() == b.size());

    std::vector<double> c(a.size(), 0.0);
    for (unsigned int i = 0; i < a.size(); i++) {
        c[i] = a[i] - b[i];
    }
    return c;
}

double vec_norm(const std::vector<double>& a) {
    double norm = 0.0;
    for (auto& x : a) {
        norm += x * x;
    }
    return sqrt(norm);
}
