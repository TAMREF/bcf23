// Restricted SSSP. Original from BCF23
// We can always assume there is a virtual source.
// We'll always employ all_source() variant of shortest path algorithms.
#pragma once

#include "config.hpp"
#include "spalgo.hpp"
#include "spresult.hpp"
#include "scc.hpp"

constexpr size_t LOW_KAPPA_LIMIT = 2;
constexpr long double BALL_ESTIMATION_ADDITIVE_ERROR = 0.125; // eps such that additive error <= eps * n
constexpr long double LIGHT_RATIO = 0.5 + 2 * BALL_ESTIMATION_ADDITIVE_ERROR;
constexpr size_t BALL_ESTIMATOR_SAMPLE_COEFF = 5 / (BALL_ESTIMATION_ADDITIVE_ERROR * BALL_ESTIMATION_ADDITIVE_ERROR);
constexpr long double RADIUS_TEMPERATURE = 20;

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

        if(!cfg.capper ->incr()) return vector<size_t>();
        auto dist = naive_dijkstra::single_source(g, v, true);

        for(size_t j = 0; j < n; j++) ball_counter[j] += (dist[j] <= kappa / 4);
    }

    vector<size_t> ret;
    for(size_t i = 0; i < n; i++) {
        // estimate real ball size with ball_counter * n / k.
        // ball_counter * n / k <= LIGHT_RATIO * n
        if(ball_counter[i] <= LIGHT_RATIO * k) ret.emplace_back(i);
    }

    return ret;
}

// internal function to solve rsssp.
template <typename T>
Witness<T> _solve_rsssp(
    Graph<T> &g,
    size_t kappa,
    SSSPConfig &cfg
) {
    // fail if g is not an scc.
    if(!g.is_scc) return Witness<T>();

    assert(g.is_restricted());

    cerr << "asdf\n";

    // capper on recursion depth.
    if(!cfg.capper -> incr()) return Witness<T>();

    cerr << "n = " << g.N() << ", kappa = " << kappa << '\n';

    // base case.
    if(g.N() <= 1 || kappa <= LOW_KAPPA_LIMIT) {
        auto dist = lazy_dijkstra::all_source(g, kappa, false, cfg.capper);
        // capper failure
        if(cfg.capper ->fail()) return Witness<T>();
        if( !validate_shortest_path_tree(g, dist) ) return Witness<T>();

        return make_witness_for_sptree(dist);
    }

    // recursion case.
    auto in_light_vertices = get_in_light_vertices(g, kappa, cfg);
    if(cfg.capper -> fail()) return Witness<T>();

    cerr << "in_light_verts: "; for(auto inv : in_light_vertices) cerr << inv << ' '; cerr << '\n';

    auto gt = g.transpose();
    auto out_light_vertices = get_in_light_vertices(gt, kappa, cfg);
    if(cfg.capper ->fail()) return Witness<T>();

    cerr << "out_light_verts: "; for(auto inv : out_light_vertices) cerr << inv << ' '; cerr << '\n';

    // initialize geometric sampler for radius
    geometric_distribution<T> radius_sampler(RADIUS_TEMPERATURE * log(g.N()) / kappa);

    g.enable_dels();
    gt.enable_dels();
    for(auto v : out_light_vertices) {
        if(g.deleted_vertex(v)) continue;

        T r = radius_sampler(cfg.rng);
        auto [ball, boundary] = naive_dijkstra::get_ball_and_boundary(g, v, r, cfg.capper);

        for(auto v : ball) {
            g.delete_vertex(v);
            gt.delete_vertex(v);
        }

        for(auto e : boundary) {
            g.delete_edge(e);
            gt.delete_edge(e);
        }
    }

    for(auto v : in_light_vertices) {
        if(g.deleted_vertex(v)) continue;

        T r = radius_sampler(cfg.rng);
        auto [ball, boundary] = naive_dijkstra::get_ball_and_boundary(gt, v, r, cfg.capper);

        for(auto v : ball) {
            g.delete_vertex(v);
            gt.delete_vertex(v);
        }

        for(auto e : boundary) {
            g.delete_edge(e);
            gt.delete_edge(e);
        }
    }

    cerr << "boundary removal done\n";

    // scc decomposition with edges deleted.
    g.delv.clear();
    gt.delv.clear();
    SCCDecomposition<T> S(g);

    cerr << "scc decomposition done, n_scc = " << S.num_scc() << '\n';;

    vector<Witness<T>> witness_by_scc;
    for(auto &scc : S.scc_subgraphs) {
        cerr << "scc size is " << scc.N() << '\n';
        // Case 1 : n decays.
        if(scc.N() <= LIGHT_RATIO * g.N()) {
            cerr << "case 1: small n\n";
            Witness<T> witness = _solve_rsssp(scc, kappa, cfg);
            cerr << "witness received with state = " << witness.state << "\n";
            if( cfg.capper -> fail() || !witness.validate(scc) ) {
                return Witness<T>();
            }
            witness_by_scc.emplace_back(witness);
        } 
        // Case 2 : kappa decays.
        else {
            cerr << "case 2: small kappa\n";
            Witness<T> witness = _solve_rsssp(scc, kappa / 2, cfg);
            if( cfg.capper ->fail() || !witness.validate(scc) ) {
                return Witness<T>();
            }
            witness_by_scc.emplace_back(witness);
        }
    }

    // update intra-SCC potential of g
    g.disable_dels();
    for(
        size_t scc_idx = 0, nscc = S.num_scc(); 
        scc_idx < nscc;
        scc_idx++
    ) {
        Witness<T> &wit = witness_by_scc[scc_idx];
        for(
            size_t v_scc = 0, n = S.scc_subgraphs[scc_idx].N();
            v_scc < n;
            v_scc++
        ) {
            size_t v_g = S.vertex_up_map[scc_idx][v_scc];
            g.phi[v_g] += wit.shortest_path_tree_witness[v_scc];
        }
    }

    // update DAG-potential of g
    for(
        size_t v_g = 0, n = g.N();
        v_g < n;
        v_g++
    ) {
        auto scc_idx = S.vertex_down_map[v_g].first;
        g.phi[v_g] -= T(scc_idx);
    }

    cerr << "potential adjusted\n";

    // lazy dijkstra with unlimited kappa. cap by capper
    auto dist = lazy_dijkstra::all_source(
        g,
        size_t(-1),
        false,
        cfg.capper
    );

    cerr << "concur step done\n";

    Witness<T> witness = make_witness_for_sptree(dist);

    if(cfg.capper ->fail() || !witness.validate(g)) {
        return Witness<T>();
    }

    return witness;
}

template <typename T>
Witness<T> solve_rsssp(Graph<T> g, SSSPConfig &cfg) {
    return _solve_rsssp(
        g,
        g.N(),
        cfg
    );
}