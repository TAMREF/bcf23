// Shortest Path algorithms.
// includes dijkstra, bellman_ford and lazy_dijkstra (BCF23 original)
#include "graph.h"
#include "sputils.h"
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
        vector<T> &dist
    ) {
        while(!Q.empty()) {
            auto [current_dist, current_vertex] = Q.top();
            Q.pop();

            if(current_dist != dist[current_vertex]) continue;
            
            for(auto edge_idx : g.adj[current_vertex]) {
                auto edge = g.edges[edge_idx];
                
                if(g.get_weight(edge) < T(0)) continue; // ignore negative edges
                
                size_t next_vertex = edge.e;
                
                if(dist[next_vertex] > dist[current_vertex] + g.get_weight(edge)) {
                    Q.emplace(
                        dist[next_vertex] = dist[current_vertex] + g.get_weight(edge),
                        next_vertex
                    );
                }
            }
        }
    }
}

// Naive dijkstra.
// @param ignore_negative_edges: if false, make error when negative edges are present.
// otherwise, ignore negative edges and proceed dijkstra's algorithm only with non-negative edges.
// @return The distance vector from src.
template <typename T, typename PairT = pair<T, size_t>>
vector<T> dijkstra(
    Graph<T> &g,
    size_t src, 
    bool ignore_negative_edges
) {
    if(!ignore_negative_edges) {
        assert(g.get_min_edge_weight() >= T(0));
    }

    vector<T> dist = g.initial_dist();
    priority_queue<PairT, vector<PairT>, greater<PairT>> Q;
    Q.emplace(dist[src] = T(0), src);

    internal::relax_dijkstra_with_priority_queue(g, Q, dist);

    return dist;
}

// bellman-ford algorithm. This should be ground-truth.
// TODO : detect negative cycle.
template <typename T>
vector<T> bellman_ford(
    Graph<T> &g,
    size_t src
) {
    vector<T> dist = g.initial_dist();
    dist[src] = T(0);

    auto n = g.N();
    for(int i = 0; i < n - 1; i++) {
        for(auto e : g.edges) {
            if(dist[e.s] != numeric_limits<T>::max()) { 
                // negative edge & e.s is reached out
                dist[e.e] = min(
                    dist[e.e],
                    dist[e.s] + g.get_weight(e)
                );
            }
        }
    }
    return dist;
}

// lazy dijkstra. This is from BCF23.
// Complexity: O(dijkstra * kappa)
// kappa: guaranteed upper bound of loop iterations.
template <typename T, typename PairT = pair<T, size_t>>
vector<T> lazy_dijkstra(
    Graph<T> &g,
    size_t src,
    size_t kappa,
    bool validate=false
) {
    priority_queue<PairT, vector<PairT>, greater<PairT>> Q;

    vector<T> dist = g.initial_dist();

    Q.emplace(dist[src] = T(0), src);
    for(size_t counter = 0; counter < kappa && !Q.empty(); counter++) {

        // dijkstra stage.
        internal::relax_dijkstra_with_priority_queue(g, Q, dist);

        // bellman-ford with negative edges.
        for(auto e : g.edges) {
            if(g.get_weight(e) < T(0) && dist[e.s] != numeric_limits<T>::max()) { 
                // negative edge & e.s is reached out
                if(dist[e.e] > dist[e.s] + g.get_weight(e)) {
                    Q.emplace(
                        dist[e.e] = dist[e.s] + g.get_weight(e),
                        e.e
                    );
                }
            }
        }
    }

    if( validate ) {
        assert( validate_shortest_path_tree(g, dist) );
    }

    return dist;
}