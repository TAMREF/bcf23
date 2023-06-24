#pragma once

#include "capper.hpp"
#include <random>

using namespace std;

// SSSPConfig manages universal config to solve SSSP, such as RNG or time budget.
struct SSSPConfig {
    OperationCapper *capper;
    mt19937_64 rng;

    SSSPConfig(OperationCapper *capper, mt19937_64 rng) : capper(capper), rng(rng) {}
};

SSSPConfig NewTrivialConfig(size_t seed = 0x5174) {
    auto capper = new NoCapOperationCapper();
    auto rng = mt19937_64(seed);
    return SSSPConfig(capper, rng);
}

SSSPConfig NewCappedConfig(size_t budget, size_t seed = 0x5174) {
    auto capper = new NormalOperationCapper(budget);
    auto rng = mt19937_64(seed);
    return SSSPConfig(capper, rng);
}