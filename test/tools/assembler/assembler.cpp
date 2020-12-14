#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>

#include "scanner.h"

using namespace std;

int main(){
    Scanner scanner = Scanner();
    unordered_map<string, uint32_t> labels{};

    string line;
    // boolean to keep track of if error occured at all (not necessary but useful if wanting to test later)
    bool scanError = false;
    // scan file using scanner
    for(getline(cin, line); !cin.fail(); getline(cin, line)){
        if(line == "asd") break; // allow for exiting loop when debugging
        scanner.scanLine(line);
        scanError |= scanner.error;
        scanner.error = false;
    }
    // assign label tokens to label map
    for(Token label : *scanner.getLabelsAddr()){
        if(labels.find(label.name) != labels.end()){ // if label already exists in table then reoirt error
            scanError = true;
            cerr << "Error: [Line " << label.line << "] Label '" << label.name << "' has already been assigned." << endl;
            continue;
        }
        labels.emplace(label.name, label.memLine);
    }
    // fix instructions that used labels
    for(auto it = scanner.getTokensAddr()->begin(); it != scanner.getTokensAddr()->end(); it++){
        if(it->label != ""){ // if label does not exists than label field will be empty
            // check if label exists
            if(labels.find(it->label) == labels.end()){
                scanError = true;
                cerr << "Error: [Line " << it->line << "] Label '" << it->label << "' doesn't exist." << endl;
            }
            else{
                uint32_t memLine = labels.at(it->label);
                uint32_t op = it->data;
                op >>= 26;
                if(op == 0b000010 || op == 0b000011){ // if jump pattern
                    uint32_t compareMem = ((memLine >> 2) << 6) >> 6;
                    it->data |= compareMem;
                }
                else{
                    uint32_t jumpDiff = (int32_t)(memLine - (it->memLine + 4)) >> 2; // adjust difference to be by instruction and not by byte
                    uint16_t checkLineSize = jumpDiff;
                    uint32_t checkLineSizeSignExtended = checkLineSize | (0xffff0000 & (-(checkLineSize>>15)));
                    // cout << hex << (checkLineSize>>15) << " " << checkLineSize << " " << checkLineSizeSignExtended << " " << jumpDiff << endl;
                    if(checkLineSizeSignExtended != jumpDiff){  // check if jump firs in 16 bits
                        scanError = true;
                        cerr << "Error: [Line " << it->line << "] Label '" << it->label << "' is too far away from current instructions, jump is too large. Max jump size stored in 16 bits, calculated difference of: 0x" << hex << jumpDiff << "." << endl;
                    }
                    else{
                        it->data |= checkLineSize;
                    }
                }
            }
        }
    }

    // cout generated file (if error first output is "ERROR:")
    if(scanError) cout << "ERROR:" << endl;
    for(Token instr : *scanner.getTokensAddr()){
        cout << hex << setfill('0') << setw(8) << instr.data << endl;
    }
}

