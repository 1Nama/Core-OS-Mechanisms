#include <iostream>
#include <vector>

const int P = 5; // Number of processes
const int R = 3; // Number of resource types

// Function to check if the system is in a safe state
bool isSafe(std::vector<int> available, std::vector<std::vector<int>> max, std::vector<std::vector<int>> allocation) {
    std::vector<std::vector<int>> need(P, std::vector<int>(R));
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }

    std::vector<bool> finish(P, false);
    std::vector<int> safeSeq;
    std::vector<int> work = available;

    int count = 0;
    while (count < P) {
        bool found = false;
        for (int p = 0; p < P; p++) {
            if (!finish[p]) {
                int j;
                for (j = 0; j < R; j++) {
                    if (need[p][j] > work[j]) {
                        break;
                    }
                }
                if (j == R) {
                    for (int k = 0; k < R; k++) {
                        work[k] += allocation[p][k];
                    }
                    safeSeq.push_back(p);
                    finish[p] = true;
                    found = true;
                    count++;
                }
            }
        }
        if (!found) {
            std::cout << "System is not in a safe state!\n";
            return false;
        }
    }

    std::cout << "System is in a safe state.\nSafe sequence is: ";
    for (int i = 0; i < P; i++) {
        std::cout << "P" << safeSeq[i] << (i == P - 1 ? "" : " -> ");
    }
    std::cout << "\n";
    return true;
}

// Resource-Request Algorithm
void resourceRequest(int processId, std::vector<int> request, std::vector<int>& available, std::vector<std::vector<int>>& max, std::vector<std::vector<int>>& allocation) {
    std::cout << "\n--- Processing Request from P" << processId << " for resources [ ";
    for(int r : request) std::cout << r << " ";
    std::cout << "] ---\n";

    std::vector<std::vector<int>> need(P, std::vector<int>(R));
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }

    // 1. Check if request <= need
    for (int i = 0; i < R; i++) {
        if (request[i] > need[processId][i]) {
            std::cout << "Error: Process has exceeded its maximum claim.\n";
            return;
        }
    }

    // 2. Check if request <= available
    for (int i = 0; i < R; i++) {
        if (request[i] > available[i]) {
            std::cout << "Process must wait, resources not available.\n";
            return;
        }
    }

    // 3. Pretend to allocate resources
    std::vector<int> tempAvailable = available;
    std::vector<std::vector<int>> tempAllocation = allocation;

    for (int i = 0; i < R; i++) {
        tempAvailable[i] -= request[i];
        tempAllocation[processId][i] += request[i];
    }
    
    // 4. Run safety algorithm on the new state
    if (isSafe(tempAvailable, max, tempAllocation)) {
        std::cout << "Request granted!\n";
        // If safe, permanently update the state
        available = tempAvailable;
        allocation = tempAllocation;
    } else {
        std::cout << "Request denied as it leads to an unsafe state.\n";
    }
}

int main() {
    std::vector<std::vector<int>> allocation = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };

    std::vector<std::vector<int>> max = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };

    std::vector<int> available = {3, 3, 2};

    // Check initial state
    std::cout << "--- Initial System State Check ---\n";
    isSafe(available, max, allocation);

    // Simulate a resource request
    // Request from P1 for (1, 0, 2)
    resourceRequest(1, {1, 0, 2}, available, max, allocation);
    
    // Simulate another resource request that might fail
    // Request from P4 for (3, 3, 0)
    resourceRequest(4, {3, 3, 0}, available, max, allocation);
    
    // Simulate another request from P0
    // Request from P0 for (0, 2, 0)
    resourceRequest(0, {0, 2, 0}, available, max, allocation);

    return 0;
}