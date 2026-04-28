#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TString.h>  

std::map<int, TH1F*> SSD;
std::map<int, TH1F*> DSSSD;
std::map<int, TH1F*> Edep;
std::map<int, TH1F*> Edep_SSD;
std::map<int, TH1F*> Edep_DSSSD;

int ToltalNumberDetectors = 2;

void Coincidences(std::vector<std::vector<float>> information);

void GEANT4_Analysis_n_TOF(){
    std::string file_name = "./build/Circular_Anisotropic_BIF_1000000_45_degrees_3MeV.root";
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
    OutputAdress = new TFile("Test_n_TOF.root", "RECREATE");
    for(int i = 0; i < ToltalNumberDetectors; i++){
        if(i == 0){
            SSD[i] = new TH1F(Form("SSD #%d", i + 1), Form("SSD #%d", i + 1), 16, 0, 16);
            SSD[i]->SetDirectory(OutputAdress);
        }else if(i == 1){
            DSSSD[i] = new TH1F(Form("DSSSD Front #%d", i + 1), Form("DSSSD Front #%d", i + 1), 16, 0, 16);
            DSSSD[i]->SetDirectory(OutputAdress);
            DSSSD[i+1] = new TH1F(Form("DSSSD Back #%d", i + 1), Form("DSSSD Back #%d", i + 1), 16, 0, 16);
            DSSSD[i+1]->SetDirectory(OutputAdress);
        }
    }

    Edep[0] = new TH1F(Form("Deposited energy Both detectors #%d", 1), Form("Deposited energy Both detectors #%d", 1), 10000, 0, 20.0);
    Edep[0]->SetDirectory(OutputAdress);
    Edep_SSD[0] = new TH1F(Form("Deposited energy SSD #%d", 1), Form("Deposited energy SSD #%d", 1), 10000, 0, 20.0);
    Edep_SSD[0]->SetDirectory(OutputAdress);
    Edep_SSD[1] = new TH1F(Form("Deposited energy coincidence SSD #%d", 1), Form("Deposited energy coincidence SSD #%d", 1), 10000, 0, 20.0);
    Edep_SSD[1]->SetDirectory(OutputAdress);
    Edep_DSSSD[0] = new TH1F(Form("Deposited energy DSSSD #%d", 1), Form("Deposited energy DSSSD #%d", 1), 10000, 0, 20.0);
    Edep_DSSSD[0]->SetDirectory(OutputAdress);

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
            if(side != 2){
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
            if(side != 2){
                info.push_back(auxinfo);
            }
            auxinfo.clear();
        }
        if(side == 0){
            SSD[det]->Fill(strip);
        }else if(side == 1){
            DSSSD[det]->Fill(strip);
        }else if(side == 2){
            DSSSD[det + 1]->Fill(strip);
        }else{
            continue;
        }
    }

    OutputAdress->Write();
    OutputAdress->Close();
}

void Coincidences(std::vector<std::vector<float>> information){

    if(information.size() == 1 && information[0][0] == 0){
        Edep_SSD[information[0][0]]->Fill(information[0][1] / 1000);
        Edep[0]->Fill(information[0][1] / 1000);
    }else if(information.size() > 1){
        for(int i = 0; i < information.size(); i++){
            if(information[i][0] == 0){
                Edep_SSD[1]->Fill(information[i][1] / 1000);
                Edep[0]->Fill(information[i][1] / 1000);
            }else if(information[i][0] == 1){
                Edep_DSSSD[0]->Fill(information[i][1] / 1000);
                Edep[0]->Fill(information[i][1] / 1000);
            }
        }
    }

}