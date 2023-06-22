#include "sputils.hpp"
#include "spalgo.hpp"
#include "graph.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("validate_shortest_path_tree", "[validate]") {
    Graph<int> G(4);

    G.add_edge(Edge<int>({0, 1, 2}));
    G.add_edge(Edge<int>({1, 2, 1}));
    G.add_edge(Edge<int>({2, 3, -1}));

    vector<int> v = lazy_dijkstra::single_source(G, 0, 2, false);

    SECTION("dijkstra should succeed") {
        REQUIRE( validate_shortest_path_tree(G, v) );
    }

    SECTION("phi manipulated distance map should fail") {
        G.phi = vector<int>({1, 2, 3, 2}); // arbitrarily re-assign potentials
        REQUIRE_FALSE( validate_shortest_path_tree(G, v) );
    }
}

TEST_CASE("validate_negative_cycle", "[validate]") {
    Graph<int> G(3);

    G.add_edge(Edge<int>({0, 1, 2}));
    G.add_edge(Edge<int>({2, 0, -1}));
    G.add_edge(Edge<int>({1, 2, -3}));

    vector<size_t> neg_cycle({0, 2, 1});

    REQUIRE( validate_negative_cycle(G, neg_cycle) );

    G.edges[2].w = 3;
    REQUIRE_FALSE( validate_negative_cycle(G, neg_cycle) );
}