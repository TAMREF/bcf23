// BCF23 depends on various 'terminate if does not work on time budget'.
// Here we replace wall clock with operation count.
#pragma once
#include <cstdlib>
#include <iostream>

using namespace std;

struct OperationCapper {
    virtual bool incr(size_t amount = 1) = 0;

    bool fail() {
        return !incr(0);
    }

    virtual ~OperationCapper() {}
};

struct NoCapOperationCapper : OperationCapper {
    bool incr(size_t amount = 1) override {
        return true;
    }

    ~NoCapOperationCapper() {
        cerr << "safely deconstructing nocap operator\n";
    }
};

struct NormalOperationCapper : OperationCapper {
    size_t counter, budget;
    NormalOperationCapper(size_t budget) : budget(budget), counter(0) {}

    bool incr(size_t amount = 1) override {
        bool ret = counter <= budget; // last mercy
        counter += amount;
        if(!ret) cerr << "!!!Capper failed!!!\n";
        return ret;
    }
};