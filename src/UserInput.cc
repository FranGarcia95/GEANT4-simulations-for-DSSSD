#include "UserInput.hh"

DSSSDUserInput::DSSSDUserInput(char *configName)
{
  cout << "Creating the input manager " << endl;
  Read_ConfigFile(configName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DSSSDUserInput::~DSSSDUserInput() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DSSSDUserInput::Read_ConfigFile(char *finpName) {
  string buffer;
  ifstream finp(finpName);
  if (finp.fail()) {
    cout << "Poblem getting config file: " << finpName << endl;
    exit(-1);
  } else
    cout << "Opening config file: " << finpName << endl;

  while (finp >> buffer) {
    if (buffer.find("#") == 0)
      continue;
    else if (buffer == "name")
      finp >> targetName;
    else if(buffer == "SampleShape"){
      finp >> sampleshape;
      if(sampleshape == "Circular"){
        finp >> buffer;
        if (buffer == "diameter(mm)"){
          finp >> targetDiameter;
        }else {
          cout << "Missing diameter(cm)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "thickness(mm)"){
          finp >> targetThickness;
        }else {
          cout << "Missing thickness(mm)" << endl;
          exit(-1);
        }
      }else if(sampleshape == "Rectangular"){
        finp >> buffer;
        if (buffer == "height(mm)"){
          finp >> targetHeight;
        }else {
          cout << "Missing height(mm)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "width(mm)"){
          finp >> targetWidth;
        }else {
          cout << "Missing width(mm)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "thickness(mm)"){
          finp >> targetThickness;
        }else {
          cout << "Missing thickness(mm)" << endl;
          exit(-1);
        }
      }
    }
    else if(buffer == "angulardistribution"){
      finp >> angulardistribution;
      if(angulardistribution == "Anisotropic"){
        finp >> buffer;
        if(buffer == "AngularDistributionfile"){
          finp >> AngularDistributionFileName;
        }
      }
    }
    else if (buffer == "atomicMass(g/mol)")
      finp >> targetaMass;
    else if (buffer == "density(g/cm3)")
      finp >> targetDensity;
    else if (buffer == "RotationangleX(degrees)")
      finp >> TargetRotationX;
    else if (buffer == "RotationangleY(degrees)")
      finp >> TargetRotationY;
    else if (buffer == "RotationangleZ(degrees)")
      finp >> TargetRotationZ;
    else if (buffer == "particle")
      finp >> particle;
    else if (buffer == "energy")
      finp >> particleEnergy;
    else if (buffer == "energyspread"){
      finp >> energyspread;  
      if(energyspread > particleEnergy){
        G4cout << "Energy spread error: " << " energyspread " << energyspread <<
                  " (keV) bigger than particle energy " << particleEnergy << " (keV)" << G4endl;
        exit(-1);
      }    
    }
    else if(buffer == "ReferenceFrame"){
      finp >> referenceFrame;
      if(referenceFrame == "LabFramefromCMFrame_Simultaneous"){
        finp >> buffer;
        if(buffer == "secondparticle"){
          finp >> secondaryparticle;
        }else{
          G4cout << "Secondary particle missing " << G4endl;
          exit(-1);
        }
      }
    }else if(buffer == "incidentparticle"){
      finp >> incidentparticle;
    }else if(buffer == "targetnucleus"){
      finp >> targetnucleus;
    }else if(buffer == "ejectedparticle"){
      finp >> ejectedparticle;
    }else if(buffer == "residualnucleus"){
      finp >> residualnucleus;
    }else if (buffer == "emission") {
      finp >> emissionMode;
      if (emissionMode == "B") {
        finp >> buffer;
        if (buffer == "bifFilename") {
          finp >> bifFilename;
        } else {
          cout << "Emission B -> Missing bifFilename" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "MissalignX(mm)"){
          finp >> MissalignX;
        }else {
          cout << "Missing MissalignX(mm)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "MissalignY(mm)"){
          finp >> MissalignY;
        }else {
          cout << "Missing MissalignY(mm)" << endl;
          exit(-1);
        }

      }else if (emissionMode == "Decay") {
        finp >> buffer;
        if(buffer == "Source"){
          finp >> ADecayIsotope;
          finp >> ZDecayIsotope;
          G4cout << ADecayIsotope << "    " << ZDecayIsotope << G4endl;
        }else {
          cout << "Missing Decay Source" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "MissalignX(mm)"){
          finp >> MissalignX;
        }else {
          cout << "Missing MissalignX(mm)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "MissalignY(mm)"){
          finp >> MissalignY;
        }else {
          cout << "Missing MissalignY(mm)" << endl;
          exit(-1);
        }
      }
    } else if (buffer == "verbose")
      finp >> verboseLevel;
    else if (buffer == "Nevents") {
      finp >> nEvents;
      if (nEvents == 0){
        std::cerr << "No events simulated" << std::endl;
        exit(-1);
      }
    }

    else if (buffer == "NumberDetectors") {
      finp >> TotalNumberDectors;
      for (int i = 1; i < (TotalNumberDectors + 1); i++) {
        finp >> buffer;
        if (buffer == "SSD"){
          flagDSSSD[i] = 1;
        } else if(buffer == "DSSSD"){
          flagDSSSD[i] = 2;
        }else {
          cout << "Detector type" << buffer << "not defined" << endl;
          exit(-1);
        }

        finp >> NumberDetector;

        if (NumberDetector != i) {
          cout << "Detector numbers are not in order i=" << i
               << " NumberDetector=" << NumberDetector << endl;
          exit(-1);
        }
        if (NumberDetector > 50) {
          cout << "The maximum number of detectors is 50" << endl;
          exit(-1);
        }

        finp >> buffer;
        if (buffer == "Thickness(mm)") {
          finp >> ThicknessDSSSD[i];
        } else {
          cout << "Missing Thickness(mm)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "z(cm)") {
          finp >> zDSSSD[i];
        } else {
          cout << "Missing z(cm)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "theta(deg)") {
          finp >> thetaDSSSD[i];
        } else {
          cout << "Missing theta(deg)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if (buffer == "phi(deg)") {
          finp >> phiDSSSD[i];
        } else {
          cout << "Missing phi(deg)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if(buffer == "TranslationX(cm)"){
          finp >>  TranslationX[i];
        }else{
          cout << "Missing TranslationX(cm)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if(buffer == "TranslationY(cm)"){
          finp >>  TranslationY[i];
        }else{
          cout << "Missing TranslationY(cm)" << endl;
          exit(-1);
        }
        finp >> buffer;
        if(buffer == "TranslationZ(cm)"){
          finp >>  TranslationZ[i];
        }else{
          cout << "Missing TranslationZ(cm)" << endl;
          exit(-1);
        }
      }
    }

    else if (buffer == "visualize") {
      finp >> buffer;
      if (buffer == "true") flagVisualization = true;
    }

    else if (buffer == "outputFilename") {
      finp >> outputFilename;
    }
  }
  finp.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  
void DSSSDUserInput::Get_TargetCircular(float &diameter, float &thickness){

  diameter = targetDiameter;
  thickness = targetThickness;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DSSSDUserInput::Get_TargetRectangular(float &height, float &width, float &thickness){

  height = targetHeight;
  width  = targetWidth;
  thickness = targetThickness;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DSSSDUserInput::Get_MissAligment(float &missalignx, float &missaligny){

  missalignx = MissalignX;
  missaligny = MissalignY;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DSSSDUserInput::Return_DSSSD(int *theDetectorType, float* thethickness, float *thez,
                                float *thetaRot, float *phiRot, float *translationx, 
                                float *translationy, float *translationz) {
  
  for (int i = 1; i < (TotalNumberDectors + 1); i++) {
    theDetectorType[i] = flagDSSSD[i];
    thez[i] = zDSSSD[i];
    thetaRot[i] = thetaDSSSD[i];
    thethickness[i] = ThicknessDSSSD[i];
    phiRot[i] = phiDSSSD[i];
    translationx[i] = TranslationX[i];
    translationy[i] = TranslationY[i];
    translationz[i] = TranslationZ[i];
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DSSSDUserInput::Return_Rotation(float &targetrotationx, float &targetrotationy, float &targetrotationz){
  targetrotationx = TargetRotationX;
  targetrotationy = TargetRotationY;
  targetrotationz = TargetRotationZ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DSSSDUserInput::Return_particleinfo(float &Incident, float &Target, 
                                         float &Ejected, float &Residual) {
  
  Incident = incidentparticle;
  Target = targetnucleus;
  Ejected = ejectedparticle;
  Residual =residualnucleus;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool DSSSDUserInput::Is_SSD_Define(int i) {
  if (flagDSSSD[i] == 1) {
    return true;
  }else{
    return false;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool DSSSDUserInput::Is_DSSSD_Define(int i) {
  if (flagDSSSD[i] == 2) {
    return true;
  }else{
    return false;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DSSSDUserInput::Get_Origin(float &x0, float &y0, float &z0) {
  x0 = Xorigin;
  y0 = Yorigin;
  z0 = Zorigin;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DSSSDUserInput::Get_Direction(float &x0, float &y0, float &z0) {
  x0 = Xdirection;
  y0 = Ydirection;
  z0 = Zdirection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

