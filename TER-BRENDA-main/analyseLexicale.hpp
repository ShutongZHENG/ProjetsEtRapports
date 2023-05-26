#include "structData.hpp"
#include "Graph.hpp"
#include <iostream>
#include <fstream>
#include <tuple>
#include <sstream>
using std::cout;
using std::endl;
using std::ifstream;
using std::ostream;
using std::stringstream;

class analyseLexicale
{
private:
    string pathfile;
    void extract();
    void buildGraph();

public:
    // map<string, Espace *> espaces_reactions;
    map<string, Espace *> espaces;
    vector<Inhibition *> inhibitions;
    vector<Reaction *> reactions;
    vector<Reaction *> listReactions;
    vector<Inhibition *> listInhibitions;
    vector<Edge *> edges;
    Graph *graph;
    analyseLexicale(string pathfile);
    ~analyseLexicale();
    string getPathfile();
    void printListReactions()
    {
        for (auto r : listReactions)
            cout << r->enzyme->name << endl;
    };
    void printEdges()
    {
        for (auto e : edges)
        {
            cout << "from: ";
            for (auto i : e->fromIndices)
                cout << i << " ";
            cout << "to: ";
            for (auto i : e->toIndices)
                cout << i << " ";
            cout << endl;
        }
    };

    nodeFromTo getTraces(string entre, string sortie);
    void printTrace(nodeFromTo trace)
    {
        cout << "from: ";
        for (auto i : trace.from)
            cout << i << " ";
        cout << "to: ";
        for (auto i : trace.to)
            cout << i << " ";
        cout << endl;
    };
    vector<string> findPaths(int entre, int sortie, int distance);
    string printReaction(int indice);
    void verifierInhib();
};
ostream &operator<<(ostream &out, const analyseLexicale &a);
