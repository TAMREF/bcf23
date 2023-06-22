#include "catch2/catch_all.hpp"
#include <vector>
#include "graph.hpp"

TEST_CASE("graph_basic_tests", "[graph]") {
    Graph<int> g(size_t(3));

    SECTION("initialized graph") {
        REQUIRE(g.N() == 3);
    }

    g.add_vertex();

    SECTION("add a vertex") {
        REQUIRE(g.N() == 4); 
    }

    SECTION("add some edges") {
        g.add_edge(Edge<int>({0, 1, 2}));
        g.add_edge(Edge<int>({1, 2, 1}));
        g.add_edge(Edge<int>({2, 3, -1}));

        REQUIRE(g.deg(0) == 1);
        REQUIRE(g.deg(1) == 1);
        REQUIRE(g.deg(2) == 1);

        REQUIRE(g.M() == 3);
        REQUIRE(g.get_min_edge_weight() == -1);
        REQUIRE(g.get_weight(Edge<int>({0, 1, 2})) == 2);
    }

    SECTION("manipulated potential") {
        g.phi[0] = 1;
        REQUIRE(g.get_weight(Edge<int>({0, 1, 2})) == 3);
    }
}