#include <iostream>
#include <list>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>

class LRUCache {
private:
    int capacity;
    // list of {key, value} pairs. The front is most recent.
    std::list<std::pair<int, int>> items; 
    // map from key to an iterator pointing to its position in the list
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cacheMap; 
    int pageFaults = 0;

public:
    LRUCache(int cap) : capacity(cap) {}

    void refer(int key) {
        std::cout << "Referring to page " << key << ": ";
        
        // Case 1: Page is NOT in cache (Page Fault)
        if (cacheMap.find(key) == cacheMap.end()) {
            pageFaults++;
            std::cout << "Page Fault! -> ";

            // Case 1a: Cache is full, need to evict LRU page
            if (items.size() == capacity) {
                // Get the least recently used item (at the back of the list)
                std::pair<int, int> lru = items.back();
                items.pop_back();
                cacheMap.erase(lru.first);
                 std::cout << "Cache full, evicting page " << lru.first << ". ";
            }
             // Add the new page to the front (most recent)
            items.push_front({key, key}); // Using key as value for simplicity
            cacheMap[key] = items.begin();
            std::cout << "Loaded page " << key << ".\n";
        } 
        // Case 2: Page IS in cache (Cache Hit)
        else {
            std::cout << "Cache Hit!\n";
            // Move the accessed item to the front to mark it as most recently used
            items.splice(items.begin(), items, cacheMap[key]);
        }
        printCacheState();
    }
    
    void printCacheState() {
        std::cout << "Cache State: [ ";
        for(const auto& item : items) {
            std::cout << item.first << " ";
        }
        std::cout << "]\n\n";
    }

    int getPageFaults() const {
        return pageFaults;
    }
};

int main() {
    int cacheCapacity = 4;
    LRUCache lru(cacheCapacity);

    // A sample reference string of page numbers
    std::vector<int> referenceString = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};

    std::cout << "Simulating LRU Cache with capacity " << cacheCapacity << "\n";
    std::cout << "Reference String: ";
    for(int page : referenceString) std::cout << page << " ";
    std::cout << "\n---------------------------------------\n\n";

    for (int page : referenceString) {
        lru.refer(page);
    }
    
    std::cout << "---------------------------------------\n";
    std::cout << "Simulation Complete.\n";
    std::cout << "Total Page Faults: " << lru.getPageFaults() << "\n";

    return 0;
}