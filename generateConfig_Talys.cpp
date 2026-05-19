// g++ generateConfig.cpp -o generateConfig `root-config --cflags --glibs` -lm -ldl
//Linker: `root-config --glibs` -lm -ldl -lpthread 
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "TString.h" 

using namespace std;

int main(int argc, char** argv) {

    string finpName = "li6.in.txt";
    ifstream finp(finpName.c_str());
    const char* input_file = "/home/francisco/Desktop/Year_2026/Simulations_MONNET/code/Input_Taly";
    const char* output_file = "/home/francisco/Desktop/Year_2026/Simulations_MONNET/code/Output_Taly";
    const char* executable = "/home/francisco/Desktop/talys/bin/talys";

    if (finp.fail()) {
        cout << "ERROR: Cannot open input file: " << finpName << endl;
        exit(-1);
    }
    int numberenergies = 80;
    vector<double> nEnergies; 

    nEnergies.push_back(0.0001);
    nEnergies.push_back(0.0005);
    nEnergies.push_back(0.001);
    nEnergies.push_back(0.005);
    nEnergies.push_back(0.01);
    nEnergies.push_back(0.05);
    nEnergies.push_back(0.1);
    nEnergies.push_back(0.25);
    nEnergies.push_back(0.5);
    nEnergies.push_back(0.75);

    nEnergies.push_back(1);
    nEnergies.push_back(2);
    nEnergies.push_back(3);
    nEnergies.push_back(4);
    nEnergies.push_back(5);
    nEnergies.push_back(6);
    nEnergies.push_back(7);
    nEnergies.push_back(8);
    nEnergies.push_back(9);

    int size = nEnergies.size();

    for(int i = 1; i < numberenergies - size; i++){
        nEnergies.push_back(i * 10); 
    }
    
    string foutName[nEnergies.size()];
    for (int i = 0; i < nEnergies.size(); i++) {
        foutName[i] = Form("%s/%s_%.4f_keV.in.txt", input_file, finpName.substr(0, finpName.find(".in.txt")).c_str(), nEnergies[i]);
        cout << "Creating file: " << foutName[i] << endl;
        ofstream fout(foutName[i].c_str());
        if (fout.fail()) {
            cout << "ERROR: Cannot open output file: " << foutName[i] << endl;
            exit(-1);
        }
        finp.clear();
        finp.seekg(0);
        string buffer;
        while (getline(finp, buffer)) {
            if (buffer.find("energy ") < buffer.size()) {
                buffer = Form("energy %.9f", nEnergies[i] / 1000);
            }
            fout << buffer << endl;
        }
        fout.close();
    }


    
    string foutNameSh = Form("%s/run_li6_megr_%ld_keV.sh", input_file, nEnergies.size());
    cout << "Creating file: " << foutNameSh << endl;
    ofstream fout(foutNameSh.c_str());
    if (fout.fail()) {
        cout << "ERROR: Cannot open output file: " << foutNameSh << endl;
        exit(-1);
    }
    for (int i = 0; i < nEnergies.size(); i++) {
      string outputfilename = Form("%s/%s_%.4f_keV.out", output_file, finpName.substr(0, finpName.find(".in.txt")).c_str(), nEnergies[i]);
      string command = Form("%s  <%s> %s", executable, foutName[i].c_str() , outputfilename.c_str());
      fout << endl << command << endl << endl;
      fout << "mv " << "nt* " << output_file << Form("/nt_%.4f_keV.L00", nEnergies[i]) << endl;
      fout << "rm " << "n* "  << endl; 
    }
    fout.close();

    return 0;
}
