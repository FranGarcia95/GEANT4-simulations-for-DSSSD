#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "TString.h" 

void Efficiency(){

    const char* output_file = "/home/francisco/Desktop/Year_2026/Simulations_MONNET/code/Output_Simulations_Isotropic";

    TFile* Outputfile;
    Outputfile = new TFile("Efficiency_nTOF_Isotropic.root", "RECREATE");

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

    std::vector<std::string> dsssd = {"SSD_1", "DSSSD_Front_2"};

    TH1D* SSD;
    TH1D* DSSSD;
    TFile* root_file;
    float counts=0;

    std::vector<float> Efficiency_SSD;
    std::vector<float> Efficiency_DSSSD;
    std::vector<float> Error_SSD;
    std::vector<float> Error_DSSSD;

    for(int i = 0; i < nEnergies.size(); i++){
        std::string file_name = Form("%s/LiF_Rectangular_Isotropic_BIF_Mode_1000000_%.4f_keV.root", output_file, nEnergies[i]);
        std::cout << file_name << std::endl;
        root_file = TFile::Open(file_name.c_str(), "READ");
        if (!root_file) {
            cout << "ERROR: Cannot open input file: " << root_file << endl;
            exit(-1);
        }

        root_file->GetObject(dsssd[0].c_str(), SSD);
        SSD->SetDirectory(0);
        root_file->GetObject(dsssd[1].c_str(), DSSSD);
        DSSSD->SetDirectory(0);

        if(!SSD || !DSSSD){
            std::cerr << "Error: Histogram " << dsssd[0].c_str() << " or " << dsssd[1].c_str() << " not found in file!" << std::endl;
        }

        counts = SSD->Integral();
        Efficiency_SSD.push_back(counts*100/1000000);
        Error_SSD.push_back(sqrt(counts)*100/1000000);
        counts = 0;

        counts = DSSSD->Integral();
        Efficiency_DSSSD.push_back(counts*100/1000000);
        Error_DSSSD.push_back(sqrt(counts)*100/1000000);
        counts = 0;

        root_file->Close();

    }

    // ============================
    // Create graphs
    // ============================

    TGraphErrors* grSSD =
        new TGraphErrors(nEnergies.size());

    TGraphErrors* grDSSSD =
        new TGraphErrors(nEnergies.size());

    for(int i = 0; i < nEnergies.size(); i++){

        grSSD->SetPoint(i, nEnergies[i], Efficiency_SSD[i]);
        grSSD->SetPointError(i, 0.0, Error_SSD[i]);

        grDSSSD->SetPoint(i, nEnergies[i], Efficiency_DSSSD[i]);
        grDSSSD->SetPointError(i, 0.0, Error_DSSSD[i]);
    }

    // ============================
    // Style
    // ============================

    grSSD->SetName("Efficiency_SSD");
    grSSD->SetTitle("SSD Efficiency");

    grSSD->GetXaxis()->SetTitle("Neutron Energy (keV)");
    grSSD->GetYaxis()->SetTitle("Efficiency");

    grSSD->SetMarkerStyle(20);
    grSSD->SetLineWidth(2);

    grDSSSD->SetName("Efficiency_DSSSD");
    grDSSSD->SetTitle("DSSSD Efficiency");

    grDSSSD->GetXaxis()->SetTitle("Neutron Energy (keV)");
    grDSSSD->GetYaxis()->SetTitle("Efficiency");

    grDSSSD->SetMarkerStyle(21);
    grDSSSD->SetLineWidth(2);

    // ============================
    // Save to ROOT file
    // ============================

    Outputfile->cd();

    grSSD->Write();
    grDSSSD->Write();

    Outputfile->Write();
    Outputfile->Close();

    std::cout << "Efficiency graphs saved in "
              << "Efficiency_nTOF.root"
              << std::endl;

    Outputfile->Write();
    Outputfile->Close();
}