#include "catch2/catch_all.hpp"
#include <vector>
#include "graph.h"

TEST_CASE("graph_basic_tests", "[graph]") {
    Graph<int> g(size_t(3));

    REQUIRE(g.N() == 3);      

    g.add_vertex();

    // added a vertex
    REQUIRE(g.N() == 4); 

    g.add_edge(Edge<int>({0, 1, 2}));
    g.add_edge(Edge<int>({1, 2, 1}));
    g.add_edge(Edge<int>({2, 3, -1}));

    REQUIRE(g.M() == 3);
    REQUIRE(g.get_min_edge_weight() == -1);

}