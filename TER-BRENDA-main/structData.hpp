#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <list>
using std::list;
using std::map;
using std::string;
using std::vector;

// Define the Espace class, representing a biochemical entity (e.g., enzyme, substrate, product)
class Espace
{
public:
    int indice;  // Index of the entity
    string name; // Name of the entity
    Espace(int indice, string name) : indice(indice), name(name){};
};

// Define the Reaction class, representing a biochemical reaction
class Reaction
{
public:
    Espace *enzyme;              // Pointer to the enzyme catalyzing the reaction
    Espace *substats[16];        // Array of pointers to substrates involved in the reaction
    int n_substats;              // Number of substrates involved in the reaction
    Espace *products[16];        // Array of pointers to products generated by the reaction
    int n_products;              // Number of products generated by the reaction
    Espace *units[16];           // Array of pointers to units corresponding to the concentrations
    int n_units;                 // Number of units for concentrations
    double *concentrations[16];  // Array of pointers to concentrations of substrates and products
    double Kcat;                 // Turnover number (Kcat) of the enzyme
    vector<Reaction *> suivants; // Vector of pointers to subsequent reactions in the pathway
};

// Define the Inhibition class, representing enzyme inhibition
class Inhibition
{
public:
    Espace *enzyme;                // Pointer to the enzyme being inhibited
    Espace *substats[1];           // Array of pointers to the substrate(s) causing inhibition
    int n_substats;                // Number of substrates causing inhibition
    Espace *units[1];              // Array of pointers to units corresponding to the concentrations
    int n_units;                   // Number of units for concentrations
    double *concentrations[1];     // Array of pointers to concentrations of inhibiting substrates
    vector<Inhibition *> suivants; // Vector of pointers to subsequent inhibitions in the pathway
};

// Define the Edge class, representing an edge between two reactions in the pathway
class Edge
{
public:
    vector<int> fromIndices; // Vector of indices of 'from' nodes in the edge
    vector<int> toIndices;   // Vector of indices of 'to' nodes in the edge
    void addFromIndice(int indice) { this->fromIndices.push_back(indice); };
    void addToIndice(int indice) { this->toIndices.push_back(indice); };
};

// Define the nodeFromTo structure, representing a pair of 'from' and 'to' nodes in the pathway
struct nodeFromTo
{
    vector<int> from; // Vector of indices of 'from' nodes
    vector<int> to;   // Vector of indices of 'to' nodes
};
