#include <iostream>
#include <filesystem>
#include <map>

using namespace std;
namespace fs = std::filesystem;

int main() {
    string startDir;
    int binWidth;

    // Take input from user
    cout << "Enter directory path: ";
    cin >> startDir;

    cout << "Enter bin width: ";
    cin >> binWidth;

    map<int, int> histogram;
// loop over all files in dir recursively 
    try {
        fs::recursive_directory_iterator it(startDir);
        fs::recursive_directory_iterator end;

        // loop over all files and get size and compite bin index
        while (it != end) {
            if (fs::is_regular_file(it->path())) {
            try {
                    int size = fs::file_size(it->path());
                    int binIndex = size / binWidth;
                    histogram[binIndex]++;
                } catch (...) {
                    // skip files we can't access
                }
            }
            ++it;
        }
    } 
    //outer handles invalid paths
    catch (const fs::filesystem_error& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    // Print histogram
    cout << "\nFile Size Histogram:\n";
// loop thru map n get bin range
    map<int, int>::iterator iter;
    for (iter = histogram.begin(); iter != histogram.end(); iter++) {
        int binIndex = iter->first;
        int count = iter->second;

        int lower = binIndex * binWidth;
        int upper = lower + binWidth - 1;

        cout << lower << " - " << upper << " : ";

        for (int i = 0; i < count; i++) {
            cout << "*";
        }

        cout << " (" << count << ")" << endl;
    }

    return 0;
}