#include <bits/stdc++.h>
using namespace std;

// The Process struct now includes a priority level
struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int priority; // Used by MLQ/MLFQ

    // State variables for scheduling
    int remainingTime;
    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;
};

// ** UPDATED printResults function **
void printResults(const std::vector<Process>& processes, const std::string& algorithmName) {
    float totalTurnaroundTime = 0;
    float totalWaitingTime = 0;
    int n = processes.size();

    // Decide whether to display the priority column
    bool displayPriority = true;
    if (algorithmName.find("FCFS") != std::string::npos ||
        algorithmName.find("SJF") != std::string::npos ||
        algorithmName.find("RR") != std::string::npos) {
        displayPriority = false;
    }

    std::cout << "\n--- " << algorithmName << " ---\n";
    std::cout << std::setw(5) << "PID" << std::setw(10) << "Arrival" << std::setw(10) << "Burst"
              << std::setw(10) << "Priority" << std::setw(15) << "Completion" << std::setw(15) << "Turnaround" << std::setw(10) << "Waiting" << "\n";

    std::vector<Process> sortedProcesses = processes;
    std::sort(sortedProcesses.begin(), sortedProcesses.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    for (const auto& p : sortedProcesses) {
        std::cout << std::setw(5) << p.id << std::setw(10) << p.arrivalTime << std::setw(10) << p.burstTime;
        
        // Print the priority value or a dash based on the flag
        if (displayPriority) {
            std::cout << std::setw(10) << p.priority;
        } else {
            std::cout << std::setw(10) << "-";
        }
        
        std::cout << std::setw(15) << p.completionTime << std::setw(15) << p.turnaroundTime << std::setw(10) << p.waitingTime << "\n";
        
        totalTurnaroundTime += p.turnaroundTime;
        totalWaitingTime += p.waitingTime;
    }
    std::cout << "\nAverage Turnaround Time: " << totalTurnaroundTime / n << "\n";
    std::cout << "Average Waiting Time: " << totalWaitingTime / n << "\n";
}


// First Come First Served
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

// Preemptive Shortest Job First (SRTF)
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

// Round Robin
void roundRobin(std::vector<Process> processes, int quantum) {
    int n = processes.size();
    if (n == 0) return;

    std::queue<int> readyQueue;
    std::vector<bool> inQueue(n, false);
    int currentTime = 0;
    int completed = 0;

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    while (completed < n) {
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && !inQueue[i]) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }

        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }

        int idx = readyQueue.front();
        readyQueue.pop();
        
        int timeToRun = std::min(quantum, processes[idx].remainingTime);
        processes[idx].remainingTime -= timeToRun;
        currentTime += timeToRun;
        
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && !inQueue[i]) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }

        if (processes[idx].remainingTime > 0) {
            readyQueue.push(idx);
        } else {
            completed++;
            processes[idx].completionTime = currentTime;
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
            inQueue[idx] = false;
        }
    }
    printResults(processes, "Round Robin (RR)");
}

// Multilevel Queue (3-Level)
void multilevelQueue(std::vector<Process> processes) {
    int n = processes.size();
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    std::queue<int> q_system;      // Priority 1: RR, quantum=4
    std::queue<int> q_interactive; // Priority 2: RR, quantum=8
    std::queue<int> q_batch;       // Priority 3: FCFS

    int quanta[] = {0, 4, 8};
    
    int currentTime = 0;
    int completed = 0;
    int processIdx = 0;
    std::vector<int> timeInQuantum(n + 1, 0);

    while (completed < n) {
        while (processIdx < n && processes[processIdx].arrivalTime <= currentTime) {
            if (processes[processIdx].priority == 1) {
                q_system.push(processIdx);
            } else if (processes[processIdx].priority == 2) {
                q_interactive.push(processIdx);
            } else {
                q_batch.push(processIdx);
            }
            processIdx++;
        }

        if (!q_system.empty()) {
            int currentProc = q_system.front();
            int quantum = quanta[1];

            if (timeInQuantum[processes[currentProc].id] == quantum) {
                 timeInQuantum[processes[currentProc].id] = 0;
                 q_system.pop();
                 q_system.push(currentProc);
                 currentProc = q_system.front();
            }
            processes[currentProc].remainingTime--;
            timeInQuantum[processes[currentProc].id]++;
            currentTime++;
            if (processes[currentProc].remainingTime == 0) {
                processes[currentProc].completionTime = currentTime;
                completed++;
                q_system.pop();
            }
        } 
        else if (!q_interactive.empty()) {
            int currentProc = q_interactive.front();
            int quantum = quanta[2];

            if (timeInQuantum[processes[currentProc].id] == quantum) {
                 timeInQuantum[processes[currentProc].id] = 0;
                 q_interactive.pop();
                 q_interactive.push(currentProc);
                 currentProc = q_interactive.front();
            }
            processes[currentProc].remainingTime--;
            timeInQuantum[processes[currentProc].id]++;
            currentTime++;
            if (processes[currentProc].remainingTime == 0) {
                processes[currentProc].completionTime = currentTime;
                completed++;
                q_interactive.pop();
            }
        }
        else if (!q_batch.empty()) {
            int currentProc = q_batch.front();
            processes[currentProc].remainingTime--;
            currentTime++;
            if (processes[currentProc].remainingTime == 0) {
                processes[currentProc].completionTime = currentTime;
                completed++;
                q_batch.pop();
            }
        } 
        else {
            currentTime++;
        }
    }

    for(int i = 0; i < n; ++i) {
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
    }
    printResults(processes, "Multilevel Queue (MLQ)");
}

// Multilevel Feedback Queue (MLFQ)
void multilevelFeedbackQueue(std::vector<Process> processes) {
    int n = processes.size();
    
    std::vector<std::queue<int>> queues(3);
    int quanta[] = {8, 16, -1};
    
    int currentTime = 0;
    int completed = 0;
    int processIdx = 0;
    std::vector<int> currentQueue(n, 0);
    std::vector<int> timeInQuantum(n, 0);
    
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    const int AGING_PERIOD = 50; 

    while(completed < n) {
        while (processIdx < n && processes[processIdx].arrivalTime <= currentTime) {
            queues[0].push(processIdx);
            processIdx++;
        }

        if (currentTime > 0 && currentTime % AGING_PERIOD == 0) {
            std::vector<int> tempStore;
             for(size_t q_level = 1; q_level < queues.size(); ++q_level) {
                 while(!queues[q_level].empty()) {
                     int p_idx = queues[q_level].front();
                     queues[q_level].pop();
                     tempStore.push_back(p_idx);
                 }
             }
             for(int p_idx : tempStore) {
                 queues[0].push(p_idx);
                 currentQueue[p_idx] = 0;
                 timeInQuantum[p_idx] = 0;
             }
        }

        int currentProc = -1;
        int qLevel = -1;

        for (size_t i = 0; i < queues.size(); ++i) {
            if (!queues[i].empty()) {
                currentProc = queues[i].front();
                qLevel = i;
                break;
            }
        }

        if (currentProc == -1) {
            currentTime++;
            continue;
        }

        queues[qLevel].pop();
        currentTime++;
        processes[currentProc].remainingTime--;
        timeInQuantum[currentProc]++;

        if (processes[currentProc].remainingTime == 0) {
            completed++;
            processes[currentProc].completionTime = currentTime;
        } else {
            if (qLevel < 2 && timeInQuantum[currentProc] >= quanta[qLevel]) {
                queues[qLevel + 1].push(currentProc); 
                currentQueue[currentProc] = qLevel + 1;
                timeInQuantum[currentProc] = 0;
            } else {
                queues[qLevel].push(currentProc);
            }
        }
    }

    for(int i = 0; i < n; ++i) {
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
    }
    printResults(processes, "Multilevel Feedback Queue (MLFQ)");
}


int main() {
    // Sample processes {id, arrival, burst, priority}
    // For MLQ -> Prio 1: System, Prio 2: Interactive, Prio 3: Batch
     std::vector<Process> processes = {
    // {id, arrival, burst, priority}
        {1, 0, 20, 1}, // A CRITICAL but long "system" process
        {2, 1, 2,  3}, // An unimportant, short "batch" process
        {3, 2, 2,  3}  // Another unimportant, short "batch" process
    };
    
    int rr_quantum = 4;
    
    // Create a working copy for each algorithm
    std::vector<Process> processes_copy = processes;
    for(auto& p : processes_copy) p.remainingTime = p.burstTime;
    fcfs(processes_copy);

    for(auto& p : processes_copy) p.remainingTime = p.burstTime;
    preemptiveSJF(processes_copy);
    
    for(auto& p : processes_copy) p.remainingTime = p.burstTime;
    roundRobin(processes_copy, rr_quantum);

    for(auto& p : processes_copy) p.remainingTime = p.burstTime;
    multilevelQueue(processes_copy);
    
    for(auto& p : processes_copy) p.remainingTime = p.burstTime;
    multilevelFeedbackQueue(processes_copy);

    return 0;
}