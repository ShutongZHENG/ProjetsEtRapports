//
//  Edge.hpp
//  Prim
//
//  Created by ZHENG Shutong on 2022/10/10.
//

#ifndef Edge_hpp
#define Edge_hpp

#include <stdio.h>
#include <string>
using namespace std;
class Edge{
private:
    string from;
    string to;
    int weight;
    
public:
    Edge(string from, string to, int weight);
    string getFrom();
    string getTo();
    int getWeight();
    
    
};

#endif /* Edge_hpp */
