#pragma once
#include "graph.hpp"
#include "scc.hpp"
#include "sputils.hpp"

// Shortest Path Algorithm Result State.
enum ShortestPathState {
    UNKNOWN,
    SHORTEST_PATH_TREE_FOUND,
    NEGATIVE_CYCLE_FOUND
};

// Shortest path tree witness is distance map.
template <typename T>
using ShortestPathTreeWitness = vector<T>;

// negative cycle edge witness is edge idx.
using NegativeCycleWitness = vector<size_t>;

template <typename T>
struct Witness {
    ShortestPathState state;
    ShortestPathTreeWitness<T> shortest_path_tree_witness;
    NegativeCycleWitness negative_cycle_witness;

    bool validate(Graph<T> &g) {
        switch ShortestPathState {
            case UNKNOWN:
            return false;
            case SHORTEST_PATH_TREE_FOUND:
            return validate_shortest_path_tree(g, shortest_path_tree_witness);
            case NEGATIVE_CYCLE_FOUND:
            return validate_negative_cycle(g, negative_cycle_witness);
        }
    }
};

template <typename T>
Witness<T> make_witness_for_sptree(const vector<T> &dist) {
    Witness<T> wt;
    wt.state = SHORTEST_PATH_TREE_FOUND;
    wt.shortest_path_tree_witness = dist;
    return wt;
}


// Shortest Path with result and witness per SCC.
template <typename T>
struct ShortestPathResult : SCCDecomposition<T> {
    vector<Witness<T>> witness_per_scc;

    ShortestPathResult(Graph<T> &g) : SCCDecomposition<T>(g) {
        witness_per_scc.resize(num_scc());
    }

    bool validate_per_scc() {
        size_t n = num_scc();
        
        for(size_t i = 0; i < n; i++) {
            if(!witness_per_scc[i].validate(scc_subgraphs[i])) return false;
        }

        return true;
    }
};