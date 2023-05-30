#ifndef GRAPH_H
#define GRAPH_H

#include <bits/stdc++.h>
using namespace std;

// Directional Edge
template <typename T>
struct Edge {
    size_t s, e; // s -> e
    T w;
};

template <typename T>
struct Graph {
    vector<T> phi; // vertex-wide potential
    vector<Edge<T>> edges; // edges
    vector<vector<size_t>> adj; // adjacency list

    Graph(size_t n) : phi(n), adj(n) {}

    void add_edge(const Edge<T> e) {
        assert(0 <= e.s && e.s < N()); // zero-based
        assert(0 <= e.e && e.e < N());

        edges.push_back(e);
        auto edge_idx =  M() -1;

        adj[e.s].push_back(edge_idx);
        adj[e.e].push_back(edge_idx);
    }

    inline size_t N() {
        return phi.size();
    }

    inline size_t M() {
        return edges.size();
    }

    T get_weight(const Edge<T> &e) {
        return e.w + phi[e.s] - phi[e.e];
    }

    T get_min_edge_weight() {
        auto it = min_element(edges.begin(), edges.end(), [&](const Edge<T> &e, const Edge<T> &f) {
            return get_weight(e) < get_weight(f);
        });
        return get_weight(*it);
    }

    // Naive dijkstra.
    // @param ignore_negative_edges: if false, make error when negative edges are present.
    // otherwise, ignore negative edges and proceed dijkstra's algorithm only with non-negative edges.
    // @return The distance vector from src.
    vector<T> dijkstra(size_t src, bool ignore_negative_edges) {
        if(!ignore_negative_edges) {
            assert(get_min_edge_weight() >= T(0));
        }

        vector<T> dist(N(), numeric_limits<T>::max());

        using PairT = pair<T, size_t>;
        priority_queue<PairT, vector<PairT>, greater<PairT>> Q;
        Q.emplace(dist[src] = T(0), src);

        while(!Q.empty()) {
            auto [current_dist, current_vertex] = Q.top();
            Q.pop();

            if(current_dist != dist[current_vertex]) continue;
            
            for(auto edge_idx : adj[current_vertex]) {
                auto edge = edges[edge_idx];
                
                if(get_weight(edge) < T(0)) continue; // ignore negative edges
                
                size_t next_vertex = edge.e;
                
                if(dist[next_vertex] > dist[current_vertex] + get_weight(edge)) {
                    Q.emplace(
                        dist[next_vertex] = dist[current_vertex] + get_weight(edge),
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
    vector<T> lazy_dijkstra(size_t src, size_t kappa) {
        assert(false); // TBD
        return vector<T>(N());
    }
};

#endif