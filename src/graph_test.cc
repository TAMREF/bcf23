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

    g.add_edge(Edge<int>({0, 1, 2}));
    g.add_edge(Edge<int>({1, 2, 1}));
    g.add_edge(Edge<int>({2, 3, -1}));
    
    SECTION("add some edges") {

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

    SECTION("make a copy") {
        auto original_edge = vector<Edge<int>>(
            {
                {0, 1, 2},
                {1, 2, 1},
                {2, 3, -1}
            }
        );

        auto expected_edge = vector<Edge<int>>(
            {
                {1, 0, 2},
                {2, 1, 1},
                {3, 2, -1}
            }
        );

        auto gt = g.transpose();

        REQUIRE( original_edge == g.edges );
        REQUIRE( expected_edge == gt.edges );

        REQUIRE( gt.deg(0) == 0 );
        REQUIRE( gt.deg(1) == 1 );
        REQUIRE( gt.deg(2) == 1 );
        REQUIRE( gt.deg(3) == 1 );
    }
}