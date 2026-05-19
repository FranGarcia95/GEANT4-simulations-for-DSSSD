// g++ generateConfig.cpp -o generateConfig `root-config --cflags --glibs` -lm -ldl
//Linker: `root-config --glibs` -lm -ldl -lpthread 
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "TString.h" 

using namespace std;

int main(int argc, char** argv) {

    string finpName = "UserInput_Test";
    const char* input_file = "/home/francisco/Desktop/Year_2026/Simulations_MONNET/code/User_Input";
    const char* output_file = "/home/francisco/Desktop/Year_2026/Simulations_MONNET/code/Output_Simulations";
    const char* executable = "/home/francisco/Desktop/Year_2026/Simulations_MONNET/code/build";
    const char* anisotropy = "/home/francisco/Desktop/Year_2026/Simulations_MONNET/code/Output_Taly";
    ifstream finp(Form("%s/%s", input_file, finpName.c_str()));

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
        foutName[i] = Form("%s/%s_LiF_%.4f_keV", input_file, finpName.c_str(), nEnergies[i]);
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
            if(buffer.find("energy ") < buffer.size()) {
                buffer = Form("energy %.4f", nEnergies[i]);
            }else if(buffer.find("energyspread ") < buffer.size()) {
                buffer = Form("energyspread %.4f", nEnergies[i]/100);
            }else if(buffer.find("angulardistribution ") < buffer.size()){
                buffer = "angulardistribution Anisotropic";
            }else if(buffer.find("AngularDistributionfile ") < buffer.size()){
                buffer = Form("AngularDistributionfile %s/nt_%.4f_keV.L00", anisotropy, nEnergies[i]);
            }else if(buffer.find("outputFilename ") < buffer.size()){
                buffer = Form("outputFilename %s/LiF_Rectangular_Anisotropic_BIF_Mode_1000000_%.4f_keV.root", output_file, nEnergies[i]);
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
      string command = Form("%s/myDSSSD -i %s -t 2", executable, foutName[i].c_str());
      fout << endl << command << endl;
    }
    fout.close();

    return 0;
}
