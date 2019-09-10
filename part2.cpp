/*
 * Copyright (c) 2019, Aditya Rohan
 * Copyright (c) 2019, Aniket Pandey
 * 
 * Submitted to:
 * CS622A: 2019-20 Fall Semester. Assignment 2
 */

#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;
namespace fs = std::experimental::filesystem;

typedef long long int_t;
typedef std::vector <int_t> MyClassSet;
typedef std::map <int_t, MyClassSet> MyClassSetMap;

#define BLOCK_OFFSET 6

int main(int argc, char const *argv[]) {
    int_t tid, block, time = 0;
    string folder = "output";
    for (const auto& tracefile : fs::directory_iterator(folder)) {
        std::ifstream infile (tracefile.path());
        if (!infile.is_open()) {
            cerr << "Tracefile " << tracefile.path().filename().string()
                 << " could not be opened\n";
        }
        MyClassSetMap maps;
        map <int_t, int_t> distance;
        cout << "Processing file " << tracefile.path().filename().string()
             << endl;
        while (infile >> tid >> block) {
            block = block >> BLOCK_OFFSET;
            maps[block].emplace_back(time);
            time++;
        }
        // Segregate all blocks into their respective 'distances'
        for(auto& elem : maps) {
            for (int i = 0; i < elem.second.size() - 1; ++i) {
                int_t cd = elem.second[i+1] - elem.second[i];
                distance[cd]++;
            }
        }
        // Result Compilation
        for(auto& elem : distance) {
            cout << "Number of " << elem.first << " access distance blocks: "
                 << elem.second << endl;
        }
    }
    return 0;
}
