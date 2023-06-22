#pragma once

#include <vector>
#include <iostream>
#include "graph.hpp"

using namespace std;

// validate_shortest_path_tree validates
// if the shortest path tree (distance map) is valid.
// Once this is broken, it might indicate the existence of a negative cycle.
template <typename T>
bool validate_shortest_path_tree(
    Graph<T> &g,
    vector<T> &dist, 
    bool ignore_negative_edges=false
) {
    for(auto e : g.edges) {
        if(ignore_negative_edges && g.get_weight(e) < T(0)) {
            continue;
        }

        if(dist[e.s] != numeric_limits<T>::max() && dist[e.e] > dist[e.s] + g.get_weight(e)) {
            cerr << "dist[" << e.s << "] = " 
            << dist[e.s] << " -> dist[" << e.e << "] = " << dist[e.e] 
            << ", but there is an edge with weight " 
            << g.get_weight(e) << " violating the shortest path tree\n";
            return false;
        }
    }
    return true;
}

// validate_negative_cycle validates if negative cycle is truly negative.
template <typename T>
bool validate_negative_cycle(
    Graph<T> &g,
    vector<size_t> &negative_cycle
) {
    if(negative_cycle.empty()) {
        return false;
    }

    T weight = 0;
    size_t previous_end = g.edges[negative_cycle.back()].e;
    for(auto edge_idx : negative_cycle) {
        auto edge = g.edges[edge_idx];

        assert(previous_end == edge.s);
        previous_end = edge.e;

        weight += edge.w;
    }

    return weight < 0;
}