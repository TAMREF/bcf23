#pragma once
// makes scc from a graph.
#include "graph.hpp"
#include <vector>
#include <iostream>

using namespace std;
using SCCIndex = pair<size_t, size_t>; // (scc idx, idx of vertex in scc)

constexpr SCCIndex INVALID_SCC_INDEX = SCCIndex(-1, -1);

template <typename T>
struct SCCDecomposition {
    Graph<T> &g;

    // DAG storing inter-component edges only.
    Graph<T> inter_scc;
    // subgraphs for each SCC.
    vector<Graph<T>> scc_subgraphs;
    // subgraph vertex index -> original vertex index
    vector<vector<size_t>> vertex_up_map;
    vector<vector<size_t>> edge_up_map;

    // original vertex index -> (subgraph index, subgraph vertex index)
    vector<SCCIndex> vertex_down_map; // size: n
    vector<size_t> edge_down_map; // size: m

    SCCDecomposition(Graph<T> &_g) : g(_g), inter_scc(_g.N()) {
        _decompose();
    }

    size_t num_scc() {
        return scc_subgraphs.size();
    }

    // utility function to add a vertex into given scc group.
    SCCIndex _add_scc_vertex(size_t v, size_t scc_idx) {
        vertex_up_map[scc_idx].emplace_back(v);
        return SCCIndex(scc_idx, vertex_up_map[scc_idx].size() - 1);
    }

    bool in_same_scc(size_t v1, size_t v2) {
        return vertex_down_map[v1].first == vertex_down_map[v2].first;
    }

    SCCIndex get_edge_index(size_t edge_idx) {
        auto edge = g.edges[edge_idx];
        if( in_same_scc(edge.s, edge.e) ) return SCCIndex(vertex_down_map[edge.s].first, edge_down_map[edge_idx]);
        else return SCCIndex(INVALID_SCC_INDEX.first, edge_down_map[edge_idx]);
    }

    void _decompose() {
        auto n = g.N();

        // Initialize scc-related variables
        vertex_down_map.resize(n, INVALID_SCC_INDEX);
        size_t last_scc_idx = 0;

        // Kosaraju's algorithm for SCC
        vector<size_t> dt(n);
        vector<size_t> ft;
        size_t clk = 0;

        // temporary inverse-adjacency list. only stores vertex
        vector<vector<size_t>> radj(n);

        // forward step dfs
        function<void(size_t)> dfs = [&](size_t x) {
            dt[x] = ++clk;
            for(auto edge_idx : g.adj[x]) {
                assert(x == g.edges[edge_idx].s);
                auto y = g.edges[edge_idx].e;
                radj[y].emplace_back(x);

                if(!dt[y]) {
                    dfs(y);
                }
            }

            ft.push_back(x);
        };

        // backward step dfs
        function<void(size_t)> assign_scc = [&](size_t x) {
            vertex_down_map[x] = _add_scc_vertex(x, last_scc_idx);
            scc_subgraphs[last_scc_idx].add_vertex(T(0)); // override with zero potential

            for(auto y : radj[x]) {
                if(vertex_down_map[y] != INVALID_SCC_INDEX) continue;
                assign_scc(y);
            }
        };

        for(int i = 0; i < n; i++) {
            if(!dt[i]) dfs(i);
        }

        reverse(ft.begin(), ft.end());

        for(int i : ft) {
            if(vertex_down_map[i] == INVALID_SCC_INDEX) {
                // assign new scc subgraph and inverse mapping
                scc_subgraphs.emplace_back(0);
                vertex_up_map.emplace_back();
                assign_scc(i);
                ++last_scc_idx;
            }
        }

        for(auto edge : g.edges) {
            // compare subgraph index
            if(vertex_down_map[edge.s].first == vertex_down_map[edge.e].first) {
                // projected edge
                auto scc_idx = vertex_down_map[edge.s].first;

                scc_subgraphs[scc_idx].add_edge(
                    {
                        vertex_down_map[edge.s].second,
                        vertex_down_map[edge.e].second,
                        edge.w
                    }
                );

                edge_down_map.emplace_back(scc_subgraphs[scc_idx].M() - 1);
            } else {
                inter_scc.add_edge(edge);
                edge_down_map.emplace_back(inter_scc.M() - 1);
            }
        }
    }
};