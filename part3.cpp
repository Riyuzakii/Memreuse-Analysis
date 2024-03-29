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
#include <list>
#include <map>
#include <memory>
#include <utility>
#include <vector>

using namespace std;
namespace fs = std::experimental::filesystem;

typedef long long int_t;
typedef std::vector <int_t> MyClassSet;
typedef std::map <int_t, MyClassSet> MyClassSetMap;

#define BLOCK_OFFSET  6
#define NUM_WAYS      16
#define NUM_SETS      2048

struct cell {
    bool present;   // Valid bit
    int_t address;  // Memory address
};

class Cache {
  public:
    typedef std::shared_ptr<Cache> Ptr;

    // ctor
    Cache(int num_sets, int num_ways);

    // dtor
    ~Cache() = default;

    // Retrieve block from memory on miss.
    int_t add_block(int_t address);
    
    // Check hit or miss for req. address
    bool check_hit_or_miss(int_t address);
    
    // Evict victim block from cache.
    void invalidate_block(int_t address);

    // Update LRU table on cache hit.
    void update_on_hit(int_t address);

    // Count of hits and misses
    int_t hits = 0, misses = 0;

  private:
    // Number of sets in the cache.
    int num_sets_;

    // Number of ways in the cache.
    int num_ways_;

    // 2-d matrix, representing the cache org.
    std::vector <std::vector <cell> > matrix_;

    // List of sets for recording LRU status.
    std::vector <std::list <int_t> > lru_set_;
    
};

//----------------------------------------------------------------------------

Cache::Cache(int num_sets, int num_ways)
            : num_sets_(move(num_sets)),
              num_ways_(move(num_ways)) {
    // Create a 2d array and LRU record for the cache
    matrix_.resize(num_sets, vector <cell>(num_ways));
    lru_set_.resize(num_sets);
};

int_t Cache::add_block(int_t address) {
    int set_num = address % num_sets_;
    // Check if a way is free in the set
    for (auto &set : matrix_.at(set_num)) {
        if (set.present) continue;
        // Found an empty slot
        set.present = true;
        set.address = address;
        lru_set_.at(set_num).push_front(address);
        return 0;
    }
    // All 'ways' in the set are valid, evict one
    int_t evict_block = lru_set_.at(set_num).back();
    lru_set_.at(set_num).pop_back();

    for (auto &block : matrix_.at(set_num)) {
        if (block.address != evict_block) continue;
        // Found the block to be evicted
        block.address = address;
        lru_set_.at(set_num).push_front(address);
        return evict_block;
    }
    abort(); // Something bad happened
}

bool Cache::check_hit_or_miss(int_t address) {
    int set_num = address % num_sets_;
    for (auto &block : matrix_.at(set_num)) {
        if ((block.address == address) && (block.present))
            return true;
    }
    return false;
}

void Cache::invalidate_block(int_t address) {
    int set_num = address % num_sets_;
    for (auto &block : matrix_.at(set_num)) {
        if (block.address != address) continue;
        // Found the block; Invalidate it
        block.present = false;
        lru_set_.at(set_num).remove(address);
        return;
    }
    abort(); // Something bad happened
}

void Cache::update_on_hit(int_t address) {
    int set_num = address % num_sets_;
    // Move the block to the top of LRU set
    lru_set_.at(set_num).remove(address);
    lru_set_.at(set_num).push_front(address);
    return;
}

//----------------------------------------------------------------------------

int main(int argc, char const *argv[]) {
    int_t tid, block, time = 0;
    int_t num_dist = 0, f_dist = 0, fn = 0;
    string folder = "traces", output = "result/part3-cdf-lru-misses.out";
    for (const auto& tracefile : fs::directory_iterator(folder)) {
        std::ifstream infile (tracefile.path());
        if (!infile.is_open()) {
            cerr << "Tracefile " << tracefile.path().filename().string()
                 << " could not be opened\n";
        }
        Cache::Ptr cache = make_shared<Cache> (NUM_SETS, NUM_WAYS);
        MyClassSetMap maps;
        map <int_t, int_t> distance;

        cout << "Processing file " << tracefile.path().filename().string()
             << endl;
        while (infile >> tid >> block) {
            block = block >> BLOCK_OFFSET;
            // Simulate the block through 2MB 16-Way cache
            if (cache->check_hit_or_miss(block)) {
                (cache->hits)++;
                cache->update_on_hit(block);
            } else {
                (cache->misses)++;
                cache->add_block(block);
                maps[block].emplace_back(time);
                time++;
            }
        }
        // Segregate all blocks into their respective 'distances'
        for(auto& elem : maps) {
            for (int i = 0; i < elem.second.size() - 1; ++i) {
                int_t cd = elem.second[i+1] - elem.second[i];
                distance[cd]++;
                num_dist++;
            }
        }
        // Result Compilation
        ofstream myfile;
        myfile.open (output);
        for(auto& elem : distance) {
            f_dist += elem.second;
            myfile << elem.first << ": " << (float) f_dist/(float)num_dist
                   << endl;
        }
        myfile.close();
        cout << "\nNumber of hits: " << cache->hits << "\nNumber of misses: "
             << cache->misses << "\n\n";
    }
    return 0;
}
