// Restricted SSSP. Original from BCF23
// We can always assume there is a virtual source.
// We'll always employ all_source() variant of shortest path algorithms.
#pragma once

#include "config.hpp"
#include "spalgo.hpp"
#include "spresult.hpp"

constexpr size_t LOW_KAPPA_LIMIT = 2;

// solve rsssp.
template <typename T>
Witness<T> _solve_rsssp(
    Graph<T> &g,
    size_t kappa,
    SSSPConfig &cfg
) {
    // fail if g is not an scc.
    if(!g.is_scc) return Witness<T>();

    // base case.
    if(g.N() <= 1 || kappa <= LOW_KAPPA_LIMIT) {
        auto dist = lazy_dijkstra::all_source(g, kappa, false, &cfg.capper);
        // capper failure
        if(cfg.capper.fail()) return Witness<T>();
        if( !validate_shortest_path_tree(g, dist) ) return Witness<T>();

        return make_witness_for_sptree(dist);
    }

    // recursion case.
    auto [in_light_vertices, out_light_vertices] = internal::get_light_vertices(g, kappa, cfg);
}

namespace internal {
    template <typename T>
    pair<vector<size_t>, vector<size_t>> get_light_vertices(
        Graph<T> &g,
        size_t kappa,
        SSSPConfig &cfg
    ) {
        // 
    }
}