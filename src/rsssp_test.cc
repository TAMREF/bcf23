#include "rsssp.hpp"
#include "graph.hpp"
#include "config.hpp"
#include "spresult.hpp"
#include "catch2/catch_all.hpp"
#include <random>

TEST_CASE("minimal example of restricted graph", "[rsssp]") {
    Graph<int> g(3);
    
    g.add_edge({1, 2, 1});
    g.add_edge({2, 0, -1});
    g.add_edge({0, 1, 1});

    g.is_scc = true;
    
    SSSPConfig cfg(-1, 0x33343);
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

    SSSPConfig cfg(200);
    Witness<int> w = solve_rsssp(g, cfg);

    REQUIRE_FALSE( w.validate(g) );
    REQUIRE( w.state == UNKNOWN );
    REQUIRE( cfg.capper -> fail() );
}

// gen_path makes random path graph with random weight.
Graph<int> gen_path(size_t n) {
    Graph<int> g(n);
    vector<size_t> v(n);
    mt19937 rng(0x123123);

    iota(v.begin(), v.end(), 0);
    shuffle(v.begin(), v.end(), rng);

    uniform_int_distribution<int> wt(-1, 100);
    for(size_t i = 0; i < n - 1; i++) {
        g.add_edge(Edge<int>({v[i], v[i+1], wt(rng)}));
    }

    return g;
}

TEST_CASE("simple but big case - path", "[rsssp]") {
    const size_t n = 15;

    Graph<int> g = gen_path(n);

    SSSPConfig cfg(size_t(1600 * log(n)));

    Witness<int> wit = solve_rsssp(g, cfg);

    REQUIRE(wit.state == SHORTEST_PATH_TREE_FOUND);
    REQUIRE(wit.validate(g));
}

// gen_wheel makes random path graph with random weight.
Graph<int> gen_wheel(size_t n) {
    Graph<int> g(n);
    vector<size_t> v(n);
    mt19937 rng(0x123123);

    iota(v.begin(), v.end(), 0);
    shuffle(v.begin(), v.end(), rng);

    uniform_int_distribution<int> wt(-1, n / 8);

    for(size_t i = 1; i < n; i++) {
        g.add_edge(Edge<int>({v[0], v[i], wt(rng)}));
    }
    
    for(size_t i = 1; i < n-1; i++) {
        g.add_edge(Edge<int>({v[i], v[i+1], wt(rng)}));
    }

    g.add_edge(Edge<int>({v[n-1], v[1], wt(rng)}));

    return g;
}

TEST_CASE("simple but big case - wheel", "[rsssp]") {
    const size_t n = 15;

    Graph<int> g = gen_wheel(n);

    SSSPConfig cfg(size_t(1600 * log(n)));

    Witness<int> wit = solve_rsssp(g, cfg);

    REQUIRE( wit.state == SHORTEST_PATH_TREE_FOUND );
    REQUIRE( wit.validate(g) );
}