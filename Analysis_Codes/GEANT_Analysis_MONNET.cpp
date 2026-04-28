#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TString.h>   

std::map<int, TH1F*> DSSSD;
std::map<int, TH1F*> Edep;
std::map<int, TH1F*> Edep_One;
std::map<int, TH1F*> Edep_Both;

int ToltalNumberDetectors = 2;

void Coincidences(std::vector<std::vector<float>> information);

void GEANT4_Analysis_MONNET(){
    std::string file_name = "./build/Circular_Anisotropic_BIF_1000000_0_degrees_3MeV_MONNET.root";
    std::ifstream file_check(file_name);

    if (!file_check.good()) {
        std::cout << "File does not exist." << std::endl;
        return;
    }

    TFile* file = new TFile(file_name.c_str(), "READ");

    //We get the ttree generated in the simulations
    TTree* tree = (TTree*)file->Get("DSSSD");

    if (!tree) {
        std::cerr << "Error: DSSSD tree not found!" << std::endl;
        file->Close();
        //delete file;
        return;
    }

    //We define the histogram to store the number of counts per strip
    TFile* OutputAdress;
    OutputAdress = new TFile("Test_MONNET.root", "RECREATE");
    for(int i = 0; i < ToltalNumberDetectors; i++){
        DSSSD[i*2] = new TH1F(Form("DSSSD Front #%d", i + 1), Form("DSSSD Front #%d", i + 1), 16, 0, 16);
        DSSSD[i*2]->SetDirectory(OutputAdress);
        DSSSD[i*2+1] = new TH1F(Form("DSSSD Back #%d", i + 1), Form("DSSSD Back #%d", i + 1), 16, 0, 16);
        DSSSD[i*2+1]->SetDirectory(OutputAdress);
    }

    Edep[0] = new TH1F(Form("DSSSD total deposited energy #%d", 1), Form("DSSSD total deposited energy #%d", 1), 10000, 0, 20.0);
    Edep[0]->SetDirectory(OutputAdress);
    Edep[1] = new TH1F(Form("DSSSD total deposited energy #%d", 2), Form("DSSSD total deposited energy #%d", 2), 10000, 0, 20.0);
    Edep[1]->SetDirectory(OutputAdress);
    Edep_One[0] = new TH1F(Form("DSSSD deposited energy w/o coincidence #%d", 1), Form("DSSSD deposited energy w/o coincidence #%d", 1), 10000, 0, 20.0);
    Edep_One[0]->SetDirectory(OutputAdress);
    Edep_One[1] = new TH1F(Form("DSSSD deposited energy w/o coincidence #%d", 2), Form("DSSSD deposited energy w/o coincidence #%d", 2), 10000, 0, 20.0);
    Edep_One[1]->SetDirectory(OutputAdress);
    Edep_Both[0] = new TH1F(Form("DSSSD deposited energy w coincidence #%d", 1), Form("DSSSD deposited energy w coincidence #%d", 1), 10000, 0, 20.0);
    Edep_Both[0]->SetDirectory(OutputAdress);

    int event, det, strip, side, particleID;
    double energy;
    float *content;

    tree->SetBranchAddress("EventN", &event);
    tree->SetBranchAddress("det", &det);
    tree->SetBranchAddress("strip", &strip);
    tree->SetBranchAddress("Side", &side);
    tree->SetBranchAddress("edep", &energy);
    tree->SetBranchAddress("particle", &particleID);

    std::vector<std::vector<float>> info;
    std::vector<float> auxinfo;
    int auxevt, firstevent;
    tree->GetEntry(0);
    firstevent = event;
    auxevt = firstevent;
    
    for(int i = 0; i < tree->GetEntries(); i++){
        tree->GetEntry(i);
        firstevent = event;
        if(firstevent == auxevt){
            auxinfo.push_back(det);
            auxinfo.push_back(energy);
            if(side == 1){
                info.push_back(auxinfo);
            }
            auxinfo.clear();
        }else if(firstevent != auxevt){
            Coincidences(info);
            auxevt = firstevent;
            info.clear();
            auxinfo.push_back(det);
            auxinfo.push_back(energy);
            auxinfo.push_back(particleID);
            if(side == 1){
                info.push_back(auxinfo);
            }
            auxinfo.clear();
        }
        if(side == 1){
            DSSSD[det*2]->Fill(strip);
        }else if(side == 2){
            DSSSD[det*2 + 1]->Fill(strip);
        }else{
            continue;
        }
    }

    OutputAdress->Write();
    OutputAdress->Close();
}

void Coincidences(std::vector<std::vector<float>> information){

    if(information.size() == 1){
        Edep_One[information[0][0]]->Fill(information[0][1] / 1000);
        Edep[information[0][0]]->Fill(information[0][1] / 1000);
    }else if(information.size() == 2){
        Edep_Both[0]->Fill(information[0][1] / 1000);
        Edep_Both[0]->Fill(information[1][1] / 1000);
        Edep[information[0][0]]->Fill(information[0][1] / 1000);
        Edep[information[1][0]]->Fill(information[1][1] / 1000);
    }

}