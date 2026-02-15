#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <cstring>

using namespace std;

extern char** environ;

//prompting da user
void promp() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    cout << cwd << " >> ";
}

// converts cmd into tokens and stores token and returns veectorised
vector<string> tokenn(string line) {
    stringstream s(line);
    string word;
    vector<string> tokenn;
    while (s >> word) tokenn.push_back(word);
    return tokenn;
}

// handle if no command entered
bool hbin(vector<string>& tokenn) {
    if (tokenn.empty()) return true;

    string cmd = tokenn[0];

    // cd
    if (cmd == "cd") {
        if (tokenn.size() == 1) {
            char cwd[PATH_MAX];
            getcwd(cwd, sizeof(cwd));
            cout << cwd << endl;
        }
        else {
            if (chdir(tokenn[1].c_str()) != 0)
                perror("cd");
            else {
                char cwd[PATH_MAX];
                getcwd(cwd, sizeof(cwd));
                setenv("PWD", cwd, 1);
            }
        }
        return true;
    }

    // lists directory
    if (cmd == "dir") {
        string dir = (tokenn.size() > 1) ? tokenn[1] : ".";
        system(("ls -al " + dir).c_str());
        return true;
    }

    // prints environ var
    if (cmd == "environ") {
        for (char** env = environ; *env; env++)
            cout << *env << endl;
        return true;
    }

    // set variable
    if (cmd == "set") {
        if (tokenn.size() >= 3)
            setenv(tokenn[1].c_str(), tokenn[2].c_str(), 1);
        else
            cout << "set var value\n";
        return true;
    }

    // echos whatever and loops thru all tokenized commands
    if (cmd == "echo") {
        for (size_t i = 1; i < tokenn.size(); i++)
            cout << tokenn[i] << " ";
        cout << endl;
        return true;
    }

    // when help is input will show everythg the system does
    if (cmd == "help") {
        cout << "Simple Shell Help\n";
        cout << "cd [dir]     Change directory\n";
        cout << "dir [dir]    List directory\n";
        cout << "environ      Show environment\n";
        cout << "set VAR VAL  Set environment variable\n";
        cout << "echo text    Display text\n";
        cout << "pause        Wait for Enter\n";
        cout << "quit         Exit shell\n";
        return true;
    }

    // pauses anything happening
    if (cmd == "pause") {
        cout << "Press Enter to continue...";
        cin.get();
        return true;
    }

    // quit
    if (cmd == "quit") {
        exit(0);
    }

    return false; 
}

// handle external commands and creates and child process
void execy(vector<string> tokenn) {
    bool background = false;

    // check &
    if (!tokenn.empty() && tokenn.back() == "&") {
        background = true;
        tokenn.pop_back();
    }

    pid_t pid = fork();

    if (pid == 0) {
        // redirection with in and outp
        for (size_t i = 0; i < tokenn.size(); i++) {

            // output >
            if (tokenn[i] == ">" || tokenn[i] == ">>") {
                int flags = O_WRONLY | O_CREAT;
                if (tokenn[i] == ">>") flags |= O_APPEND;
                else flags |= O_TRUNC;

                int fd = open(tokenn[i + 1].c_str(), flags, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);

                tokenn.erase(tokenn.begin() + i, tokenn.begin() + i + 2);
                i--;
            }

            // input <
            else if (tokenn[i] == "<") {
                int fd = open(tokenn[i + 1].c_str(), O_RDONLY);
                dup2(fd, STDIN_FILENO);
                close(fd);

                tokenn.erase(tokenn.begin() + i, tokenn.begin() + i + 2);
                i--;
            }
        }

        // convert to char, excecvp to replace process w command
        vector<char*> args;
        for (auto& s : tokenn)
            args.push_back(const_cast<char*>(s.c_str()));
        args.push_back(nullptr);

        execvp(args[0], args.data());
        perror("exec");
        exit(1);
    }
    else {
        if (!background)
            waitpid(pid, NULL, 0);
    }
}

// handles user input
int main(int argc, char* argv[]) {
    istream* ins = &cin;
    ifstream files;

    // batch, read commands from a file
    if (argc == 2) {
        files.open(argv[1]);
        if (!files) {
            perror("batch file");
            return 1;
        }
        ins = &files;
    }

    string line;

    while (true) {
        if (ins == &cin)
            promp();

        if (!getline(*ins, line))
            break;

        vector<string> tokens = tokenn(line);
        if (tokens.empty()) continue;

        if (!hbin(tokens))
            execy(tokens);
    }

    return 0;
}

