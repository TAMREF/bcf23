#include "spresult.hpp"
#include "spalgo.hpp"
#include "graph.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("compare bellman ford and lazy dijkstra", "[validate]") {
    Graph<int> G(4);

    G.add_edge(Edge<int>({1, 0, -1}));
    G.add_edge(Edge<int>({2, 1, -1}));
    G.add_edge(Edge<int>({3, 2, -1}));

    ShortestPathTreeWitnessV2<int> v = lazy_dijkstra::single_source(G, 3, 3, false);
    ShortestPathTreeWitnessV2<int> w = bellman_ford::single_source(G, 3);

    SECTION("lazy dijkstra should succeed") {
        REQUIRE( validate_shortest_path_tree(G, v) );
    }

    SECTION("result should agree") {
        REQUIRE( v.dist == w.dist );
    }

    // insufficient loop
    ShortestPathTreeWitnessV2<int> y = lazy_dijkstra::single_source(G, 3, 1, false);
    SECTION("result should not agree") {
        REQUIRE( y.dist != w.dist );
    }
}

TEST_CASE("all source sssp", "[validate]") {
    Graph<int> g(3);

    g.add_edge({1, 2, 1});
    g.add_edge({2, 0, -1});
    g.add_edge({0, 1, 1});
    g.phi = vector<int>({-2, -1, 0});

    ShortestPathTreeWitnessV2<int> y = lazy_dijkstra::artificial_source(g, 10, false);
    
    REQUIRE( y.dist == vector<int>({1, 1, 0}) );
}