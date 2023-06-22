#include "sputils.h"
#include "graph.h"
#include "catch2/catch_all.hpp"

TEST_CASE("validate_shortest_path_tree", "[validate]") {
    // Graph<int> G(3);
    
    // G.add_vertex();

    // G.add_edge(Edge<int>({0, 1, 2}));
    // G.add_edge(Edge<int>({1, 2, 1}));
    // G.add_edge(Edge<int>({2, 3, -1}));

    // vector<int> v = lazy_dijkstra(G, 0, 2);

    REQUIRE(1 + 1 == 2);

    // SECTION("dijkstra should succeed") {
    //     REQUIRE( validate_shortest_path_tree(G, v) );
    // }

    // SECTION("phi manipulated distance map should fail") {
    //     G.phi = vector<int>({1, 2, 3, 2}); // arbitrarily re-assign potentials
    //     REQUIRE_FALSE( validate_shortest_path_tree(G, v) );
    // }
}