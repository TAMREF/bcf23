#include "rsssp.hpp"
#include "graph.hpp"
#include "config.hpp"
#include "spresult.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("minimal example of restricted graph", "[rsssp]") {
    Graph<int> g(3);
    
    g.add_edge({1, 2, 1});
    g.add_edge({2, 0, -1});
    g.add_edge({0, 1, 1});

    g.is_scc = true;
    
    SSSPConfig cfg = NewTrivialConfig(0x33343);
    Witness<int> w = solve_rsssp(g, cfg);

    cerr << "witness state = " << w.state << endl;

    REQUIRE( w.validate(g) );
    REQUIRE( w.state == SHORTEST_PATH_TREE_FOUND );
    REQUIRE( w.shortest_path_tree_witness == vector<int>({
        1, 1, 0
    }));
}

TEST_CASE("should fail on negative cycle", "[rsssp]") {
    Graph<int> g(3);

    g.add_edge({0, 1, 0});
    g.add_edge({1, 2, 0});
    g.add_edge({2, 0, -1});

    g.is_scc = true;

    SSSPConfig cfg = NewCappedConfig(200);
    Witness<int> w = solve_rsssp(g, cfg);

    REQUIRE_FALSE( w.validate(g) );
    REQUIRE( w.state == UNKNOWN );
    REQUIRE( cfg.capper -> fail() );
}