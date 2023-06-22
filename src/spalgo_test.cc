#include "sputils.h"
#include "spalgo.h"
#include "graph.h"
#include "catch2/catch_all.hpp"

TEST_CASE("compare bellman ford and lazy dijkstra", "[validate]") {
    Graph<int> G(4);

    G.add_edge(Edge<int>({1, 0, -1}));
    G.add_edge(Edge<int>({2, 1, -1}));
    G.add_edge(Edge<int>({3, 2, -1}));

    vector<int> v = lazy_dijkstra(G, 3, 3);
    vector<int> w = bellman_ford(G, 3);

    SECTION("lazy dijkstra should succeed") {
        REQUIRE( validate_shortest_path_tree(G, v) );
    }

    SECTION("result should agree") {
        REQUIRE( v == w );
    }

    // insufficient loop
    vector<int> y = lazy_dijkstra(G, 3, 1);
    SECTION("result should not agre") {
        REQUIRE(y != w);
    }
}