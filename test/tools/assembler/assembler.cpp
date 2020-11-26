#include <iostream>
#include <vector>
#include <unordered_map>

#include "scanner.h"
#include "scanner.cpp"

using namespace std;

int main(){
    Scanner scanner = Scanner();
    unordered_map<string, uint32_t> labels{};

    string line;
    bool scanError = false;
    for(getline(cin, line); !cin.fail(); getline(cin, line)){
        scanner.scanLine(line);
        scanError |= scanner.error;
        scanner.error = false;
    }

    int a = 1;
    a = a + a;

    vector<uint32_t> mem{};
}

