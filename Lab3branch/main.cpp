//
//  main.cpp
//  Lab3branch
//
//  Created by Jen Liu on 12/17/16.
//  Copyright © 2016 Jen Liu. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//saturating counter state:
#define STRONGLYTAKEN 3
#define WEAKLYTAKEN 2
#define WEAKLYNOTTAKEN 1
#define STRONGLYNOTTAKEN 0

//class for the saturating counters

class PHT{

public:
    PHT(int m){
        satCounter.resize(pow(2.0, m));
        this->m = m;
        for (int i = 0; i < satCounter.size(); i ++ ) {
            satCounter[i] = STRONGLYTAKEN;
        }
    }
    
    int predict(bitset<32> hex){
        int prediction = satCounter[getAddr(hex)];
        if((prediction == STRONGLYTAKEN) || (prediction == WEAKLYTAKEN))
            return 1;
        else return 0;
    }
    
    //return the correctness of prediction
    bool introspect(bitset<32> hex, string outcome){
        int *prediction;
        prediction = &satCounter[getAddr(hex)];
        if (outcome == "1") {//outcome is 1
            switch (*prediction) {
                case STRONGLYTAKEN:
                    return true;
                    break;
                case WEAKLYTAKEN:
                    *prediction = STRONGLYTAKEN;
                    return true;
                    break;
                case STRONGLYNOTTAKEN:
                    *prediction = WEAKLYNOTTAKEN;
                    return false;
                    break;
                case WEAKLYNOTTAKEN:
                    *prediction = STRONGLYTAKEN;
                    return false;
                    break;
                default:
                    break;
            }
        } else {//out come is 0
            switch (*prediction) {
                case STRONGLYTAKEN:
                    *prediction = WEAKLYTAKEN;
                    return false;
                    break;
                case WEAKLYTAKEN:
                    *prediction = STRONGLYNOTTAKEN;
                    return false;
                    break;
                case STRONGLYNOTTAKEN:
                    return true;
                    break;
                case WEAKLYNOTTAKEN:
                    *prediction = STRONGLYNOTTAKEN;
                    return true;
                    break;
                    
                default:
                    break;
            }
        }
        return true;//this line should not be executed.

    }
    
    
    
    unsigned long getAddr(bitset<32> hex){
        unsigned long addr = hex.to_ulong();
        addr = addr%int(pow(2, m));
        return addr;
    }
private:
    vector<int> satCounter;
    int m;
};

int main(int argc, const char * argv[]) {

    //get m from config
    ifstream lsb;
    int lsbLength;
    lsb.open("/Users/jenliu/Desktop/Lab3branch/Lab3branch/config.txt");//should be argv[1]?
    lsb >> lsbLength;
    
    //initialize PHT and prediction
    PHT pht(lsbLength);
    int prediction;
    bool correctness;
    float missRate;
    int miss = 0;
    int total = 0;
    
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string("/Users/jenliu/Desktop/Lab3branch/Lab3branch/trace.txt") + ".out";//should be argv[2]?
    
    traces.open("/Users/jenliu/Desktop/Lab3branch/Lab3branch/trace.txt");//should be argv[2]?
    tracesout.open(outname.c_str());
    
    string line;
    string outcome;
    string xaddr;
    unsigned int addr;
    bitset<32> accessaddr;
    
    if (traces.is_open()&&tracesout.is_open()){
        while (getline (traces,line)){   // read mem access file and access Cache
            
            istringstream iss(line);
            if (!(iss >> xaddr >> outcome)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
            
            //determine the status of BHR:
            prediction = pht.predict(accessaddr);
            correctness = pht.introspect(accessaddr, outcome);
            if (!correctness)
                miss ++;
            total ++;
            
            
            tracesout<< prediction << endl;  // Output prediction to the output file;
            
            
        }
        
    }
    missRate = (float)miss/(float)total;
    cout << "m = " << lsbLength << endl;
    cout << "miss: "<<  miss << "    total: "<<total << "   missRate:" << missRate << endl;
}
