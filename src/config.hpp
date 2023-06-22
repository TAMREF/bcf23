#pragma once

#include "capper.hpp"
#include <random>

using namespace std;

// SSSPConfig manages universal config to solve SSSP, such as RNG or time budget.
struct SSSPConfig {
    OperationCapper &capper;
    mt19937_64 &rng;

    SSSPConfig(OperationCapper &capper, mt19937_64 &rng) : capper(capper), rng(rng) {}
};