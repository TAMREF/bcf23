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
        switch (state) {
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