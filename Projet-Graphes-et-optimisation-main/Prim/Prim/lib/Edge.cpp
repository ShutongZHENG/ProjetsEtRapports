//
//  Edge.cpp
//  Prim
//
//  Created by ZHENG Shutong on 2022/10/10.
//

#include "Edge.hpp"

Edge::Edge(string from, string to, int weight){
    this->from = from;
    this->to = to;
    this->weight = weight;
}

string Edge::getTo(){
    return to;
}

string Edge::getFrom(){
    return from;
}

int Edge::getWeight(){
    return weight;
}
