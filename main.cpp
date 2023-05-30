#include <bits/stdc++.h>
#include "graph.h"

using namespace std;

int main() {
    int n = 4;
    
    Graph<int> G(4);

    G.add_edge(Edge<int>({0, 1, 2}));
    G.add_edge(Edge<int>({1, 2, 1}));
    G.add_edge(Edge<int>({2, 3, -1}));

    vector<int> v = G.dijkstra(0, true);

    for(int i : v) cout << i << ' ';
    cout << '\n';
}