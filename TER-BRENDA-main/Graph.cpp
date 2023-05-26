#include "Graph.hpp"

// Function to add an edge to the graph
void Graph::addEdge(int node, int to)
{
    listNode[node].push_back(to);
}

// Depth-First Search (DFS) algorithm to find paths
void Graph::dfs(int entre, int sortie, unordered_set<int> &isVisited, vector<int> &paths, int distance, vector<vector<int>> &all_paths)
{

    if (paths.size() <= distance)
    {
        isVisited.insert(entre);
        paths.push_back(entre);

        if (entre == sortie && paths.size() == distance)
        {
            all_paths.push_back(paths);
            std::cout << "paths: " << paths[0] << " " << paths[1] << " " << paths[2] << " " << paths[3] << std::endl;
        }
        else
        {
            for (int i : listNode[entre])
            {
                if (paths.size() == 1)
                    std::cout << "path: " << paths[0] << "    i = " << i << std::endl;
                if (paths.size() == 2)
                    std::cout << "path: " << paths[0] << " " << paths[1] << "    i = " << i << std::endl;
                if (paths.size() == 3)
                    std::cout << "path: " << paths[0] << " " << paths[1] << " " << paths[2] << "    i = " << i << std::endl;
                if (isVisited.find(i) == isVisited.end())
                {

                    // std::cout<<"isVisited.find(i) == isVisited.end()"<<std::endl;
                    // std::cout<<"i = "<<i<<std::endl;

                    dfs(i, sortie, isVisited, paths, distance, all_paths);
                }
            }
        }

        isVisited.erase(entre);
        paths.pop_back();
    }
}

// Breadth-First Search (BFS) algorithm to find paths
vector<vector<int>> Graph::bfs(int entre, int sortie)
{
    vector<vector<int>> all_paths;
    queue<vector<int>> q;
    vector<int> path;
    path.push_back(entre);
    q.push(path);

    while (!q.empty())
    {
        path = q.front();
        q.pop();
        int last_node = path.back();

        if (last_node == sortie)
        {
            all_paths.push_back(path);
        }
        else
        {
            for (int i : listNode[last_node])
            {
                if (std::find(path.begin(), path.end(), i) == path.end())
                {
                    vector<int> new_path(path);
                    new_path.push_back(i);
                    q.push(new_path);
                }
            }
        }
    }

    return all_paths;
}

// Find paths with given distance
vector<vector<int>> Graph::findPaths(int entre, int sortie, int distance)
{
    //    unordered_set<int> isVisited;
    //    vector<int> paths;
    //    vector<vector<int> > all_paths;
    //    dfs(entre, sortie, isVisited, paths, distance, all_paths);
    //    return all_paths;
    return iddfs(entre, sortie, distance);
}

// Depth-Limited Search (DLS) algorithm to find paths
void Graph::dls(int entre, int sortie, unordered_set<int> &isVisited, vector<int> &paths, int distance, vector<vector<int>> &all_paths)
{

    if (distance == 0 && entre == sortie)
    {
        paths.push_back(entre);
        all_paths.push_back(paths);
        paths.pop_back();
        return;
    }

    if (distance > 0)
    {
        isVisited.insert(entre);
        paths.push_back(entre);

        for (int i : listNode[entre])
        {
            if (isVisited.find(i) == isVisited.end())
            {
                dls(i, sortie, isVisited, paths, distance - 1, all_paths);
            }
        }

        isVisited.erase(entre);
        paths.pop_back();
    }
}

// Iterative Deepening Depth-First Search (IDDFS) algorithm to find paths
vector<vector<int>> Graph::iddfs(int entre, int sortie, int distance)
{
    unordered_set<int> isVisited;
    vector<int> paths;
    vector<vector<int>> all_paths;

    for (int i = 0; i <= distance; i++)
    {
        dls(entre, sortie, isVisited, paths, i, all_paths);
    }

    return all_paths;
}
