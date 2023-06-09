// Shortest Path algorithms.
// includes dijkstra, bellman_ford and lazy_dijkstra (BCF23 original)
#pragma once
#include "graph.hpp"
#include "spresult.hpp"
#include "capper.hpp"
#include <numeric>
#include <vector>

using namespace std;

namespace internal {
    // internal update function of dist map.
    // used by: dijkstra, lazy_dijkstra.
    // ignore negative edges by default.
    template <typename T, typename PairT = pair<T, size_t>>
    void relax_dijkstra_with_priority_queue(
        Graph<T> &g,
        priority_queue<PairT, vector<PairT>, greater<PairT>> &Q,
        ShortestPathTreeWitnessV2<T> &wit,
        OperationCapper *capper = nullptr
    ) {
        if(capper == nullptr) capper = new NoCapOperationCapper();

        if(!capper -> incr()) return;

        while(!Q.empty()) {
            auto [current_dist, current_vertex] = Q.top();
            Q.pop();

            if(current_dist != wit.dist[current_vertex]) continue;
            
            for(auto edge_idx : g.adj[current_vertex]) {
                if(g.deleted_edge(edge_idx)) continue;

                // skipped since edge is deleted
                auto edge = g.edges[edge_idx];
                
                // ignore negative edges
                if(g.get_weight(edge) < T(0)) continue;
                
                size_t next_vertex = edge.e;

                // skipped since next vertex is deleted
                if(g.deleted_vertex(next_vertex)) continue;
                
                // skipped due to useless relzaxation
                if(
                    wit.dist[next_vertex] 
                    <= wit.dist[current_vertex] + g.get_weight(edge)
                ) continue;
                
                // relax
                wit.dist[next_vertex] = wit.dist[current_vertex] + g.get_weight(edge);
                wit.parent_edge_idx[next_vertex] = edge_idx;

                Q.emplace(
                    wit.dist[next_vertex],
                    next_vertex
                );
            }
        }
    }
}

namespace naive_dijkstra {
    // Naive dijkstra with multiple sources.
    // @param ignore_negative_edges: if false, make error when negative edges are present.
    // otherwise, ignore negative edges and proceed dijkstra's algorithm only with non-negative edges.
    // @return The distance vector from src.
    template <typename T, typename PairT = pair<T, size_t>>
    ShortestPathTreeWitnessV2<T> multi_source(
        Graph<T> &g,
        vector<size_t> src, 
        bool ignore_negative_edges,
        OperationCapper *capper = nullptr
    ) {
        if(!ignore_negative_edges) {
            assert(g.get_min_edge_weight() >= T(0));
        }

        if(capper == nullptr) {
            capper = new NoCapOperationCapper();
        }

        ShortestPathTreeWitnessV2<T> wit(g.N());
        wit.dist = g.initial_dist();

        priority_queue<PairT, vector<PairT>, greater<PairT>> Q;
        for(auto s : src) {
            if(g.deleted_vertex(s)) continue;
            Q.emplace(wit.dist[s] = T(0), s);
        }

        internal::relax_dijkstra_with_priority_queue(g, Q, wit, capper);
        // skip capper validation here.

        return wit;
    }

    // Naive dijkstra with single source.
    // @param ignore_negative_edges: if false, make error when negative edges are present.
    // otherwise, ignore negative edges and proceed dijkstra's algorithm only with non-negative edges.
    // @return The distance vector from src.
    template <typename T>
    ShortestPathTreeWitnessV2<T> single_source(
        Graph<T> &g,
        size_t src, 
        bool ignore_negative_edges,
        OperationCapper *capper = nullptr
    ) {
        return multi_source(g, vector<size_t>({src}), ignore_negative_edges, capper);
    }

    // Naive dijkstra algorithm on non-negative edges.
    template <typename T, typename PairT = pair<T, size_t>>
    pair<
    vector<size_t>,
    vector<size_t>
    > get_ball_and_boundary(
        Graph<T> &g,
        size_t src,
        T r,
        OperationCapper *capper = nullptr
    ) {
        if(capper == nullptr) {
            capper = new NoCapOperationCapper();
        }

        vector<size_t> ball_vertices;
        vector<size_t> boundary_edge_candidates;

        // capper failure
        if(!capper->incr()) return pair(ball_vertices, boundary_edge_candidates);

        auto dist = g.initial_dist();


        priority_queue<PairT, vector<PairT>, greater<PairT>> Q;
        Q.emplace(
            dist[src] = 0,
            src
        );

        while(!Q.empty()) {
            auto [current_dist, current_vertex] = Q.top();
            Q.pop();

            if(current_dist != dist[current_vertex]) continue;
            if(current_dist > r) continue;
            ball_vertices.emplace_back(current_vertex);
            
            for(auto edge_idx : g.adj[current_vertex]) {
                if(g.deleted_edge(edge_idx)) continue;

                // skipped since edge is deleted
                auto edge = g.edges[edge_idx];
                
                // ignore negative edges
                if(g.get_weight(edge) < T(0)) continue;
                
                size_t next_vertex = edge.e;

                // skipped since next vertex is deleted
                if(g.deleted_vertex(next_vertex)) continue;
                
                if(dist[next_vertex] > dist[current_vertex] + g.get_weight(edge)) {
                    // relax
                    Q.emplace(
                        dist[next_vertex] = dist[current_vertex] + g.get_weight(edge),
                        next_vertex
                    );
                }

                // add boundary candidates
                if(dist[next_vertex] > r) {
                    boundary_edge_candidates.emplace_back(edge_idx);
                }
            }
        }

        vector<size_t> boundary_edges;
        for(auto edge_idx : boundary_edge_candidates) {
            auto edge = g.edges[edge_idx];
            if(dist[edge.s] <= r || dist[edge.e] > r) boundary_edges.emplace_back(edge_idx);
        }

        return pair(ball_vertices, boundary_edges);
    }
} // naive_dijkstra


namespace bellman_ford {
    // Bellman-ford algorithm with multiple sources.
    template <typename T>
    ShortestPathTreeWitnessV2<T> multi_source(
        Graph<T> &g,
        vector<size_t> src
    ) {
        ShortestPathTreeWitnessV2<T> wit(g.N());
        wit.dist = g.initial_dist();

        for(auto s : src) {
            wit.dist[s] = T(0);
        }

        auto n = g.N();
        for(int i = 0; i < n - 1; i++) {
            size_t edge_idx = 0;
            for(auto e : g.edges) {
                if(wit.dist[e.s] != numeric_limits<T>::max()) { 
                    // negative edge & e.s is reached out
                    if(wit.dist[e.e] > wit.dist[e.s] + g.get_weight(e)) {
                        wit.dist[e.e] = wit.dist[e.s] + g.get_weight(e);
                        wit.parent_edge_idx[e.e] = edge_idx;
                    }
                }
                ++edge_idx;
            }
        }
        return wit;
    }

    // bellman-ford algorithm with a single source.
    template <typename T>
    ShortestPathTreeWitnessV2<T> single_source(
        Graph<T> g,
        size_t src
    ) {
        return multi_source(g, vector<size_t>({src}));
    }

    // bellman-ford algorithm with all vertices as source.
    // distance map will be non-positive in this case.
    template <typename T>
    ShortestPathTreeWitnessV2<T> all_source(
        Graph<T> g
    ) {
        vector<size_t> src(g.N());
        iota(src.begin(), src.end(), 0);
        return multi_source(g, src);
    }
} // bellman_ford

namespace lazy_dijkstra {
    template <typename T, typename PairT = pair<T, size_t>>
    ShortestPathTreeWitnessV2<T> predetermined_initial_wit(
        Graph<T> &g,
        ShortestPathTreeWitnessV2<T> wit,
        size_t kappa,
        bool validate,
        OperationCapper *capper = nullptr
    ) {
        if(capper == nullptr) {
            capper = new NoCapOperationCapper();
        }
        priority_queue<PairT, vector<PairT>, greater<PairT>> Q;

        for(size_t i = 0; i < g.N(); i++) {
            Q.emplace(wit.dist[i], i);
        }

        for(size_t counter = 0; counter < kappa && !Q.empty(); counter++) {
            // dijkstra stage.
            internal::relax_dijkstra_with_priority_queue(g, Q, wit, capper);
            if(capper->fail()) return wit;

            // bellman-ford with negative edges.
            auto edge_idx = 0;
            for(auto e : g.edges) {
                if(g.get_weight(e) < T(0) && wit.dist[e.s] != numeric_limits<T>::max()) { 
                    // negative edge & e.s is reached out
                    if(wit.dist[e.e] > wit.dist[e.s] + g.get_weight(e)) {
                        wit.dist[e.e] = wit.dist[e.s] + g.get_weight(e);
                        wit.parent_edge_idx[e.e] = edge_idx;
                        Q.emplace(
                            wit.dist[e.e],
                            e.e
                        );
                    }
                }
                ++edge_idx;
            }
        }

        if( validate ) {
            assert( validate_shortest_path_tree(g, wit) );
        }

        return wit;
    }

    // lazy dijkstra with multiple sources. This is from BCF23.
    // Complexity: O(dijkstra * kappa)
    // kappa: guaranteed upper bound of loop iterations.
    template <typename T, typename PairT = pair<T, size_t>>
    ShortestPathTreeWitnessV2<T> multi_source(
        Graph<T> &g,
        vector<size_t> src,
        size_t kappa,
        bool validate,
        OperationCapper *capper = nullptr
    ) {
        priority_queue<PairT, vector<PairT>, greater<PairT>> Q;

        ShortestPathTreeWitnessV2<T> wit(g.N());
        wit.dist = g.initial_dist();

        for(auto s : src) {
            wit.dist[s] = T(0);
        }

        return predetermined_initial_wit(g, wit, kappa, validate, capper);
    }

    // lazy dijkstra with single source. This is from BCF23.
    // Complexity: O(dijkstra * kappa)
    // kappa: guaranteed upper bound of loop iterations.
    template <typename T, typename PairT = pair<T, size_t>>
    ShortestPathTreeWitnessV2<T> single_source(
        Graph<T> &g,
        size_t src,
        size_t kappa,
        bool validate,
        OperationCapper *capper = nullptr
    ) {
        return multi_source(g, vector<size_t>({src}), kappa, validate, capper);
    }

    // lazy dijkstra with all vertices as source. This is from BCF23.
    // Complexity: O(dijkstra * kappa)
    // kappa: guaranteed upper bound of loop iterations.
    template <typename T, typename PairT = pair<T, size_t>>
    ShortestPathTreeWitnessV2<T> all_source(
        Graph<T> &g,
        size_t kappa,
        bool validate,
        OperationCapper *capper = nullptr
    ) {
        vector<size_t> src(g.N());
        iota(src.begin(), src.end(), 0);
        return multi_source(g, src, kappa, validate, capper);
    }

    template <typename T>
    ShortestPathTreeWitnessV2<T> artificial_source(
        Graph<T> &g,
        size_t kappa,
        bool validate,
        OperationCapper *capper = nullptr
    ) {
        ShortestPathTreeWitnessV2<T> wit(g.N());
        wit.dist = g.phi;
        for(auto &v : wit.dist) v = -v;
        return predetermined_initial_wit(g, wit, kappa, validate, capper);
    }
} // lazy_dijkstra