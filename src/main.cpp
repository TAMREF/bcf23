#include <vector>
#include <iostream>
#include "graph.h"
#include "sputils.h"

using namespace std;

int main() {
    int n = 4;
    
    Graph<int> G(4);

    G.add_edge(Edge<int>({0, 1, 2}));
    G.add_edge(Edge<int>({1, 2, 1}));
    G.add_edge(Edge<int>({2, 3, -1}));

    vector<int> v = lazy_dijkstra(G, 0, 2);

    for(int i : v) cout << i << ' ';
    cout << '\n';

    cout << "validation 1: " << validate_shortest_path_tree(G, v) << '\n';

    G.phi = vector<int>({1, 2, 3, 2}); // arbitrarily re-assign potentials
    cout << "validation 2: " << validate_shortest_path_tree(G, v) << '\n';
}