#include "sssp.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("test one-step scaler", "[sssp]") {
    Graph<int> g(20);

    mt19937 rng(0x4834);
    uniform_int_distribution<int> v(0, 19), wt(-300, 20);

    for(int i = 0; i < 30; i++) {
        size_t a = v(rng), b = v(rng);
        while(a == b) b = v(rng);
        if(a > b) swap(a, b);
    
        g.add_edge(Edge<int>({a, b, wt(rng)}));
    }

    SSSPConfig cfg(4000);

    cerr << "before scale: " << g.get_min_edge_weight() << endl;

    one_step_scaling(g, cfg);

    cerr << "after scale: " << g.get_min_edge_weight() << endl;

    REQUIRE(g.get_min_edge_weight() >= -200);
}