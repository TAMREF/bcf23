#include "scc.hpp"
#include "graph.hpp"
#include "catch2/catch_all.hpp"

using namespace std;

TEST_CASE("scc for test", "[scc]") {
    Graph<int> g(7);
    
    // scc #1
    g.add_edge({0, 1, 0});
    g.add_edge({1, 2, 0});
    g.add_edge({2, 0, 0});

    // scc #2
    g.add_edge({5, 4, 0});
    g.add_edge({4, 3, 0});
    g.add_edge({3, 5, 0});

    // inter-component edges
    g.add_edge({1, 3, 0});
    g.add_edge({1, 4, 0});
    g.add_edge({0, 5, 0});

    REQUIRE(g.N() == 7);
    REQUIRE(g.M() == 9);

    SCCDecomposition<int> S(g);

    REQUIRE(S.num_scc() == 3);

    REQUIRE(S.inter_scc.N() == 7);
    REQUIRE(S.inter_scc.M() == 3);
    
    auto expected_scc_indices = vector<SCCIndex>({
        {1, 0},
        {1, 2},
        {1, 1},
        {2, 0},
        {2, 1},
        {2, 2},
        {0, 0},
    });

    REQUIRE( expected_scc_indices == S.vertex_down_map );

    auto expected_scc_to_original = vector<vector<size_t>>({
        {6},
        {0, 2, 1},
        {3, 4, 5}
    });

    REQUIRE(expected_scc_to_original == S.vertex_up_map);

    REQUIRE(S.scc_subgraphs[0].N() == 1);
    REQUIRE(S.scc_subgraphs[0].M() == 0);

    REQUIRE(S.scc_subgraphs[1].N() == 3);
    REQUIRE(S.scc_subgraphs[1].M() == 3);

    REQUIRE(S.scc_subgraphs[2].N() == 3);
    REQUIRE(S.scc_subgraphs[2].M() == 3);

    // validate get_edge_index
    REQUIRE(S.get_edge_index(0) == SCCIndex(1, 0));
    REQUIRE(S.get_edge_index(8) == SCCIndex(-1, 2));
}