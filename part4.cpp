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
#include <sstream>
#include <unordered_set>

#define BLOCK_OFFSET  6
#define THREAD_NUM    8

using namespace std;
namespace fs = std::experimental::filesystem;
typedef unsigned long long int_t;

int main(int argc, char *argv[]) {
    int_t tid, block;
    string line, folder = "output";
    for (const auto& tracefile : fs::directory_iterator(folder)) {
        map <int_t, unordered_set<int> > matrix;
        map <int, int> profile;
        ifstream tracestrm (tracefile.path());
        if (!tracestrm.is_open())
            cerr << "Tracefile " << tracefile.path().filename().string()
                << " could not be opened\n";
        
        cout << "Processing file " << tracefile.path().filename().string()
             << endl;
        while (getline(tracestrm, line)) {
            stringstream line_(line);
            line_ >> tid >> block;
            block = block >> BLOCK_OFFSET;
            matrix[block].emplace(tid);
        }
        // Segregate all blocks into their respective profiles
        map <int_t, unordered_set <int> > :: iterator itr;
        for (itr = matrix.begin(); itr != matrix.end(); ++itr) {
            profile[itr->second.size()]++;
        }
        // Result compilation
        cout << "Number of private blocks:  " << profile[1] << endl;
        for (int i = 2; i <= THREAD_NUM; ++i) {
            cout << "Number of " << i << " shared blocks: "
                 << profile[i] << endl;
        }
    }
    return 0;
}
