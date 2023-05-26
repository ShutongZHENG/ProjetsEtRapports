#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include "analyseLexicale.hpp"
using std::cerr;
using std::stoi;

int main(int argc, char *argv[])
{

    // Check if correct number of command line arguments is provided
    if (argc < 5)
    {
        cerr << "Usage: " << argv[0] << " <filename> <entre> <sortie> <distance>" << endl;
        return 1;
    }

    /// Parse command line arguments
    string filename = argv[1];
    string reactant = argv[2];
    string product = argv[3];
    int distance = stoi(argv[4]);

    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();

    // Create shared pointer to analyseLexicale object
    std::shared_ptr<analyseLexicale> analyseLexicale_ptr(new analyseLexicale(filename));

    // Get nodeFromTo object for given reactant and product
    nodeFromTo nft = analyseLexicale_ptr->getTraces(reactant, product);

    // Create vector of threads and mutex for results
    vector<std::thread> threads;
    vector<std::string> results;
    std::mutex results_mutex;
    int nbTraces = 0;

    // Iterate through all possible combinations of nodeFromTo object
    for (int entre : nft.from)
    {
        for (int sortie : nft.to)
        {
            // Create a new thread for each combination of nodes
            std::thread t([&, analyseLexicale_ptr]()
                          {
                 // Find all paths with given distance between the nodes
                 vector<string> res = analyseLexicale_ptr->findPaths(entre, sortie, distance);
                 // Add the paths to the results vector using a lock_guard to prevent race conditions
                 std::lock_guard<std::mutex> lock(results_mutex);
                 for(auto e : res)
                 if (std::find(results.begin(), results.end(), e) == results.end()) {
                     results.push_back(e);
                     nbTraces++;
                 }
                 cout<< "thread finished" << endl; });
            // Add the thread to the vector of threads
            threads.push_back(std::move(t));
        }
    }

    // Wait for all threads to finish
    for (auto &t : threads)
    {
        t.join();
    }

    // Write results to output file
    std::ofstream output_file("output.txt", std::ios::trunc);
    if (output_file.is_open())
    {
        output_file << "Entre: " << reactant << std::endl;
        output_file << "Sortie: " << product << std::endl;
        output_file << "Distance: " << distance << std::endl;
        output_file << "Nombre de traces: " << nbTraces << std::endl;
        for (const auto &result : results)
        {
            if (result == "")
                continue;
            output_file << result << std::endl;
        }
        output_file.close();
    }
    else
    {
        std::cerr << "Unable to open file: output.txt" << std::endl;
    }
    // End timing and print program running time
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    cout << "Program running time: " << duration << " milliseconds" << endl;
    return 0;
}
