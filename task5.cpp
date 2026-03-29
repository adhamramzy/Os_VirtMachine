
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

struct Process {
    int pid;
    int arrival;
    int burst;
};

// firstcomefirst servee
double firstcome(vector<Process> processes) {
    sort(processes.begin(), processes.end(), []
    (Process a, Process b) {
        return a.arrival < b.arrival;
        });

    int time = 0;
    double totwait = 0;

    for (int i = 0; i < processes.size(); i++) {
        if (time < processes[i].arrival) {
            time = processes[i].arrival;
        }

        int wait = time - processes[i].arrival;
        totwait += wait;

        time = time + processes[i].burst;
    }

    return totwait / processes.size();
}

// shortest job firstt
double shortfirst(vector<Process> processes) {
    int time = 0;
    double totwait = 0;

    vector<bool> done(processes.size(), false);
    int finished = 0;

    while (finished < processes.size()) {
        int idx = -1;
        int minburs = 1000000;

        for (int i = 0; i < processes.size(); i++) {
            if (!done[i] && processes[i].arrival <= time) {
                if (processes[i].burst < minburs) {
                    minburs = processes[i].burst;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        totwait += time - processes[idx].arrival;
        time = time + processes[idx].burst;
        done[idx] = true;
        finished++;
    }

    return totwait / processes.size();
}

// roundrobin(fixed time slice)
double Rr(vector<Process> processes, int quantum) {
    queue<int> q;
    int time = 0;
    double totwait = 0;

    vector<int> remain(processes.size());
    vector<int> complete(processes.size(), 0);
    vector<bool> inq(processes.size(), false);

    for (int i = 0; i < processes.size(); i++) {
        remain[i] = processes[i].burst;
    }

    int completed = 0;

    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrival < b.arrival;
        });

    while (completed < processes.size()) {

        // add new arrival
        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].arrival <= time && !inq[i] && remain[i] > 0) {
                q.push(i);
                inq[i] = true;
            }
        }
        if (q.empty()) {
            time++;
            continue;
        }
        int idx = q.front();
        q.pop();

        int exec = quantum;
        if (remain[idx] < quantum) {
            exec = remain[idx];
        }

        time = time + exec;
        remain[idx] = remain[idx] - exec;

        // add new arrived proc
        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].arrival <= time && !inq[i] && remain[i] > 0) {
                q.push(i);
                inq[i] = true;
            }
        }
        if (remain[idx] > 0) {
            q.push(idx);
        }
        else {
            complete[idx] = time;
            completed++;
        }
    }

    // calc waiting period
    for (int i = 0; i < processes.size(); i++) {
        int turnaround = complete[i] - processes[i].arrival;
        int waiting = turnaround - processes[i].burst;
        totwait = totwait + waiting;
    }

    return totwait / processes.size();
}

// main func
int main() {

    vector<Process> processes;

    // input all da processes
    processes.push_back({ 1, 0, 5 });
    processes.push_back({ 2, 1, 3 });
    processes.push_back({ 3, 2, 8 });
    processes.push_back({ 4, 3, 6 });

    double firstcomeaverage_avg = firstcome(processes);
    double shortfirst_avg = shortfirst(processes);
    double Rr_avg = Rr(processes, 2);

    // csv form
    cout << "Algorithm,AverageWaitingTime" << endl;
    cout << "first come:" << firstcomeaverage_avg << endl;
    cout << "shortest job:" << shortfirst_avg << endl;
    cout << "Round Robin:" << Rr_avg << endl;

    return 0;
}
