#include <iostream>
#include <vector>
#include <unordered_map>

#include "scanner.h"

using namespace std;

int main(){
    Scanner scanner = Scanner();
    unordered_map<string, uint32_t> labels{};

    string line;
    bool scanError = false;
    // scan file using scanner
    for(getline(cin, line); !cin.fail(); getline(cin, line)){
        scanner.scanLine(line);
        scanError |= scanner.error;
        scanner.error = false;
    }
    // assign label tokens to label map
    for(Token label : *scanner.getLabelsAddr()){
        if(labels.find(label.name) != labels.end()){
            scanError = true;
            cerr << "[Line " << label.line << "] Label '" << label.name << "' has already been assigned.";
            continue;
        }
        labels.emplace(label.name, label.line);
    }
    // !TODO fix operator tokens that required some Label

    for(Token instr : *scanner.getTokensAddr()){
        cout << "0x" << hex << instr.data << endl;
    }
}

