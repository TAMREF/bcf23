// single source sssp assuming no negative cycle.

# pragma once
#include "graph.hpp"
#include "config.hpp"
#include "rsssp.hpp"
#include "spresult.hpp"

// ceil div function with b > 0
template <typename T>
T __div_ceil(T a, T b) {
    if(a >= 0) return (a + b - 1) / b;
    return -( (-a) / b );
}

// one_step_scaling decreases the magnitude of negative weight by 2/3,
// if no negative cycle is given. It'll be inserted on 
// time budget is controlled by the capper.
template <typename T>
void one_step_scaling(
    Graph<T> &g,
    SSSPConfig &cfg
) {
    T min_weight = g.get_min_edge_weight();

    // no need to run on minw >= -3 graph
    if(min_weight >= -3) return;

    // generate restricted graph
    T W = (-min_weight) / 3 + 1;

    cerr << "W = " << W << endl;

    Graph<int> h = g;

    for(auto &e : h.edges) {
        e.w = __div_ceil(e.w, W) + 1;
    }

    cerr << h.get_min_edge_weight() << endl;

    Witness<T> wit = solve_rsssp(h, cfg);

    if(!wit.validate(h)) {
        assert(cfg.capper -> fail());
        return;
    }

    assert(wit.state == SHORTEST_PATH_TREE_FOUND);
    for(size_t i = 0; i < g.N(); i++) {
        g.phi[i] += W * (wit.shortest_path_tree_witness.pure_dist[i]);
    }
}

// sssp solves single source shortest path problem.
template <typename T>
Witness<T> sssp(
    Graph<T> &g,
    size_t src,
    SSSPConfig &cfg
) {
    // initial multiplication
    size_t n = g.N();
    Graph<T> h = g;

    T weight_mult = 4 * n;
    for(auto &phi : h.phi) {
        phi *= weight_mult;
    }
    for(auto &e : h.edges) {
        e.w *= weight_mult;
    }
    // scale before sssp
}