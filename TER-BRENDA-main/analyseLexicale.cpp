#include "analyseLexicale.hpp"

// Split a string according to a delimiter and return a vector of substrings
vector<string> split(string str, string delimiter)
{
    vector<string> res;
    stringstream ss(str);
    string tmp;
    size_t pos = 0;

    while ((pos = str.find(delimiter)) != std::string::npos)
    {
        tmp = str.substr(0, pos);
        res.push_back(tmp);
        str.erase(0, pos + delimiter.length());
    }
    res.push_back(str);

    return res;
}
// Extract the content between the first two quotes of a string
string extractContent(string str)
{
    int pos1 = str.find("\"");
    int pos2 = str.find("\"", pos1 + 1);
    string res = str.substr(pos1 + 1, pos2 - pos1 - 1);
    return res;
}
// Constructor with pathfile argument
analyseLexicale::analyseLexicale(string pathfile)
{
    this->pathfile = pathfile;
    this->extract(); // Extract data from the input file
    cout << "extract finished" << endl;
    verifierInhib();

    this->buildGraph(); // Build the graph structure of the model
    graph = new Graph(edges.size());
    for (int i = 0; i < edges.size(); i++)
    {
        /* code */
        for (int e : edges[i]->toIndices)
        {
            if (e == -1)
                break;
            graph->addEdge(i, e);
        }
    }
    cout << "build graph finished" << endl;
}

analyseLexicale::~analyseLexicale()
{
}
// Getter for the pathfile attribute
string analyseLexicale::getPathfile()
{
    return this->pathfile;
}
// Extract data from the input file and initialize the reactions and inhibitions vectors
void analyseLexicale::extract()
{
    bool isReaction = true;
    ifstream file(this->pathfile, std::ios::in);
    string line;
    int indice = -1;
    while (getline(file, line))
    {

        if (line == "// Inhibitions")
        {
            isReaction = false;
            continue;
        }
        if (line.size() == 0 || line == "// Reactions")
        {
            continue;
        }
        else
        {

            if (isReaction)
            {

                Reaction *reaction = new Reaction();
                // 分割 " : "
                vector<string> tmp;
                string str_enzyme;
                string str_units;
                string str_reac;
                string str_products;

                tmp = split(line, " : ");
                str_enzyme = tmp[0];

                // 分割 " | "
                tmp = split(tmp[1], " | ");
                str_reac = tmp[0];
                tmp = split(tmp[1], ";");
                str_units = tmp[0];

                // 分割 " -> "
                tmp = split(str_reac, " -> ");
                str_reac = tmp[0];
                str_products = tmp[1];

                // 分割 " + " 传入substats
                if (str_reac == "")
                {
                    reaction->n_substats = 0;
                }
                else
                {
                    tmp = split(str_reac, " + ");
                    reaction->n_substats = tmp.size();
                    for (int i = 0; i < tmp.size(); i++)
                        reaction->substats[i] = new Espace(i, extractContent(tmp[i]));
                }

                // 分割 " + " 传入products
                tmp = split(str_products, " + ");
                reaction->n_products = tmp.size();
                for (int i = 0; i < tmp.size(); i++)
                    reaction->products[i] = new Espace(i, extractContent(tmp[i]));
                // 分割 " - " 传入kcat

                tmp = split(str_units, " - ");

                if (tmp[0] == "")
                {
                    reaction->Kcat = stod(tmp[1]);
                    reaction->n_units = 0;
                }
                else
                {
                    str_units = tmp[0];
                    reaction->Kcat = stod(tmp[1]);
                    // 分割 ", " 传入units
                    tmp = split(str_units, ", ");
                    reaction->n_units = tmp.size();
                    for (int i = 0; i < tmp.size(); i++)
                    {
                        string str = tmp[i];
                        vector<string> tmp2 = split(str, " ");
                        reaction->units[i] = new Espace(i, tmp2[1]);
                        reaction->concentrations[i] = new double(stod(tmp2[0]));
                    }
                }

                if (this->espaces.count(extractContent(str_enzyme)) == 0)
                {
                    indice++;
                    this->espaces.insert(std::pair<string, Espace *>(extractContent(str_enzyme), new Espace(indice, extractContent(str_enzyme))));
                    reaction->enzyme = this->espaces[extractContent(str_enzyme)];
                    this->reactions.push_back(reaction);
                    this->listReactions.push_back(reaction);
                }
                else
                {
                    reaction->enzyme = this->espaces[extractContent(str_enzyme)];
                    this->reactions.back()->suivants.push_back(reaction);
                    this->listReactions.push_back(reaction);
                }
            }
            else if (!isReaction)
            {
                Inhibition *inhibition = new Inhibition();
                vector<string> tmp;
                string str_enzyme;
                string str_units;
                string str_reac;
                tmp = split(line, " : ");
                str_enzyme = tmp[0];
                tmp = split(tmp[1], " | ");
                str_reac = tmp[0];
                str_units = tmp[1];
                inhibition->n_substats = 1;
                inhibition->n_units = 1;
                inhibition->substats[0] = new Espace(0, extractContent(str_reac));
                tmp = split(str_units, " ");
                inhibition->units[0] = new Espace(0, tmp[1]);
                inhibition->concentrations[0] = new double(stod(tmp[0]));

                if (this->espaces.count(extractContent(str_enzyme)) == 0)
                {
                    indice++;
                    this->espaces.insert(std::pair<string, Espace *>(extractContent(str_enzyme), new Espace(indice, extractContent(str_enzyme))));
                    inhibition->enzyme = this->espaces[extractContent(str_enzyme)];
                    if (this->inhibitions.size() != 0 && this->inhibitions.back()->enzyme->name == inhibition->enzyme->name)
                    {
                        this->inhibitions.back()->suivants.push_back(inhibition);
                    }
                    else
                    {
                        this->inhibitions.push_back(inhibition);
                    }

                    this->listInhibitions.push_back(inhibition);
                }
                else
                {
                    inhibition->enzyme = this->espaces[extractContent(str_enzyme)];
                    if (this->inhibitions.size() != 0 && this->inhibitions.back()->enzyme->name == inhibition->enzyme->name)
                    {
                        this->inhibitions.back()->suivants.push_back(inhibition);
                    }
                    else
                    {
                        this->inhibitions.push_back(inhibition);
                    }
                    this->listInhibitions.push_back(inhibition);
                }
            }
        }
        // cout << line.size() << endl;
    }
}

// This function builds a graph of reactions by iterating over the list of reactions and creating an edge for each pair of reactions that share a common substrate or product.
void analyseLexicale::buildGraph()
{

    // this->edges.push_back();

    for (int i = 0; i < this->listReactions.size(); i++)
    {

        Edge *edge = new Edge();
        // To
        for (int j = 0; j < this->listReactions[i]->n_products; j++)
        {

            Espace *product = this->listReactions[i]->products[j];

            for (int indiceReaction = 0; indiceReaction < this->listReactions.size(); indiceReaction++)
            {

                for (int k = 0; k < this->listReactions[indiceReaction]->n_substats; k++)
                {
                    Espace *substat = this->listReactions[indiceReaction]->substats[k];

                    if (product->name == substat->name)
                    {

                        edge->addToIndice(indiceReaction);
                    }
                }
            }
        }

        edge->addToIndice(-1);

        // From
        for (int j = 0; j < this->listReactions[i]->n_substats; j++)
        {

            Espace *substat = this->listReactions[i]->substats[j];

            for (int indiceReaction = 0; indiceReaction < this->listReactions.size(); indiceReaction++)
            {

                for (int k = 0; k < this->listReactions[indiceReaction]->n_products; k++)
                {
                    Espace *product = this->listReactions[indiceReaction]->products[k];

                    if (substat->name == product->name)
                    {

                        edge->addFromIndice(indiceReaction);
                    }
                }
            }
        }

        edge->addFromIndice(-1);

        this->edges.push_back(edge);
    }
}

// This function finds all reactions that have entre as a substrate and sortie as a product and returns them as a nodeFromTo object.
nodeFromTo analyseLexicale::getTraces(string entre, string sortie)
{

    nodeFromTo traces;

    for (int i = 0; i < this->listReactions.size(); i++)
    {
        // find node entre
        for (int indiceEntre = 0; indiceEntre < listReactions[i]->n_substats; indiceEntre++)
        {

            if (listReactions[i]->substats[indiceEntre]->name == entre)
                traces.from.push_back(i);
        }
        // find node sortie
        for (int indiceSortie = 0; indiceSortie < listReactions[i]->n_products; indiceSortie++)
        {
            if (listReactions[i]->products[indiceSortie]->name == sortie)
                traces.to.push_back(i);
        }
    }

    cout << "traces from " << entre << " to " << sortie << " finised " << endl;
    return traces;
}

// This is an overloaded operator that allows printing of analyseLexicale objects. It generates a string representation of the reactions and their properties.
ostream &operator<<(ostream &out, const analyseLexicale &a)
{
    string res = "";
    for (int i = 0; i < a.reactions.size(); i++)
    {
        res += a.reactions[i]->enzyme->name + " : ";
        for (int j = 0; j < a.reactions[i]->n_substats; j++)
        {
            if (j == a.reactions[i]->n_substats - 1)

                res += a.reactions[i]->substats[j]->name;
            else
                res += a.reactions[i]->substats[j]->name + " + ";
        }
        res += " -> ";
        for (int j = 0; j < a.reactions[i]->n_products; j++)
        {

            if (j == a.reactions[i]->n_products - 1)

                res += a.reactions[i]->products[j]->name;
            else
                res += a.reactions[i]->products[j]->name + " + ";
        }
        res += " | ";
        for (int j = 0; j < a.reactions[i]->n_units; j++)
        {
            if (j == a.reactions[i]->n_units - 1)
                res += std::to_string(*a.reactions[i]->concentrations[j]) + " " + a.reactions[i]->units[j]->name;
            else
                res += std::to_string(*a.reactions[i]->concentrations[j]) + " " + a.reactions[i]->units[j]->name + ", ";
        }
        res += " - " + std::to_string(a.reactions[i]->Kcat);
        res += ";\n";

        for (int j = 0; j < a.reactions[i]->suivants.size(); j++)
        {
            /* code */
            res += a.reactions[i]->suivants[j]->enzyme->name + " : ";
            for (int k = 0; k < a.reactions[i]->suivants[j]->n_substats; k++)
            {

                if (k == a.reactions[i]->suivants[j]->n_substats - 1)

                    res += a.reactions[i]->suivants[j]->substats[k]->name;
                else
                    res += a.reactions[i]->suivants[j]->substats[k]->name + " + ";
            }
            res += " -> ";
            for (int k = 0; k < a.reactions[i]->suivants[j]->n_products; k++)
            {
                if (k == a.reactions[i]->suivants[j]->n_products - 1)

                    res += a.reactions[i]->suivants[j]->products[k]->name;
                else
                    res += a.reactions[i]->suivants[j]->products[k]->name + " + ";
            }
            res += " | ";
            for (int k = 0; k < a.reactions[i]->suivants[j]->n_units; k++)
            {
                if (k == a.reactions[i]->suivants[j]->n_units - 1)

                    res += std::to_string(*a.reactions[i]->suivants[j]->concentrations[k]) + " " + a.reactions[i]->suivants[j]->units[k]->name;
                else
                    res += std::to_string(*a.reactions[i]->suivants[j]->concentrations[k]) + " " + a.reactions[i]->suivants[j]->units[k]->name + ", ";
            }
            res += " - " + std::to_string(a.reactions[i]->suivants[j]->Kcat);
            res += ";\n";
        }
    }
    for (int i = 0; i < a.inhibitions.size(); i++)
    {
        res += a.inhibitions[i]->enzyme->name + " : ";
        res += a.inhibitions[i]->substats[0]->name + " | ";
        res += std::to_string(*a.inhibitions[i]->concentrations[0]) + " " + a.inhibitions[i]->units[0]->name;
        res += ";\n";
        for (int j = 0; j < a.inhibitions[i]->suivants.size(); j++)
        {
            res += a.inhibitions[i]->suivants[j]->enzyme->name + " : ";
            res += a.inhibitions[i]->suivants[j]->substats[0]->name + " | ";
            res += std::to_string(*a.inhibitions[i]->suivants[j]->concentrations[0]) + " " + a.inhibitions[i]->suivants[j]->units[0]->name;
            res += ";\n";
        }
    }

    out << res;
    return out;
}

// This function generates a string representation of a single reaction specified by indice.
string analyseLexicale::printReaction(int indice)
{
    string res = "";
    res += "\"" + listReactions[indice]->enzyme->name + "\" : ";
    for (int i = 0; i < listReactions[indice]->n_substats; i++)
    {
        res += "\"" + listReactions[indice]->substats[i]->name + "\" ";
        if (i != listReactions[indice]->n_substats - 1)
        {
            res += "+ ";
        }
        else
        {
            res += "-> ";
        }
    }

    for (int i = 0; i < listReactions[indice]->n_products; i++)
    {
        res += "\"" + listReactions[indice]->products[i]->name + "\" ";
        if (i != listReactions[indice]->n_products - 1)
        {
            res += "+ ";
        }
        else
        {
            res += "\n";
        }
    }

    return res;
}

// This function finds all paths of length distance between the reactions with indices entre and sortie. It returns a vector of string representations of the reactions in each path.
vector<string> analyseLexicale::findPaths(int entre, int sortie, int distance)
{

    vector<string> res;
    vector<vector<int>> all_paths = graph->findPaths(entre, sortie, distance);
    // print all paths with required distance
    for (vector<int> path : all_paths)
    {
        if (path.size() == distance)
        {
            string str = "";
            for (int indice : path)
                str += this->printReaction(indice);
            str += "-----------------\n";
            res.push_back(str);
        }
    }

    return res;
}

//  This function removes any reactions from listReactions that are inhibited by enzymes listed in listInhibitions.
void analyseLexicale::verifierInhib()
{
    if (this->listInhibitions.size() > 0)
    {
        int i = 0;
        while (i < listReactions.size())
        {
            bool shouldRemove = false;
            for (auto e : listInhibitions)
            {
                if (e->enzyme->name == listReactions[i]->enzyme->name)
                {
                    for (int j = 0; j < listReactions[i]->n_substats; j++)
                    {
                        if (listReactions[i]->substats[j]->name == e->substats[0]->name)
                        {
                            shouldRemove = true;
                            break;
                        }
                    }
                }
                if (shouldRemove)
                {
                    break;
                }
            }

            if (shouldRemove)
            {
                delete listReactions[i];
                listReactions.erase(listReactions.begin() + i);
            }
            else
            {
                i++;
            }
        }
    }
}
