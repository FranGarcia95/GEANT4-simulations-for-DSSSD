#include "Randomize.hh"
#include "generator.hh"
#include <cmath>
#include <iostream>
#include <iomanip>
#include "event.hh"           
#include "G4EventManager.hh"  
#include "G4Geantino.hh"
#include "G4ParticleTable.hh"
#include "G4AnalysisManager.hh"  
#include "G4IonTable.hh"

MyPrimaryGenerator::MyPrimaryGenerator(DSSSDUserInput* u): AngularEmmission("Isotropic")
{

    userInp = u;

    fParticleGun = new G4ParticleGun(1);
    
    G4String particleName = userInp->Get_Particle();
    particle = particleTable->FindParticle(particleName);
    G4String framereference = userInp->Get_Reference_Frame();
    if(framereference == "LabFramefromCMFrame_Simultaneous"){
        secondaryparticle = particleTable->FindParticle(userInp->Get_Secondary_Particle());
    }
    
    fParticleGun->SetParticleDefinition(particle);

    if (userInp->Get_Emission_Mode() == "B") {
        G4cout << "doing Emission B with " << userInp->Get_BIF_Filename()
               << G4endl;
        G4cout << " The BIF file has to be in cm........." << G4endl;
        
        string filename = userInp->Get_BIF_Filename();
        BifProbability* obj = new BifProbability();
        bif = obj->Get_BIF_Probability(filename);
        
    }

    if (userInp->Get_Angular_Distribution() == "Anisotropic") {
        G4cout << "doing Angular Distribution Anisotropic with " << userInp->Get_Ang_File_Name()
               << G4endl;
        G4cout << " The Angular Distribution file has to be in degrees........." << G4endl;
        std::ifstream inAD(userInp->Get_Ang_File_Name());
        
        string filename = userInp->Get_Ang_File_Name();
        BifProbability* obj = new BifProbability();
        angdist = obj->Get_Angular_Distribution(filename);
    }
}
    
MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{    
    G4String targetshape = userInp->Get_TargetShape();
    G4float diameter, thickness;
    G4float targetRadius = 0.0, targetHeight = 0.0, targetWidth = 0.0;
    if(targetshape == "Circular"){
        userInp->Get_TargetCircular(diameter, thickness);
        targetRadius = diameter * mm / 2;
    }else if(targetshape == "Rectangular"){
        userInp->Get_TargetRectangular(targetHeight, targetWidth, thickness);
    }else{
        G4cout << "ERROR: Wrong target shape selected" << G4endl << "       Word "
               << targetshape << " does not exist" << G4endl;
        exit(-1);
    }

    // change the target shape to circle
    // diameter=3cm

    G4String referenceFrame = userInp->Get_Reference_Frame();
    G4String emissionMode = userInp->Get_Emission_Mode();
    G4ThreeVector vorigin, vdirection;
    G4ThreeVector worldDir;
    G4double E_Particle;

    G4float incidentparticle, targetnucleus, ejectedparticle, residualnucleus;
    userInp->Return_particleinfo(incidentparticle, targetnucleus, ejectedparticle, residualnucleus);

    G4float M_b = ejectedparticle; // mass of emitted particle (proton)
    G4float M_B = residualnucleus; // mass of residual nucleus (Cl-35 after proton emission)
    M_a = incidentparticle; // mass of incident particle (neutron)
    M_A = targetnucleus; // mass of target nucleus (Cl-35)
    
    G4double E_a = userInp->Get_Particle_Energy() * keV; // energy of incident particle
    G4double eneSigma = userInp->Get_Particle_Energy_Spread() * keV;
    E_Incident  = G4RandGauss::shoot(E_a, eneSigma);
    if (E_a < 0) E_Incident = E_a;
    Qreaction = (M_a + M_A - M_b - M_B) * 931.494061;

    // Apply same rotation as geometry
    G4RotationMatrix rot;
    G4float TargetRotationx, TargetRotationy, TargetRotationz;
    userInp->Return_Rotation(TargetRotationx, TargetRotationy, TargetRotationz);
    rot.rotateX(TargetRotationx * deg);
    rot.rotateY(TargetRotationy * deg);
    rot.rotateZ(TargetRotationz * deg);

    auto analysisManager = G4AnalysisManager::Instance();
    // random point in circle 
    if(emissionMode == "H" && targetshape == "Circular"){
        float r, z, theta;
        r = targetRadius * mm * std::sqrt(G4UniformRand());  
        z = thickness * mm * G4UniformRand() - thickness * mm / 2;
        theta = CLHEP::twopi * G4UniformRand();        
    
        G4ThreeVector localPos(r * std::cos(theta),
                               r * std::sin(theta),
                               z);

        vorigin = rot * localPos;

    }else if(emissionMode == "H" && targetshape == "Rectangular"){

        G4ThreeVector localPos(targetHeight * mm * G4UniformRand() - targetHeight * mm / 2,
                               targetWidth * mm * G4UniformRand() - targetWidth * mm / 2,
                               thickness * mm * G4UniformRand() - thickness * mm / 2);

        vorigin = rot * localPos;

    }else if(emissionMode == "B" && targetshape == "Circular"){
        float x0, y0, z0;
        float deltax0, deltay0;
        G4double radius;
        G4double theta = CLHEP::twopi * G4UniformRand();
        userInp->Get_MissAligment(deltax0, deltay0);
        do {
            const auto p = G4UniformRand();
            for (auto i = 1; i < bif.size(); ++i) {
                const auto [r_max, p_max] = bif.at(i);
                if (p_max > p) {
                    const auto [r_min, p_min] = bif.at(i - 1);
                    radius = r_min + (p - p_min) * (r_max - r_min) / (p_max - p_min);
                    break;
                }
            }
            x0 = radius * mm * cos(theta) + deltax0;
            y0 = radius * mm * sin(theta) + deltay0;
        } while (sqrt(x0*x0 + y0*y0) > targetRadius * mm);

        z0 = thickness * mm * G4UniformRand() - thickness * mm / 2;          

        G4ThreeVector localPos(x0, y0, z0);

        vorigin = rot * localPos;

    }else if(emissionMode == "B" && targetshape == "Rectangular"){

        float x0, y0, z0;
        float deltax0, deltay0;
        userInp->Get_MissAligment(deltax0, deltay0);
        G4double radius;
        do {
            const auto p = G4UniformRand();
            for (auto i = 1; i < bif.size(); ++i) {
                const auto [r_max, p_max] = bif.at(i);
                if (p_max > p) {
                    const auto [r_min, p_min] = bif.at(i - 1);
                    radius = r_min + (p - p_min) * (r_max - r_min) / (p_max - p_min);
                    break;
                }
            }

            G4double theta = CLHEP::twopi * G4UniformRand();
            x0 = radius * cos(theta) + deltax0;
            y0 = radius * sin(theta) + deltay0;
        } while (std::abs(x0) > targetHeight / 2.0 || std::abs(y0) > targetWidth / 2.0);

        z0 = thickness * G4UniformRand() - thickness / 2;

        G4ThreeVector localPos(x0, y0, z0);

        vorigin = rot * localPos;

    }else if (emissionMode == "Decay") {  // Cs137
    float deltax0, deltay0;
        // Simulation of Source Sample generation Inside Sample
        // Position:
        G4int theA = userInp->Get_ADecayIsotope();
        G4int theZ = userInp->Get_ZDecayIsotope();

        G4double ExcitationEnergy = 0;
        G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(theZ, theA, ExcitationEnergy);
        fParticleGun->SetParticleDefinition(ion);
        fParticleGun->SetParticleEnergy(0 * eV);

        userInp->Get_MissAligment(deltax0, deltay0);

        G4ThreeVector localPos(deltax0, deltay0, 0);

        vorigin = localPos;
        fParticleGun->SetParticlePosition(vorigin);

        fParticleGun->GeneratePrimaryVertex(anEvent);
        
    }else{
        G4cout << "ERROR: Wrong emission mode selected" << G4endl << "       Word "
               << emissionMode << " does not exist" << G4endl;
        exit(-1);
    }

    G4double x1 = vorigin.x();
    G4double y1 = vorigin.y();
    G4double z1 = vorigin.z();

    analysisManager->FillH2(0, x1, y1);
    analysisManager->FillH2(1, x1, z1);
    analysisManager->FillH2(2, y1, z1);

    if(referenceFrame == "LabFramefromCMFrame"){

        phi      = CLHEP::twopi * G4UniformRand();

        //We include the anisotropy or isotropy here
        AngularEmmission = userInp->Get_Angular_Distribution();
        if(AngularEmmission == "Isotropic"){
            theta_frame = acos(1. - 2. * G4UniformRand());
        }else if(AngularEmmission == "Anisotropic"){
            G4double angle;
            do {
                const auto p = G4UniformRand();
                for (auto i = 1; i < angdist.size(); ++i) {
                    const auto [rad_max, p_max] = angdist.at(i);
                    if (p_max > p) {
                        const auto [rad_min, p_min] = angdist.at(i - 1);
                        angle = rad_min + (p - p_min) * (rad_max - rad_min) / (p_max - p_min);
                        break;
                    }
                }

            } while (angle > 1. || angle < -1. );

            theta_frame = acos(angle);   // uniform in [-1,1]
            //G4cout << theta_frame << G4endl;
        }else{
            G4cout << "ERROR: Wrong AngularEmmission selected" << G4endl << "       Word "
                   << AngularEmmission << " does not exist" << G4endl;
            exit(-1);
        }

        theta_lab = Angle_Convertion(M_b, M_B, theta_frame);  // Lab frame angle
    
        //Proton energy in lab frame
        E_Particle = Kinematic_Energy(M_b, M_B);
    
        G4int evt = anEvent->GetEventID();

        if (!std::isfinite(theta_frame)) {
            G4cerr << "theta_frame is NaN!" << G4endl;
        }
        
        // Set proton direction in lab frame using theta_lab and phi_cm
        dx = std::sin(theta_lab) * std::cos(phi);
        dy = std::sin(theta_lab) * std::sin(phi);
        dz = std::cos(theta_lab);

        worldDir = G4ThreeVector(dx, dy, dz);
    
        //G4cout << dx << "   " << dy << "   " << dz << G4endl;
        /*G4RotationMatrix rot;
        rot.rotateY(-1 * userInp->Get_Target_Rotation() * deg);

        worldDir = rot * worldDir;*/  

        fParticleGun->SetParticleMomentumDirection(worldDir);

                // theta_lab
        auto man = G4AnalysisManager::Instance();
        man->FillNtupleIColumn(1, 0, evt);            
        man->FillNtupleDColumn(1, 1, theta_lab/deg);      
        man->FillNtupleDColumn(1, 2, theta_frame/deg);  
        // man->FillNtupleDColumn(5, 3, E_Particle/MeV);  
        // man->FillNtupleDColumn(5, 4, Qreaction);     
    
        man->AddNtupleRow(1);

        fParticleGun->SetParticlePosition(vorigin);
        fParticleGun->SetParticleEnergy(E_Particle);
        
        fParticleGun->GeneratePrimaryVertex(anEvent);

    }else if(referenceFrame == "LabFramefromCMFrame_Simultaneous"){

        phi      = CLHEP::twopi * G4UniformRand();

        //We include the anisotropy or isotropy here
        AngularEmmission = userInp->Get_Angular_Distribution();
        if(AngularEmmission == "Isotropic"){
            theta_frame = acos(1. - 2. * G4UniformRand());
        }else if(AngularEmmission == "Anisotropic"){
            G4double angle;
            do {
                const auto p = G4UniformRand();
                for (auto i = 1; i < angdist.size(); ++i) {
                    const auto [rad_max, p_max] = angdist.at(i);
                    if (p_max > p) {
                        const auto [rad_min, p_min] = angdist.at(i - 1);
                        angle = rad_min + (p - p_min) * (rad_max - rad_min) / (p_max - p_min);
                        break;
                    }
                }

            } while (angle > 1. || angle < -1. );

            theta_frame = acos(angle);   // uniform in [-1,1]
            //G4cout << theta_frame << G4endl;
        }else{
            G4cout << "ERROR: Wrong AngularEmmission selected" << G4endl << "       Word "
                   << AngularEmmission << " does not exist" << G4endl;
            exit(-1);
        }

        theta_lab = Angle_Convertion(M_b, M_B, theta_frame);  // Lab frame angle  
        G4float theta_lab_primary = theta_lab;      
        
        //Proton energy in lab frame
        //First Particle
        fParticleGun->SetParticleDefinition(particle);
        E_Particle = Kinematic_Energy(M_b, M_B);

        G4int evt = anEvent->GetEventID();
        // theta_lab
        auto man = G4AnalysisManager::Instance();
        man->FillNtupleIColumn(1, 0, evt);            
        man->FillNtupleDColumn(1, 1, theta_lab/deg);
        man->FillNtupleDColumn(1, 2, theta_frame/deg); 
        man->AddNtupleRow(1);
    
        // Calculate theta_cm
        //G4double theta_cm = std::acos(costheta);
            
        // Set proton direction in lab frame using theta_lab and phi_cm
        dx = std::sin(theta_lab) * std::cos(phi);
        dy = std::sin(theta_lab) * std::sin(phi);
        dz = std::cos(theta_lab);
    
        worldDir = G4ThreeVector(dx, dy, dz);   

        fParticleGun->SetParticleMomentumDirection(worldDir);

        fParticleGun->SetParticleEnergy(E_Particle);
        
        fParticleGun->GeneratePrimaryVertex(anEvent);


        //Second particle emmission

        fParticleGun->SetParticleDefinition(secondaryparticle);

        theta_lab = Angle_Convertion(M_B, M_b, CLHEP::pi - theta_frame);  // Lab frame angle
        G4float theta_lab_secondary = theta_lab;
        E_Particle = Kinematic_Energy(M_B, M_b);

        man->FillH2(3, theta_lab_primary/deg, theta_lab_secondary/deg);

        man->FillNtupleIColumn(2, 0, evt);            
        man->FillNtupleDColumn(2, 1, theta_lab/deg);
        man->FillNtupleDColumn(2, 2, theta_frame/deg); 
        man->AddNtupleRow(2);

        dx = std::sin(theta_lab) * std::cos(phi + CLHEP::pi);
        dy = std::sin(theta_lab) * std::sin(phi + CLHEP::pi);
        dz = std::cos(theta_lab);


    
        worldDir = G4ThreeVector(dx, dy, dz);   

        fParticleGun->SetParticleMomentumDirection(worldDir);

        fParticleGun->SetParticleEnergy(E_Particle);

        fParticleGun->SetParticlePosition(vorigin);
        
        fParticleGun->GeneratePrimaryVertex(anEvent);

    }else if (referenceFrame == "LabFrame") {  // Cs137

        G4double theta = acos(2.0 * G4UniformRand() - 1.0);
        phi   = 2.0 * CLHEP::pi * G4UniformRand();

        G4double px = sin(theta) * cos(phi);
        G4double py = sin(theta) * sin(phi);
        G4double pz = cos(theta);

        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px, py, pz));

        fParticleGun->GeneratePrimaryVertex(anEvent);
        
    }else {
        G4cout << "ERROR: Wrong frame selected" << G4endl << " "
               << referenceFrame << " does not exist" << G4endl;
        exit(-1);
    }

    auto info = new PrimaryParticleInformation(vorigin, theta_lab, theta_frame);
    anEvent->GetPrimaryVertex(0)->GetPrimary(0)->SetUserInformation(info); 
}

double MyPrimaryGenerator::Angle_Convertion(G4float m_b, G4float m_B, G4float theta){

    // gamma for angle transformation
    G4double VcM_numerator = M_a * m_b * E_Incident;
    G4double Vcm_denominator = m_B * (m_b + m_B) * Qreaction + m_B * (m_B + m_b - M_a) * E_Incident;
    G4double gamma = std::sqrt(VcM_numerator / Vcm_denominator);
    
    // Convert COM angle to lab angle
    G4double cos_theta_lab = (gamma + cos(theta)) / std::sqrt( 1 + gamma*gamma + 2*gamma*cos(theta));
    G4double theta_lab2 = std::acos(cos_theta_lab);  // Lab frame angle

    return theta_lab2;

}

double MyPrimaryGenerator::Kinematic_Energy(G4float m_b, G4float m_B){

    //Proton energy in lab frame
    //We applied E = (-b + sqrt(b*b - 4 * a * c)) / 2a

    G4double a, b, c;
    G4double Energy;
    a = m_B + m_b;
    b = -(2 * std::sqrt(E_Incident * M_a * m_b ) * cos(theta_lab));
    c = - ((m_B - M_a) * E_Incident + m_B * Qreaction);

    Energy = std::pow((-b + std::sqrt(b * b - 4 * a * c)) / (2*a), 2);

    return Energy;

}