// Restricted SSSP. Original from BCF23
// We can always assume there is a virtual source.
// We'll always employ all_source() variant of shortest path algorithms.
#pragma once

#include "config.hpp"
#include "spalgo.hpp"
#include "spresult.hpp"

constexpr size_t LOW_KAPPA_LIMIT = 2;
constexpr long double BALL_ESTIMATION_ADDITIVE_ERROR = 0.125; // eps such that additive error <= eps * n
constexpr long double LIGHT_RATIO = 0.5 + 2 * BALL_ESTIMATION_ADDITIVE_ERROR;
constexpr size_t BALL_ESTIMATOR_SAMPLE_COEFF = 5 / (BALL_ESTIMATION_ADDITIVE_ERROR * BALL_ESTIMATION_ADDITIVE_ERROR);
constexpr long double RADIUS_TEMPERATURE = 20;

// solve rsssp.
template <typename T>
Witness<T> _solve_rsssp(
    Graph<T> &g,
    size_t kappa,
    SSSPConfig &cfg
) {
    // fail if g is not an scc.
    if(!g.is_scc) return Witness<T>();

    // capper on recursion depth.
    if(!cfg.capper.incr()) return Witness<T>();

    // base case.
    if(g.N() <= 1 || kappa <= LOW_KAPPA_LIMIT) {
        auto dist = lazy_dijkstra::all_source(g, kappa, false, &cfg.capper);
        // capper failure
        if(cfg.capper.fail()) return Witness<T>();
        if( !validate_shortest_path_tree(g, dist) ) return Witness<T>();

        return make_witness_for_sptree(dist);
    }

    // recursion case.
    auto in_light_vertices = internal::get_in_light_vertices(g, kappa, cfg);
    if(cfg.capper.fail()) return Witness<T>();

    auto gt = g.transpose();
    auto out_light_vertices = internal::get_in_light_vertices(gt, kappa, cfg);
    if(cfg.capper.fail()) return Witness<T>();

    g.enable_dels();
    for(auto v : out_light_vertices) {
        if(g.delv[v]) continue;

        
    }
    g.disable_dels();
}

namespace internal {
    // List up in light vertices (in ball size <= 3n/4, estimated)
    // N >= 2 is assumed.
    template <typename T>
    vector<size_t> get_in_light_vertices(
        Graph<T> &g,
        size_t kappa,
        SSSPConfig &cfg
    ) {
        auto n = g.N();
        uniform_int_distribution<size_t> vertex_sampler(0, n - 1);
        size_t k = ceil(BALL_ESTIMATOR_SAMPLE_COEFF * log(n));

        vector<size_t> ball_counter(n);
        
        for(size_t i = 0; i < k; i++) {
            auto v = vertex_sampler(cfg.rng);

            if(!cfg.capper.incr()) return vector<size_t>();
            auto dist = naive_dijkstra::single_source(g, v, true);

            for(size_t j = 0; j < n; j++) ball_counter[j] += (dist[j] <= kappa / 4);
        }

        vector<size_t> ret;
        for(size_t i = 0; i < n; i++) {
            if(ball_counter[i] <= LIGHT_RATIO * k) ret.emplace_back(i);
        }

        return ret;
    }
}