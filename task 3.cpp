
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;

// frame to store pg num, age nd the refrence bits
struct frame {
    int page;
    unsigned char age;
    bool ref;

    frame() : page(-1), age(0), ref(false) {}
};

// read from da file
vector<int> readp(const string& filename) {
    ifstream file(filename);
    vector<int> pages;
    int x;

    if (!file) {
        cerr << "Error, could not open file '" << filename << "'\n";
        return pages;
    }

    while (file >> x)
        pages.push_back(x);

    return pages;
}

// sim aging algo
int simage(const vector<int>& pages, int numframes) {
    vector<frame> frames(numframes);
    int faults = 0;

    // loop thru page ref
    for (int i = 0; i < pages.size(); i++) {
        int p = pages[i];
        bool hit = false;

        // check if pg in memory
        for (int j = 0; j < numframes; j++) {
            if (frames[j].page == p) {
                frames[j].ref = true;
                hit = true;
                break;
            }
        }

        // pg fault
        if (!hit) {
            faults++;

            // Find empty frame
            int victim = -1;
            for (int j = 0; j < numframes; j++) {
                if (frames[j].page == -1) {
                    victim = j;
                    break;
                }
            }

            // ifnone empty, pick smallest age
            if (victim == -1) {
                victim = 0;
                for (int j = 1; j < numframes; j++) {
                    if (frames[j].age < frames[victim].age)
                        victim = j;
                }
            }

            // replace da victim pg
            frames[victim].page = p;
            frames[victim].age = 0;
            frames[victim].ref = true;
        }

        // aging update, shift and  insert refrence
        for (int j = 0; j < numframes; j++) {
            frames[j].age >>= 1;
            if (frames[j].ref)
                frames[j].age |= 0x80;  // set MSB
            frames[j].ref = false;
        }
    }

    return faults;
}

int main(int argc, char* argv[]) {
    // choosefilename
    string fname;
    if (argc > 1) {
        fname = argv[1]; 
    }
    else {
        fname = "pages.txt";  // default file
    }

    // read page ref from file
    vector<int> pages = readp(fname);

    if (pages.empty()) {
        cerr << "No page references found.\n";
        return 1;
    }

    int totalref = pages.size();
    int maxfr = 10;  // changeif need

    cout << "Frames\tFaults_per_1000\n";

    for (int frames = 1; frames <= maxfr; frames++) {
        int faults = simage(pages, frames);  // sim paging
        double rate = (faults * 1000.0) / totalref;

        cout << frames << "\t"
            << fixed << setprecision(2)
            << rate << "\n";
    }

    return 0;
}
