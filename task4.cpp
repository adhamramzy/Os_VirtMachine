#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

int main(int argc, char* argv[]) {
    string fname = "task4.inp.txt";
    if (argc > 1) fname = argv[1];
    //if to check if file can open 
    ifstream fin(fname);
    if (!fin) {
        cout << "Error opening file\n";
        return 1;
    }
    //number of processes and resource types
    int numproc, mnptype;
    fin >> numproc >> mnptype;
    // create vectior to storeexisting and avalible resources
    vector<int> exist(mnptype), aval(mnptype);
    for (int j = 0; j < mnptype; j++) 
    fin >> exist[j];
    // what each processhas andwhat each processrequests
    vector<vector<int>> C(numproc, vector<int>(mnptype));
    vector<vector<int>> R(numproc, vector<int>(mnptype));

    for (int i = 0; i < numproc; i++)
        for (int j = 0; j < mnptype; j++)
            fin >> C[i][j];

    for (int i = 0; i < numproc; i++)
        for (int j = 0; j < mnptype; j++)
            fin >> R[i][j];

    // calculkate the avalible minus sum of allocations
    for (int j = 0; j < mnptype; j++) {
        int sum = 0;
        for (int i = 0; i < numproc; i++)
            sum += C[i][j];
        aval[j] = exist[j] - sum;
    }
    // boolean to check ifprocesscanfinish then another boolean to track progress finished in current loop
    vector<bool> finch(numproc, false);
    bool prog = true;
    // outer loop repeats as long asany process can makeprogress, if unfinished process check  whatit needs
    while (prog) {
        prog = false;
        for (int i = 0; i < numproc; i++) {
            if (!finch[i]) {
                bool canfin = true;
                for (int j = 0; j < mnptype; j++) {
                    if (R[i][j] > aval[j]) {
                        canfin = false;
                        break;
                    }
                }
                if (canfin) {
                    for (int j = 0; j < mnptype; j++)
                        aval[j] += C[i][j];
                      finch[i] = true;
                       prog = true;
              }
           }
        }
    }

    // boolto check deadlock
    bool dl = false;
    for (int i = 0; i < numproc; i++) {
        if (!finch[i]) {
            dl = true;
            break;
        }
    }
    // if no deadlock
    if (!dl) {
        cout << "no deadlock detected\n";
    }
    else {
        cout << "deadlock detected,\nDeadlocked processes: ";
        for (int i = 0; i < numproc; i++)
            if (!finch[i])
                cout << "P" << i << " ";
        cout << "\n";
    }

    return 0;
}


