#include "bifprobability.hh"

BifProbability::BifProbability(){
    
}

BifProbability::~BifProbability()
{}

std::vector<std::pair<G4float, G4float>> BifProbability::Get_BIF_Probability(G4String filename){
    
    std::ifstream in(filename);
    if (!in.good()) {
        G4cout << " the BIF file  " << filename
               << " Does not exist" << G4endl;
        G4cout << " #########  Error in " << __FILE__ << ", line " << __LINE__
               << " #########" << G4endl;
        exit(-1);
    }

    
    const int bins = 300;
    const double rmax = 3.0;

    std::vector<G4double> pdf(bins, 0.0);

    G4double dr = rmax / bins;
    G4float sampling = 0.0;

    std::string buffer;
    while (std::getline(in, buffer)) {
        std::istringstream b(buffer);

        if(buffer.find("#xstep ") == 0){
            std::string tag;
            b >> tag >> sampling;
            if(sampling > 0.02){
                G4cout << "Sampling must be smaller, at least 0.01, your " << sampling
                       << " Go to run your simulations with the TC again. " << G4endl;
                exit(-1);
            }
        }
        if(buffer.find("#") == 0) continue;

        G4double x0, y0, weight, dummy;
        b >> x0 >> y0 >> weight >> dummy;

        G4double r = std::sqrt(x0*x0 + y0*y0);

        if (r >= rmax) continue;
        G4int bin = r / dr;

        //G4cout << bin << G4endl;

        if (r < 3.0) {
            pdf[bin] += weight;
        }
    }

    // Normalize properly as PDF
    G4double sum = 0.0;
    for (int i = 0; i < bins; i++) {
        sum += pdf[i];
    }

    for (int i = 0; i < bins; i++) {
        pdf[i] /= sum * dr;
    }

    G4double cumulative = 0.0;

    for (int i = 0; i < bins; i++) {

        cumulative += pdf[i] * dr;   // THIS is correct CDF

        bif.push_back({(i + 0.5) * dr * 10, cumulative});

    }

    return bif;

}

std::vector<std::pair<G4float, G4float>> BifProbability::Get_Angular_Distribution(G4String filename){
    
    std::ifstream in(filename);
    if (!in.good()) {
        G4cout << " the Angular Distribution file  " << filename
               << " Does not exist" << G4endl;
        G4cout << " #########  Error in " << __FILE__ << ", line " << __LINE__
               << " #########" << G4endl;
        exit(-1);
    }

    std::vector<G4double> entries;
    std::vector<G4double> pdf;

    G4float sampling = 0.0;

    std::string buffer;
    while (std::getline(in, buffer)) {
        std::istringstream b(buffer);

        if(buffer.find("#     entries:  ") == 0){
            G4cout << buffer << G4endl;
            std::string tag;
            b >> tag >> tag >> sampling;
            if(sampling > 0.02){
                G4cout << "Sampling must be smaller, at least 90, your " << sampling
                       << " Go to run your taly simulation again. " << G4endl;
                exit(-1);
            }
        }
        if(buffer.find("#") == 0) continue;

        G4double angle, xs, dummy;
        b >> angle >> xs >> dummy >> dummy;
        G4double theta_rad = CLHEP::pi * angle / 180.0;
        G4double cos_tetha = std::cos(theta_rad);

        angraw.push_back({cos_tetha, xs*sin(theta_rad)/2}
    );

    }

    // Normalize properly as PDF
    G4double sum = 0.0;
    for (int i = 0; i < angraw.size(); i++) {
        sum += angraw[i][1];
    }

    for (int i = 0; i < angraw.size(); i++) {
        angraw[i][1] /= sum;
    }

    G4double cumulative = 0.0;

    for (int i = 0; i < angraw.size(); i++) {

        cumulative += angraw[i][1];   // THIS is correct CDF

        ang.push_back({angraw[i][0], cumulative});

    }

    return ang;

}

