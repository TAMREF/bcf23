#pragma once

#include <vector>
#include <queue>
using namespace std;

// Directional Edge with weight.
template <typename T>
struct Edge {
    size_t s, e; // s -> e
    T w;

    bool operator== (const Edge<T> &other) const {
        return s == other.s && e == other.e && w == other.w;
    }
};

// Graph struct with edge weights and vertex potential.
template <typename T>
struct Graph {
    vector<T> phi; // vertex-wide potential
    vector<Edge<T>> edges; // edges
    vector<vector<size_t>> adj, radj; // adjacency list
    bool is_scc; // original graph, or SCC?

    Graph(size_t n, bool is_scc = false) : phi(n), adj(n), radj(n), is_scc(is_scc) {}

    void add_vertex(T phi_value=0) {
        phi.emplace_back(phi_value);
        adj.emplace_back();
        radj.emplace_back();
    }

    void add_edge(const Edge<T> e) {
        assert(0 <= e.s && e.s < N()); // zero-based
        assert(0 <= e.e && e.e < N());

        edges.push_back(e);
        auto edge_idx =  M() -1;

        adj[e.s].push_back(edge_idx);
        radj[e.e].push_back(edge_idx);
    }

    inline size_t N() {
        return phi.size();
    }

    inline size_t M() {
        return edges.size();
    }

    inline size_t deg(size_t x) {
        return adj[x].size();
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

    Graph<T> transpose() {
        Graph<T> gt(*this);
        gt.adj.swap(gt.radj);

        for(auto &e : gt.edges) {
            swap(e.s, e.e);
        }

        return gt;
    }
};