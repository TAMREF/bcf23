#pragma once
#include "graph.hpp"
#include "scc.hpp"

// Shortest Path Algorithm Result State.
enum ShortestPathState {
    UNKNOWN,
    SHORTEST_PATH_TREE_FOUND,
    NEGATIVE_CYCLE_FOUND
};

// Shortest path tree witness is distance map.
template <typename T>
using ShortestPathTreeWitness = vector<T>;

// Advanced shortest path tree witness with parent edge
template <typename T>
struct ShortestPathTreeWitnessV2 {
    vector<T> dist, pure_dist;
    // pure_dist : ignored potential
    vector<size_t> parent_edge_idx;

    ShortestPathTreeWitnessV2(size_t n = 0) : dist(n), parent_edge_idx(n) {}
};

// negative cycle edge witness is edge idx.
using NegativeCycleWitness = vector<size_t>;

template <typename T>
struct Witness {
    ShortestPathState state;
    ShortestPathTreeWitnessV2<T> shortest_path_tree_witness;
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
Witness<T> make_witness_for_sptree(const ShortestPathTreeWitnessV2<T> &wit) {
    Witness<T> wt;
    wt.state = SHORTEST_PATH_TREE_FOUND;
    wt.shortest_path_tree_witness = wit;
    return wt;
}

// validate_shortest_path_distance_map validates
// if the shortest path tree (distance map) is valid.
// Once this is broken, it might indicate the existence of a negative cycle.
template <typename T>
bool validate_shortest_path_distance_map(
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

template <typename T>
bool validate_shortest_path_tree(
    Graph<T> &g,
    ShortestPathTreeWitnessV2<T> &wit
) {
    size_t n = g.N();
    vector<size_t> src;
    vector<vector<size_t>> child_edges(n);
    vector<T> dist = g.initial_dist();

    for(size_t i = 0; i < n; i++) {
        if(
            size_t edge_idx = wit.parent_edge_idx[i];
            edge_idx == size_t(-1)
        ) {
            if(wit.dist[i] != numeric_limits<T>::max()) {
                src.emplace_back(i);
            }
        } else {
            auto parent_v = g.edges[edge_idx].s;
            child_edges[parent_v].emplace_back(edge_idx);
        }
    }

    queue<size_t> q;

    for(size_t s : src) {
        dist[s] = T(0); // ignore potential
        q.emplace(s);
    }

    g.ignore_potential();

    while(!q.empty()) {
        int f = q.front(); q.pop();
        assert(dist[f] != numeric_limits<T>::max());
        for(auto ch : child_edges[f]) {
            auto e = g.edges[ch];
            dist[e.e] = dist[e.s] + g.get_weight(e);
        }
    }
    for(size_t i = 0; i < n; i++) {
        if(
            (wit.dist[i] == numeric_limits<T>::max())
            != (dist[i] == numeric_limits<T>::max()) // reachability does not agree
        ) {
            return false;
        }
    }

    bool ret = validate_shortest_path_distance_map(g, dist);
    g.regard_potential();
    wit.pure_dist = dist;
    return ret;
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