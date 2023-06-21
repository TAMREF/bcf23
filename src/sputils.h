#pragma once

#include <vector>
#include "graph.h"

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

    return dist;
}

// lazy dijkstra. This is from BCF23.
// Complexity: O(dijkstra * kappa)
// kappa: guaranteed number of loops.
template <typename T, typename PairT = pair<T, size_t>>
vector<T> lazy_dijkstra(
    Graph<T> &g,
    size_t src,
    size_t kappa
) {
    priority_queue<PairT, vector<PairT>, greater<PairT>> Q;

    vector<T> dist = g.initial_dist();

    Q.emplace(dist[src] = T(0), src);
    for(size_t counter = 0; counter < kappa && !Q.empty(); counter++) {

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

    return dist;
}