#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <queue>
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

    inline vector<T> initial_dist() {
        return vector<T>(N(), numeric_limits<T>::max());
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

        vector<T> dist = initial_dist();

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
        using PairT = pair<T, size_t>;
        priority_queue<PairT, vector<PairT>, greater<PairT>> Q;

        vector<T> dist = initial_dist();

        Q.emplace(dist[src] = T(0), src);
        for(size_t counter = 0; counter < kappa && !Q.empty(); counter++) {

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

            for(auto e : edges) {
                if(get_weight(e) < T(0) && dist[e.s] != numeric_limits<T>::max()) { 
                    // negative edge & e.s is reached out
                    if(dist[e.e] > dist[e.s] + get_weight(e)) {
                        Q.emplace(
                            dist[e.e] = dist[e.s] + get_weight(e),
                            e.e
                        );
                    }
                }
            }
        }

        return dist;
    }

    // validate if the shortest path tree (distance profile) is valid.
    // Once this is broken, it might indicate the existence of a negative cycle.
    bool validate_shortest_path_tree(vector<T> &dist, bool ignore_negative_edges=false) {
        for(auto e : edges) {
            if(ignore_negative_edges && get_weight(e) < T(0)) {
                continue;
            }

            if(dist[e.s] != numeric_limits<T>::max() && dist[e.e] > dist[e.s] + get_weight(e)) {
                cerr << "dist[" << e.s << "] = " << dist[e.s] << " -> dist[" << e.e << "] = " << dist[e.e] 
                << ", but there is an edge with weight " << get_weight(e) << "violating the shortest path tree\n";
                return false;
            }
        }
        return true;
    }
};

#endif