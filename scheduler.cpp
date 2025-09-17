#include <bits/stdc++.h>
using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;
};

void printResults(const std::vector<Process>& processes, const std::string& algorithmName) {
    float totalTurnaroundTime = 0;
    float totalWaitingTime = 0;
    int n = processes.size();

    std::cout << "\n--- " << algorithmName << " ---\n";
    std::cout << std::setw(5) << "PID" << std::setw(10) << "Arrival" << std::setw(10) << "Burst"
              << std::setw(15) << "Completion" << std::setw(15) << "Turnaround" << std::setw(10) << "Waiting" << "\n";

    // Sort by ID for consistent output
    std::vector<Process> sortedProcesses = processes;
    std::sort(sortedProcesses.begin(), sortedProcesses.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    for (const auto& p : sortedProcesses) {
        std::cout << std::setw(5) << p.id << std::setw(10) << p.arrivalTime << std::setw(10) << p.burstTime
                  << std::setw(15) << p.completionTime << std::setw(15) << p.turnaroundTime << std::setw(10) << p.waitingTime << "\n";
        totalTurnaroundTime += p.turnaroundTime;
        totalWaitingTime += p.waitingTime;
    }
    std::cout << "\nAverage Turnaround Time: " << totalTurnaroundTime / n << "\n";
    std::cout << "Average Waiting Time: " << totalWaitingTime / n << "\n";
}

// First Come First Served (No changes)
void fcfs(std::vector<Process> processes) {
    int n = processes.size();
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    for (int i = 0; i < n; ++i) {
        if (currentTime < processes[i].arrivalTime) {
            currentTime = processes[i].arrivalTime;
        }
        processes[i].completionTime = currentTime + processes[i].burstTime;
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
        currentTime = processes[i].completionTime;
    }
    printResults(processes, "First Come First Served (FCFS)");
}

// Preemptive Shortest Job First (SRTF) (No changes)
void preemptiveSJF(std::vector<Process> processes) {
    int n = processes.size();
    int completed = 0;
    int currentTime = 0;
    
    while (completed != n) {
        int shortestJobIndex = -1;
        int minRemainingTime = INT_MAX;

        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0) {
                if (processes[i].remainingTime < minRemainingTime) {
                    minRemainingTime = processes[i].remainingTime;
                    shortestJobIndex = i;
                }
            }
        }

        if (shortestJobIndex == -1) {
            currentTime++;
        } else {
            processes[shortestJobIndex].remainingTime--;
            currentTime++;

            if (processes[shortestJobIndex].remainingTime == 0) {
                completed++;
                processes[shortestJobIndex].completionTime = currentTime;
                processes[shortestJobIndex].turnaroundTime = processes[shortestJobIndex].completionTime - processes[shortestJobIndex].arrivalTime;
                processes[shortestJobIndex].waitingTime = processes[shortestJobIndex].turnaroundTime - processes[shortestJobIndex].burstTime;
            }
        }
    }
    printResults(processes, "Preemptive Shortest Job First (SJF/SRTF)");
}

// ** CORRECTED Round Robin **
void roundRobin(std::vector<Process> processes, int quantum) {
    int n = processes.size();
    if (n == 0) return;

    std::queue<int> readyQueue;
    // 'inQueue' is now declared in the function's main scope
    std::vector<bool> inQueue(n, false);
    int currentTime = 0;
    int completed = 0;

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    while (completed < n) {
        // Step 1: Add newly arrived processes to the ready queue.
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && !inQueue[i]) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }

        // If the ready queue is empty, the CPU is idle. Advance time.
        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }

        // Step 2: Get a process from the front of the queue.
        int idx = readyQueue.front();
        readyQueue.pop();
        
        // Step 3: Execute the process for one quantum or until it finishes.
        int timeToRun = std::min(quantum, processes[idx].remainingTime);
        processes[idx].remainingTime -= timeToRun;
        currentTime += timeToRun;
        
        // Step 4: After execution, re-check for any new processes that may have arrived.
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && !inQueue[i]) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }

        // Step 5: If the process is not finished, add it back to the end of the queue.
        if (processes[idx].remainingTime > 0) {
            readyQueue.push(idx);
        } else {
            // If it is finished, calculate its metrics and mark it as complete.
            completed++;
            processes[idx].completionTime = currentTime;
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
            inQueue[idx] = false; // It's no longer in the ready queue system
        }
    }
    printResults(processes, "Round Robin (RR)");
}

int main() {
    std::vector<Process> processes = {
        {1, 0, 8}, {2, 1, 4}, {3, 2, 9}, {4, 3, 5}
    };
    
    // Initialize remaining time for algorithms that need it
    for(auto& p : processes) {
        p.remainingTime = p.burstTime;
    }

    int quantum = 4;

    fcfs(processes);
    preemptiveSJF(processes);
    roundRobin(processes, quantum);

    return 0;
}