
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <pthread.h>
#include <algorithm>
#include <cctype>

using namespace std;

// wordcount nd mutex
map<string, int> gfreq;
pthread_mutex_t mutex1;

// pass data 2 thread
struct ThreadData {
    vector<string> linesproc;
};

// normalise words, all lowercase andno punct
string normal(const string& word) {
    string w = word;
    transform(w.begin(), w.end(), w.begin(), ::tolower);
    w.erase(remove_if(w.begin(), w.end(), ::ispunct), w.end());
    return w;
}

// threading
void* countWords(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    map<string, int> lfreq;

    // loop to count words
    for (int i = 0; i < data->linesproc.size(); i++) {
        string line = data->linesproc[i];  
        stringstream ss(line);
        string word;
        // while loop to clean da word
        while (ss >> word) {
            word = normal(word);  
            if (!word.empty()) {
                lfreq[word]++;   
            }
        }
    }
    // merge local to gloabal
    pthread_mutex_lock(&mutex1);
    for (map<string, int>::const_iterator it = lfreq.begin(); it != lfreq.end(); ++it) {
        gfreq[it->first] += it->second;
    }
    pthread_mutex_unlock(&mutex1);

    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    string fname = "input.txt";
    if (argc >= 2) fname = argv[1];

    ifstream file(fname);
    if (!file) {
        cerr << "Cannot open " << fname << endl;
        return 1;
    }

    // read file into liness
    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }

    // num of threads
    const int N = 4;
    int total = lines.size();

    if (total == 0) {
        cerr << "No lines to process in " << fname << endl;
        return 0;
    }

    int tcount = min(N, total);
    int csize = total / tcount;

    vector<pthread_t> threads(tcount);
    vector<ThreadData> threadData(tcount);

    pthread_mutex_init(&mutex1, NULL);

    // prep da data and create the threads
    for (int i = 0; i < tcount; i++) {
        int start = i * csize + min(i, total % tcount);
        int size = csize + (i < (total % tcount) ? 1 : 0);
        int end = min(total, start + size);
        threadData[i].linesproc.assign(lines.begin() + start, lines.begin() + end);

        pthread_create(&threads[i], NULL, countWords, &threadData[i]);
    }

    // wait for threads to finishj
    for (int i = 0; i < tcount; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex1);

    // final wordcount
    cout << "Final Word Count:\n";
    for (map<string, int>::const_iterator it = gfreq.begin(); it != gfreq.end(); ++it) {
        cout << it->first << " : " << it->second << endl;
    }

    return 0;
}

