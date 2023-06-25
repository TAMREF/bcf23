#pragma once

#include "capper.hpp"
#include <random>

using namespace std;

// SSSPConfig manages universal config to solve SSSP, such as RNG or time budget.
struct SSSPConfig {
    OperationCapper *capper;
    mt19937_64 rng;

    explicit SSSPConfig(OperationCapper *capper, mt19937_64 rng) : capper(capper), rng(rng) {}

    SSSPConfig(size_t budget, size_t seed = 0x5174) : rng(seed) {
        if(budget == size_t(-1)) capper = new NoCapOperationCapper();
        else capper = new NormalOperationCapper(budget);
    }
};