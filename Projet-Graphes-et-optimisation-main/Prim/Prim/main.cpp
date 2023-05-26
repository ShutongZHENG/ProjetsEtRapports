//
//  main.cpp
//  Prim
//
//  Created by ZHENG Shutong on 2022/10/10.
//

#include <iostream>
#include "Graph.hpp"
#include <vector>
using namespace std;
int main(int argc, const char * argv[]) {
  
    Graph graph;
    
    
    graph.addEdge(Edge("T1","T2",1));
    graph.addEdge(Edge("T1","S2",2));
    graph.addEdge(Edge("T1","S1",2));
    graph.addEdge(Edge("T1","T7",2));
    
    graph.addEdge(Edge("T2","T1",1));
    graph.addEdge(Edge("T2","S2",1));
    graph.addEdge(Edge("T2","T3",2));
    graph.addEdge(Edge("T2","S1",3));
    
    graph.addEdge(Edge("T3","T2",2));
    graph.addEdge(Edge("T3","S2",3));
    graph.addEdge(Edge("T3","T4",3));
    graph.addEdge(Edge("T3","S3",1));
    
    graph.addEdge(Edge("T4","S2",4));
    graph.addEdge(Edge("T4","S3",5));
    graph.addEdge(Edge("T4","T3",3));
    graph.addEdge(Edge("T4","T5",4));
    
    graph.addEdge(Edge("T5","S3",2));
    graph.addEdge(Edge("T5","T4",4));
    graph.addEdge(Edge("T5","T6",3));
    
    graph.addEdge(Edge("T6","T5",3));
    graph.addEdge(Edge("T6","S3",4));
    graph.addEdge(Edge("T6","S1",3));
    graph.addEdge(Edge("T6","T7",1));
    
    graph.addEdge(Edge("T7","T6",1));
    graph.addEdge(Edge("T7","S1",4));
    graph.addEdge(Edge("T7","T1",2));
 
    
    
    for(int i = 0 ; i< graph.nodes.size() ; i++)
        cout<<graph.nodes[i].name<< " ";
    
    cout<<endl<<"before calculating, miniTree size: "<< graph.miniTree.size()<<endl;
    graph.makeMiniTree();
    cout<<endl<<"after calculating, miniTree size: "<< graph.miniTree.size()<<endl;
    graph.makeMiniTree_Economic();
    
    cout<<endl<<endl<<endl<<endl<<"calculate by Kruskal"<<endl;
    graph.makeMiniTree_Kruskal();
    return 0;
}
