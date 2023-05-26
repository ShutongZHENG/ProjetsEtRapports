//
//  Graph.hpp
//  Prim
//
//  Created by ZHENG Shutong on 2022/10/10.
//

#ifndef Graph_hpp
#define Graph_hpp

#include <iostream>
#include <vector>
#include <deque>
#include "Edge.hpp"

using namespace std;

struct Node{
    string name;
    bool isSelected;
    int minWeight;
    string parent;
    int degree = 0;
    
};



class Graph{
public:
    
    vector<Edge> edges;
    vector<Edge> miniTree;
    vector<Node> nodes;
    vector<vector<Edge> > miniTree_Economic;
    vector<Node> nodes_t;
    vector<Node> nodes_s;
    void addEdge(Edge e);
    int nbrEdges();
    void makeMiniTree();
    void makeMiniTree_Economic();
    void makeMiniTree_Kruskal();


    
    
private:
    vector<Edge> getAdjacentEdgeList(Node node);
    void sortEdges();
    bool allSelected();
    int getMiniWeightNode();
    void calculMiniWeight(int index, vector<Edge> childrenEdges);
    void printTablePrim();
    void sortMiniTree();
    bool allTtoS(vector<Edge> edges);
    bool sous_allTtoS(Node node, vector<Edge> edges);
    bool isCycle(vector<Edge> edges);
    

    
};

#endif /* Graph_hpp */

