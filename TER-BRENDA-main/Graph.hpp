#include <vector>
#include <list>
#include <iostream>
#include <queue>
#include <unordered_set>
using std::list;
using std::queue;
using std::unordered_set;
using std::vector;

class Graph
{
public:
    int nbNodes;
    vector<list<int>> listNode;

    Graph(int nbNodes) : nbNodes(nbNodes), listNode(nbNodes, list<int>()) {}

    void addEdge(int node, int to);

    void dfs(int entre, int sortie, unordered_set<int> &isVisited, vector<int> &paths, int distance, vector<vector<int>> &all_paths);
    vector<vector<int>> bfs(int entre, int sortie);
    vector<vector<int>> findPaths(int entre, int sortie, int distance);

    void dls(int entre, int sortie, unordered_set<int> &isVisited, vector<int> &paths, int distance, vector<vector<int>> &all_paths);
    vector<vector<int>> iddfs(int entre, int sortie, int distance);
};
