#include "construction.hh"
#include "G4SDManager.hh"

MyDetectorConstruction::MyDetectorConstruction(DSSSDUserInput* u): 
flag_SingleSide(false), flag_DoubleSide(false) 
{
    userInp = u;
    LoadMaterials();
    flagOverlaps = false;
}

MyDetectorConstruction::~MyDetectorConstruction()
{}


G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
    
    xWorld = 20*centimeter;
    yWorld = 20*centimeter;
    zWorld = 20*centimeter;

    solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
    logicWorld = new G4LogicalVolume(solidWorld, vacuum, "logicWorld");
    physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, flagOverlaps);
    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

    //Target definition
    if (userInp->Get_Target() == "NaCl") {
        G4cout << "Build NaCl target" << G4endl;
        ConstructTarget_NaCl(logicWorld);
    } else if (userInp->Get_Target() == "LiF") {
        G4cout << "Build NaCl target" << G4endl;
        ConstructTarget_LiF(logicWorld);
    } else if (userInp->Get_Target() == "NoTarget") {
        G4cout << "Build NaCl target" << G4endl;
        ConstructTarget_NoTarget(logicWorld);
    } else if (userInp->Get_Target() == "Al26") {
        G4cout << "Build Al26 target" << G4endl;
        ConstructTarget_Al_nTOF(logicWorld);
    } else if (userInp->Get_Target() == "LiF_nTOF") {
        G4cout << "Build LiF n_TOF target" << G4endl;
        ConstructTarget_LiF_nTOF(logicWorld);
    } else {
        G4cout
            << "ERROR: Target not defined or not added to the conditional" << G4endl
            << "       Check target name in the input file" << G4endl
            << "       Check if target is defined in the DetectorConstruction class"
            << G4endl
            << "       Check conditional in Construct of DetectorConstruction class"
            << G4endl;
        exit(-1);
    }    
    
    // Single side detector definition. 
    // We define the dimension of the detector and the placement

    int TotalNumberDetectors;
    int DetectorTypeDSSSD[50];
    float zDSSSD[50], thetaDSSSD[50], phiDSSSD[50], ThickDSSSD[50];
    float TranslationX[50], TranslationY[50], TranslationZ[50];

    userInp->Return_DSSSD(DetectorTypeDSSSD, ThickDSSSD, zDSSSD, thetaDSSSD, phiDSSSD, TranslationX, TranslationY, TranslationZ);
    TotalNumberDetectors = userInp->Get_TotalNumberDectorsDSSSD();

    if(TotalNumberDetectors != 0){
        for(int i = 1; i < TotalNumberDetectors + 1; i++){
            if (userInp->Is_DSSSD_Define(i)) {
                ConstructDetector_DoubleSide_DSSSD(logicWorld, ThickDSSSD[i], zDSSSD[i], thetaDSSSD[i], phiDSSSD[i], i,
                                                   TranslationX[i], TranslationY[i], TranslationZ[i]);
                flag_DoubleSide = true;
            }else if(userInp->Is_SSD_Define(i)){
                ConstructDetector_OneSide_DSSSD(logicWorld, ThickDSSSD[i], zDSSSD[i], thetaDSSSD[i], phiDSSSD[i], i,
                                                TranslationX[i], TranslationY[i], TranslationZ[i]);
                flag_SingleSide = true;
            }
        }
    }else{
        G4cout
            << "ERROR: Detectors not defined or not added to the conditional" << G4endl
            << "       Check detector number in the input file" << G4endl
            << "       Check if detector is defined in the DetectorConstruction class"
            << G4endl
            << "       Check conditional in Construct of DetectorConstruction class"
            << G4endl;
        exit(-1);
    }

    return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyDetectorConstruction::LoadMaterials() {
    // BEGIN MATERIALS LIST
    G4NistManager* materials_list = G4NistManager::Instance();  // Materials database

    air = materials_list->FindOrBuildMaterial("G4_AIR");
    O  = materials_list->FindOrBuildMaterial("G4_O");
    Al = materials_list->FindOrBuildMaterial("G4_Al");
    F = materials_list->FindOrBuildMaterial("G4_F");
    Ca = materials_list->FindOrBuildMaterial("G4_Ca");
    Cl = materials_list->FindOrBuildMaterial("G4_Cl");
    Na = materials_list->FindOrBuildMaterial("G4_Na");
    Si = materials_list->FindOrBuildMaterial("G4_Si");
    Li = materials_list->FindOrBuildMaterial("G4_Li");
    B = materials_list->FindOrBuildMaterial("G4_B");
    Au = materials_list->FindOrBuildMaterial("G4_Au");
    quartz = materials_list->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    mylar = materials_list->FindOrBuildMaterial("G4_MYLAR");

    //Sodium Chloride target
    cl35 = new G4Material("NaCl", 2.16*g/cm3, 2);
    cl35->AddMaterial(Na, 0.5);
    cl35->AddMaterial(Cl, 0.5);

    //Lithium Fluoride target
    LiF = new G4Material("LiF", 2.646*g/cm3, 2);
    LiF->AddMaterial(F, 0.5);
    LiF->AddMaterial(Li, 0.5);

    //PCB FR-4 material
    pcb_material = new G4Material("PCB_FR4", 1.85*g/cm3, 5);
    pcb_material->AddMaterial(O, 0.53);
    pcb_material->AddMaterial(Si, 0.27);
    pcb_material->AddMaterial(Ca, 0.1);
    pcb_material->AddMaterial(Al, 0.07);
    pcb_material->AddMaterial(B, 0.03);

    //Al-26 material definition
    elAl26 = new G4Element("Aluminum26", "Al26", 13, 26*g/mole);
    Al26 = new G4Material("Al26Material", 2.70*g/cm3, 1);
    Al26->AddElement(elAl26, 1);

    vacuum_mat = materials_list->FindOrBuildMaterial("G4_Galactic");

    //  Vacuum
    vacuum = new G4Material("Vacuum", 1., 1.01 * g / mole, 1.e-25 * g / cm3, kStateGas, 273 * kelvin, 3.e-18 * pascal);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyDetectorConstruction::ConstructTarget_LiF(G4LogicalVolume* mother) {
    //Standards definition
    targetrot = new G4RotationMatrix();
    G4float TargetRotationx, TargetRotationy, TargetRotationz;
    userInp->Return_Rotation(TargetRotationx, TargetRotationy, TargetRotationz);
    targetrot->rotateX(TargetRotationx * deg);
    targetrot->rotateY(-TargetRotationy * deg);
    targetrot->rotateZ(TargetRotationz * deg);
    targetDiameter = 30 * mm;
    targetThickness = 0.001 * mm;
    targetRadius = targetDiameter/2.0;

    G4double mylarthickness = 0.005 * mm;
    
    //mylar foil
    //Gold layer definition
    G4Tubs* mylarfoil = new G4Tubs("Mylar_foil", 0, targetRadius, mylarthickness / 2.0, 0*degree, 360*degree);
    mylarlayer = new G4LogicalVolume(mylarfoil, mylar, "Mylar_foil");
    new G4PVPlacement(targetrot, G4ThreeVector(0, 0, mylarthickness / 2 + targetThickness / 2.0), mylarlayer, "Mylar_foil", mother, false, 0, flagOverlaps);
    mylarlayer->SetVisAttributes(G4Colour::Blue());

    //Sodiumd Chloride target
    Target_LiF = new G4Tubs("Target_LiF", 
                             0,
                             targetRadius,
                             targetThickness/2.0,
                             0*degree,
                             360*degree);
    targetLiF = new G4LogicalVolume(Target_LiF, LiF, "Target_LiF");
    new G4PVPlacement(targetrot, G4ThreeVector(0, 0, 0), targetLiF, "Target_LiF", mother, false, 0, flagOverlaps);
    targetLiF->SetVisAttributes(new G4VisAttributes(G4Colour::Green()));

    // Aluminum Ring
    G4double Althickness = 0.5 * mm;
    solidAlRing = new G4Tubs("AlRing", targetRadius, targetRadius + 2 * mm, Althickness / 2, 0, 360 * deg);
    G4LogicalVolume* logicAlRing = new G4LogicalVolume(solidAlRing, Al, "AlRing");
    new G4PVPlacement(targetrot, G4ThreeVector(0., 0., -1. * Althickness / 2 + targetThickness / 2), logicAlRing, "AlRing", 
                    mother, false, 0, flagOverlaps);
    logicAlRing->SetVisAttributes(G4Colour::Gray());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyDetectorConstruction::ConstructTarget_Al_nTOF(G4LogicalVolume* mother) {
    //Standards definition
    targetrot = new G4RotationMatrix();
    G4float TargetRotationx, TargetRotationy, TargetRotationz;
    userInp->Return_Rotation(TargetRotationx, TargetRotationy, TargetRotationz);
    targetrot->rotateX(TargetRotationx * deg);
    targetrot->rotateY(-TargetRotationy * deg);
    targetrot->rotateZ(TargetRotationz * deg);
    
    targetWidth = 50 * mm;
    targetHeight = 60 * mm;
    targetThickness = 0.001 * mm;
    
    G4double BackingWidth = 82 * mm;
    G4double BackingHeight = 68 * mm;
    G4double BackingThickness = 0.005 * mm;

    //Backingdefinition
    G4Box* Albacking = new G4Box("Al_Backing", BackingWidth / 2.0, BackingHeight / 2.0, BackingThickness / 2.0);
    albacking = new G4LogicalVolume(Albacking, Al, "Al_Backing");
    new G4PVPlacement(targetrot, G4ThreeVector(0, 0, - BackingThickness - targetThickness / 2.0), albacking, "Al_Backing", mother, false, 0, flagOverlaps);
    albacking->SetVisAttributes(G4Colour::Blue());

    //Aluminum 26 target
    Target_Al26 = new G4Box("Target_Al26", targetWidth / 2.0, targetHeight / 2.0, targetThickness / 2.0);
    targetAl26 = new G4LogicalVolume(Target_Al26, Al26, "Target_Al26");
    new G4PVPlacement(targetrot, G4ThreeVector(0, 0, 0), targetAl26, "Target_Al26", mother, false, 0, flagOverlaps);
    targetAl26->SetVisAttributes(new G4VisAttributes(G4Colour::Green()));

    //Aluminum Frame
    G4double FrameWidth = 96 * mm;
    G4double FrameHeight = 82 * mm;
    G4double FrameThickness = 1 * mm;
    
    G4Box* Dummybacking_In = new G4Box("Dummy_Backing_In", BackingWidth / 2.0, BackingHeight / 2.0, FrameThickness / 2.0 + 0.001 * mm);
    G4Box* Dummybacking_Out  = new G4Box("Dummy_Backing_Out", FrameWidth / 2.0, FrameHeight / 2.0, FrameThickness / 2.0);
    G4SubtractionSolid* AlFrame = new G4SubtractionSolid("Al_Frame", Dummybacking_Out, Dummybacking_In);

    G4LogicalVolume* logicAlFrame = new G4LogicalVolume(AlFrame, Al, "AlFrame");
    new G4PVPlacement(targetrot, G4ThreeVector(0., 0., FrameThickness / 2), logicAlFrame, "AlFrame", 
                    mother, false, 0, flagOverlaps);
    logicAlFrame->SetVisAttributes(G4Colour::Gray());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyDetectorConstruction::ConstructTarget_LiF_nTOF(G4LogicalVolume* mother) {
    //Standards definition
    targetrot = new G4RotationMatrix();
    G4float TargetRotationx, TargetRotationy, TargetRotationz;
    userInp->Return_Rotation(TargetRotationx, TargetRotationy, TargetRotationz);
    targetrot->rotateX(TargetRotationx * deg);
    targetrot->rotateY(-TargetRotationy * deg);
    targetrot->rotateZ(TargetRotationz * deg);
    
    targetWidth = 60 * mm;
    targetHeight = 50 * mm;
    targetThickness = 0.001 * mm;
    
    G4double BackingWidth = 82 * mm;
    G4double BackingHeight = 68 * mm;
    G4double BackingThickness = 0.005 * mm;

    //Backing definition
    G4Box* LiFbacking = new G4Box("LiF_Backing", BackingWidth / 2.0, BackingHeight / 2.0, BackingThickness / 2.0);
    lifbacking = new G4LogicalVolume(LiFbacking, mylar, "LiF_Backing");
    new G4PVPlacement(targetrot, G4ThreeVector(0, 0, - BackingThickness - targetThickness / 2.0), lifbacking, "LiF_Backing", mother, false, 0, flagOverlaps);
    lifbacking->SetVisAttributes(G4Colour::Blue());

    //Lithium Fluoride target
    Target_LiF_nTOF = new G4Box("Target_LiF_nTOF", targetWidth / 2.0, targetHeight / 2.0, targetThickness / 2.0);
    targetLiF = new G4LogicalVolume(Target_LiF_nTOF, LiF, "Target_LiF_nTOF");
    new G4PVPlacement(targetrot, G4ThreeVector(0, 0, 0), targetLiF, "Target_LiF_nTOF", mother, false, 0, flagOverlaps);
    targetLiF->SetVisAttributes(new G4VisAttributes(G4Colour::Green()));

    //Lithium Fluoride Frame
    G4double FrameWidth = 96 * mm;
    G4double FrameHeight = 82 * mm;
    G4double FrameThickness = 1 * mm;
    
    G4Box* Dummybacking_In = new G4Box("Dummy_Backing_In", BackingWidth / 2.0, BackingHeight / 2.0, FrameThickness / 2.0 + 0.01 * mm);
    G4Box* Dummybacking_Out  = new G4Box("Dummy_Backing_Out", FrameWidth / 2.0, FrameHeight / 2.0, FrameThickness / 2.0);
    G4SubtractionSolid* LiF_Frame = new G4SubtractionSolid("LiF_Frame", Dummybacking_Out, Dummybacking_In);

    G4LogicalVolume* logicLiF_Frame = new G4LogicalVolume(LiF_Frame, Al, "LiF_Frame");
    new G4PVPlacement(targetrot, G4ThreeVector(0., 0., FrameThickness / 2), logicLiF_Frame, "LiF_Frame", 
                    mother, false, 0, flagOverlaps);
    logicLiF_Frame->SetVisAttributes(G4Colour::Gray());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyDetectorConstruction::ConstructTarget_NoTarget(G4LogicalVolume* mother) {
  // Solid
  G4float target_diameter = 0.0001 * mm;
  G4float target_thickness = 0.001 * mm;

  float deltax0, deltay0, deltaz0;
  userInp->Get_MissAligment(deltax0, deltay0, deltaz0);

  G4Tubs* target = new G4Tubs("Target_Calibration", 0, target_diameter / 2,
                              target_thickness / 2, 0. * deg, 360. * deg);
  // Logic
  no_target = new G4LogicalVolume(target, vacuum, "Target_Calibration");
  // Physic
  /* G4VPhysicalVolume *phys_target=*/new G4PVPlacement(
      0, G4ThreeVector(deltax0, deltay0, deltaz0), no_target, "Target_Calibration", mother,
      false, 0, flagOverlaps);
  no_target->SetVisAttributes(new G4VisAttributes(G4Colour::Yellow()));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyDetectorConstruction::ConstructTarget_NaCl(G4LogicalVolume* mother) {
    //Standards definition
    targetrot = new G4RotationMatrix();
    G4float TargetRotationx, TargetRotationy, TargetRotationz;
    userInp->Return_Rotation(TargetRotationx, TargetRotationy, TargetRotationz);
    targetrot->rotateX(TargetRotationx * deg);
    targetrot->rotateY(-TargetRotationy * deg);
    targetrot->rotateZ(TargetRotationz * deg);
    targetDiameter = 30 * mm;
    targetThickness = 0.001 * mm;
    targetRadius = targetDiameter/2.0;

    G4double mylarthickness = 0.005 * mm;
    G4double goldthickness  = 0.0001 * mm;
    
    //mylar foil
    //Gold layer definition
    G4Tubs* mylarfoil = new G4Tubs("Mylar_foil", 0, targetRadius, mylarthickness / 2.0, 0*degree, 360*degree);
    mylarlayer = new G4LogicalVolume(mylarfoil, mylar, "Mylar_foil");
    new G4PVPlacement(targetrot, G4ThreeVector(0, 0, mylarthickness / 2 + targetThickness / 2.0), mylarlayer, "Mylar_foil", mother, false, 0, flagOverlaps);
    mylarlayer->SetVisAttributes(G4Colour::Blue());

    //Sodiumd Chloride target
    Target_cl35 = new G4Tubs("Target_cl35", 
                             0,
                             targetRadius,
                             targetThickness/2.0,
                             0*degree,
                             360*degree);
    targetCl = new G4LogicalVolume(Target_cl35, cl35, "Target_cl35");
    new G4PVPlacement(targetrot, G4ThreeVector(0, 0, 0), targetCl, "Target_cl35", mother, false, 0, flagOverlaps);
    targetCl->SetVisAttributes(new G4VisAttributes(G4Colour::Green()));

    //Gold layer definition
    G4Tubs* target = new G4Tubs("Target_197Au", 0, targetRadius, goldthickness / 2.0, 0*degree, 360*degree);
    goldlayer = new G4LogicalVolume(target, Au, "Target_197Au");
    new G4PVPlacement(targetrot, G4ThreeVector(0, 0, -1.0 * targetThickness / 2.0 - 1.0 * goldthickness / 2), goldlayer, "Target_197Au", mother, false, 0, flagOverlaps);
    goldlayer->SetVisAttributes(G4Colour::Yellow());

    // Aluminum Ring
    G4double Althickness = 0.5 * mm;
    solidAlRing = new G4Tubs("AlRing", targetRadius, targetRadius + 2 * mm, Althickness / 2, 0, 360 * deg);
    G4LogicalVolume* logicAlRing = new G4LogicalVolume(solidAlRing, Al, "AlRing");
    new G4PVPlacement(targetrot, G4ThreeVector(0., 0., -1. * Althickness / 2 + targetThickness / 2), logicAlRing, "AlRing", mother, false, 0, flagOverlaps);
    logicAlRing->SetVisAttributes(G4Colour::Gray());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyDetectorConstruction::ConstructDetector_OneSide_DSSSD(G4LogicalVolume* mother, G4float dimZ, G4float Distance,
                                                             G4float thetaDSSSD, G4float phiDSSSD, G4int detID, 
                                                             G4float translationx, G4float translationy, G4float translationz){
        
    G4String Detector_name = "Detector_SSD_" + std::to_string(detID);

    //PCB Dimensions
    G4float PCB_Xdim = 61.5 * mm;
    G4float PCB_Ydim = 71.0 * mm;
    G4float PCB_Zdim =  1.6 * mm;

    //chip Dimensions
    G4float Chip_Xdim = 53.3 * mm;
    G4float Chip_Ydim = 53.3 * mm; 

    //Detector Dimensions
    G4float dimX = 50 * mm; 
    G4float dimY = 50 * mm;
    G4float ThicknessDSSSD = dimZ * mm;
    G4float DistanceDSSSD = Distance * cm;

    nStrips = 16;
    G4double stripY = dimY / nStrips;

    //Definition of the sperical coordinate
    G4RotationMatrix rotDSSSD;
    G4double rDSSSD;
    G4ThreeVector posDSSSD;
    
    rotDSSSD = G4RotationMatrix();
    rotDSSSD.rotateY(thetaDSSSD * deg);
    rotDSSSD.rotateZ(phiDSSSD * deg);
    rDSSSD = DistanceDSSSD + ThicknessDSSSD / 2;
    posDSSSD = G4ThreeVector(
           rDSSSD * sin(thetaDSSSD * deg) * cos(phiDSSSD * deg),
           rDSSSD * sin(thetaDSSSD * deg) * sin(phiDSSSD * deg),
           rDSSSD * cos(thetaDSSSD * deg));
    
    G4ThreeVector global = posDSSSD + G4ThreeVector(translationx, translationy, translationz);


    //Definition of the PCB board
    G4Box* PCB_Outer = new G4Box("PCB_SSD_Outer_" + std::to_string(detID), PCB_Xdim / 2, PCB_Ydim / 2, PCB_Zdim / 2);
    G4Box* PCB_Inner = new G4Box("PCB_SSD_Inner_" + std::to_string(detID), Chip_Xdim / 2, Chip_Ydim / 2, (PCB_Zdim + 0.1 * mm) / 2);
    G4SubtractionSolid* PCB_Board = new G4SubtractionSolid("PCB_SSD_Board_" + std::to_string(detID), PCB_Outer, PCB_Inner);

    dElogicPCB = new G4LogicalVolume(PCB_Board, pcb_material, "PCB_SSD_" + std::to_string(detID));
    new G4PVPlacement(G4Transform3D(rotDSSSD, global), dElogicPCB, "PCB_SSD_" + std::to_string(detID), mother, false, detID, flagOverlaps);
    dElogicPCB->SetVisAttributes(G4Colour::Yellow());

    //Definition os the dimension of the full Si chip
    G4Box* Chip_Outer = new G4Box("Chip_SSD_Outer_" + std::to_string(detID), Chip_Xdim / 2, Chip_Ydim / 2, ThicknessDSSSD/2);
    G4Box* Chip_Inner = new G4Box("Chip_SSD_Inner_" + std::to_string(detID), dimX/2, dimY/2, ThicknessDSSSD/2);
    G4SubtractionSolid* Chip_Board = new G4SubtractionSolid("Chip_SSD_Board_" + std::to_string(detID), Chip_Outer, Chip_Inner);
    G4LogicalVolume* dElogicChip = new G4LogicalVolume(Chip_Board , Si, "Chip_SSD_" + std::to_string(detID));
    new G4PVPlacement(G4Transform3D(rotDSSSD, global), dElogicChip, "Chip_SSD_" + std::to_string(detID), mother, false, detID, flagOverlaps);
    dElogicChip->SetVisAttributes(G4Colour::Grey());

    //One side detector defnition
    G4double DeadLayer = 0.0005 * mm;
    G4double MetalizationLayer = 0.00025 * mm;
    G4Box* dESensor = new G4Box("ThinSensor_SSD_" + std::to_string(detID), dimX/2, dimY/2, ThicknessDSSSD/2.0 + DeadLayer + MetalizationLayer);
    G4LogicalVolume* dElogicSensor = new G4LogicalVolume(dESensor, Si, "ThinSensor_SSD_" + std::to_string(detID));
    new G4PVPlacement(G4Transform3D(rotDSSSD, global), dElogicSensor, "ThinSensor_SSD_" + std::to_string(detID), mother, false, detID, flagOverlaps);  
     
    //Dead Layer volumen
    G4Box* Si_DeadLayer = new G4Box("Si_DeadLayer_DSSSD_" + std::to_string(detID), dimX / 2.0, dimY / 2.0, DeadLayer / 2.0);
    G4LogicalVolume* logicdeadlayer_Front = new G4LogicalVolume(Si_DeadLayer, Si, "Si_DeadLayer_Front_DSSSD_" + std::to_string(detID));
    G4LogicalVolume* logicdeadlayer_Back = new G4LogicalVolume(Si_DeadLayer, Si, "Si_DeadLayer_Back_DSSSD_" + std::to_string(detID));
    new G4PVPlacement(0, G4ThreeVector(0., 0., - ThicknessDSSSD/2.0 - DeadLayer / 2.0), logicdeadlayer_Front, "Si_DeadLayer_Front_DSSSD_" + std::to_string(detID), dElogicSensor, false, detID, flagOverlaps);
    new G4PVPlacement(0, G4ThreeVector(0., 0., + ThicknessDSSSD/2.0 + DeadLayer / 2.0), logicdeadlayer_Back, "Si_DeadLayer_Back_DSSSD_" + std::to_string(detID), dElogicSensor, false, detID, flagOverlaps);
    logicdeadlayer_Front->SetVisAttributes(G4Colour::Grey());
    logicdeadlayer_Back->SetVisAttributes(G4Colour::Grey());

    //Aluminum Metalisation volumen
    G4Box* Al_Metalisation = new G4Box("Al_Metalisation_DSSSD_" + std::to_string(detID), dimX / 2.0, dimY / 2.0, MetalizationLayer / 2.0);
    G4LogicalVolume* logicmetalisation_Front = new G4LogicalVolume(Al_Metalisation, Al, "Al_Metalisation_Front_DSSSD_" + std::to_string(detID));
    G4LogicalVolume* logicmetalisation_Back = new G4LogicalVolume(Al_Metalisation, Al, "Al_Metalisation_Back_DSSSD_" + std::to_string(detID));
    new G4PVPlacement(0, G4ThreeVector(0., 0., - ThicknessDSSSD/2.0 - DeadLayer - MetalizationLayer / 2.0), logicmetalisation_Front, "Al_Metalisation_Front_DSSSD_" + std::to_string(detID), dElogicSensor, false, detID, flagOverlaps);
    new G4PVPlacement(0, G4ThreeVector(0., 0., + ThicknessDSSSD/2.0 + DeadLayer + MetalizationLayer / 2.0), logicmetalisation_Back, "Al_Metalisation_Back_DSSSD_" + std::to_string(detID), dElogicSensor, false, detID, flagOverlaps);
    logicmetalisation_Front->SetVisAttributes(G4Colour::Magenta());
    logicmetalisation_Back->SetVisAttributes(G4Colour::Magenta());

    //Definition of the strip dimension
    solidStripdE = new G4Box(Detector_name, dimX/2, stripY/2, ThicknessDSSSD/2);
    //Logical volume of the strip
    logicStripdE = new G4LogicalVolume(solidStripdE, Si, Detector_name);
    logicSSD.push_back(logicStripdE);
    //Definition of each strip within the activ area of the volume
    for (G4int i = 0; i < nStrips; i++){
        G4double yPosdE = -0.5*dimY + (i + 0.5) * stripY;
        new G4PVPlacement(0, G4ThreeVector(0., yPosdE, 0.), 
                          logicStripdE, Detector_name, dElogicSensor, 
                          false, i, flagOverlaps);
    }    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyDetectorConstruction::ConstructDetector_DoubleSide_DSSSD(G4LogicalVolume* mother, G4float dimZ, 
                                                                G4float Distance, G4float thetaDSSSD, 
                                                                G4float phiDSSSD, G4int detID, 
                                                                G4float translationx, G4float translationy, G4float translationz){

    G4String Detector_name = "Detector_DSSSD_" + std::to_string(detID);

    //PCB Dimensions
    G4float PCB_Xdim = 61.5 * mm;
    G4float PCB_Ydim = 71.0 * mm;
    G4float PCB_Zdim =  1.6 * mm;

    //chip Dimensions
    G4float Chip_Xdim = 53.3 * mm;
    G4float Chip_Ydim = 53.3 * mm;

    //Detector Dimensions
    G4float dimX = 50 * mm; 
    G4float dimY = 50 * mm;
    G4float ThicknessDSSSD = dimZ * mm;
    G4float DistanceDSSSD = Distance * cm;

    //Strip size definition
    DSnPixelX = 16;
    DSnPixelY = 16;
    G4double stripx = dimX / DSnPixelX;
    G4double stripy = dimY / DSnPixelY;

    //Definition of the sperical coordinate
    G4RotationMatrix rotDSSSD;
    G4double rDSSSD;
    G4ThreeVector posDSSSD;
    

    rotDSSSD = G4RotationMatrix();
    rotDSSSD.rotateY(thetaDSSSD * deg);
    rotDSSSD.rotateZ(phiDSSSD * deg);
    rDSSSD = DistanceDSSSD + ThicknessDSSSD / 2;
    posDSSSD = G4ThreeVector(
           rDSSSD * sin(thetaDSSSD * deg) * cos(phiDSSSD * deg),
           rDSSSD * sin(thetaDSSSD * deg) * sin(phiDSSSD * deg),
           rDSSSD * cos(thetaDSSSD * deg));

    G4ThreeVector global = posDSSSD + G4ThreeVector(translationx, translationy, translationz);

    G4Box* PCB_Outer = new G4Box("PCB_DSSSD_Outer_" + std::to_string(detID), PCB_Xdim / 2, PCB_Ydim / 2, PCB_Zdim / 2);
    G4Box* PCB_Inner = new G4Box("PCB_DSSSD_Inner_" + std::to_string(detID), Chip_Xdim / 2, Chip_Ydim / 2, (PCB_Zdim + 0.1 * mm) / 2);
    G4SubtractionSolid* PCB_Board = new G4SubtractionSolid("PCB_DSSSD_Board_" + std::to_string(detID), PCB_Outer, PCB_Inner);

    dElogicPCB = new G4LogicalVolume(PCB_Board, pcb_material, "PCB_DSSSD_" + std::to_string(detID));
    new G4PVPlacement(G4Transform3D(rotDSSSD, global), dElogicPCB, 
                      "PCB_DSSSD_" + std::to_string(detID), mother, false, detID, flagOverlaps);
    dElogicPCB->SetVisAttributes(G4Colour::Red());


    //Definition os the dimension of the full Si chip
    G4Box* Chip_Outer = new G4Box("Chip_DSSSD_Outer_" + std::to_string(detID), Chip_Xdim / 2, Chip_Ydim / 2, ThicknessDSSSD/2);
    G4Box* Chip_Inner = new G4Box("Chip_DSSSD_Inner_" + std::to_string(detID), dimX/2, dimY/2, ThicknessDSSSD/2);
    G4SubtractionSolid* Chip_Board = new G4SubtractionSolid("Chip_DSSSD_Board_" + std::to_string(detID), Chip_Outer, Chip_Inner);
    G4LogicalVolume* dElogicChip = new G4LogicalVolume(Chip_Board , Si, "Chip_DSSSD_" + std::to_string(detID));
    new G4PVPlacement(G4Transform3D(rotDSSSD, global), dElogicChip, "Chip_DSSSD_" + std::to_string(detID), mother, false, detID, flagOverlaps);
    dElogicChip->SetVisAttributes(G4Colour::Grey());


    //Double side detector defnition
    G4double DeadLayer = 0.0005 * mm;
    G4double MetalizationLayer = 0.00025 * mm;
    solidDSWafer = new G4Box("ThickSensor_DSSSD_" + std::to_string(detID), dimX / 2.0, dimY / 2.0, ThicknessDSSSD / 2.0 + DeadLayer + MetalizationLayer);
    logicDSWafer = new G4LogicalVolume(solidDSWafer, vacuum, "DSWafer_DSSSD_" + std::to_string(detID));
    new G4PVPlacement(G4Transform3D(rotDSSSD, global), 
                     logicDSWafer, "DSWafer_SSD_" + std::to_string(detID), mother, false, detID, flagOverlaps);

    //Dead Layer volumen
    G4Box* Si_DeadLayer = new G4Box("Si_DeadLayer_DSSSD_" + std::to_string(detID), dimX / 2.0, dimY / 2.0, DeadLayer / 2.0);
    G4LogicalVolume* logicdeadlayer_Front = new G4LogicalVolume(Si_DeadLayer, Si, "Si_DeadLayer_Front_DSSSD_" + std::to_string(detID));
    G4LogicalVolume* logicdeadlayer_Back = new G4LogicalVolume(Si_DeadLayer, Si, "Si_DeadLayer_Back_DSSSD_" + std::to_string(detID));
    new G4PVPlacement(0, G4ThreeVector(0., 0., - ThicknessDSSSD/2.0 - DeadLayer / 2.0), logicdeadlayer_Front, "Si_DeadLayer_Front_DSSSD_" + std::to_string(detID), logicDSWafer, false, detID, flagOverlaps);
    new G4PVPlacement(0, G4ThreeVector(0., 0., + ThicknessDSSSD/2.0 + DeadLayer / 2.0), logicdeadlayer_Back, "Si_DeadLayer_Back_DSSSD_" + std::to_string(detID), logicDSWafer, false, detID, flagOverlaps);
    logicdeadlayer_Front->SetVisAttributes(G4Colour::Grey());
    logicdeadlayer_Back->SetVisAttributes(G4Colour::Grey());

    //Aluminum Metalisation volumen
    G4Box* Al_Metalisation = new G4Box("Al_Metalisation_DSSSD_" + std::to_string(detID), dimX / 2.0, dimY / 2.0, MetalizationLayer / 2.0);
    G4LogicalVolume* logicmetalisation_Front = new G4LogicalVolume(Al_Metalisation, Al, "Al_Metalisation_Front_DSSSD_" + std::to_string(detID));
    G4LogicalVolume* logicmetalisation_Back = new G4LogicalVolume(Al_Metalisation, Al, "Al_Metalisation_Back_DSSSD_" + std::to_string(detID));
    new G4PVPlacement(0, G4ThreeVector(0., 0., - ThicknessDSSSD/2.0 - DeadLayer - MetalizationLayer / 2.0), logicmetalisation_Front, "Al_Metalisation_Front_DSSSD_" + std::to_string(detID), logicDSWafer, false, detID, flagOverlaps);
    new G4PVPlacement(0, G4ThreeVector(0., 0., + ThicknessDSSSD/2.0 + DeadLayer + MetalizationLayer / 2.0), logicmetalisation_Back, "Al_Metalisation_Back_DSSSD_" + std::to_string(detID), logicDSWafer, false, detID, flagOverlaps);
    logicmetalisation_Front->SetVisAttributes(G4Colour::Magenta());
    logicmetalisation_Back->SetVisAttributes(G4Colour::Magenta());

    //Strips Volume
    solidDSpixel = new G4Box("DSpixel_DSSSD_" + std::to_string(detID), stripx/2, stripy/2, ThicknessDSSSD/2);
    logicDSpixel = new G4LogicalVolume(solidDSpixel, Si, "DSpixel_DSSSD_" + std::to_string(detID));
    logicDSSSD.push_back(logicDSpixel);

    for(G4int xIndex = 0; xIndex < DSnPixelX; ++xIndex){ 
        for(G4int yIndex = 0; yIndex < DSnPixelY; ++yIndex){
            G4double xPosdE = -0.5*dimX + (xIndex + 0.5) * stripx;
            G4double yPosdE =  0.5*dimY - (yIndex + 0.5) * stripy;
            G4int copyNo = 1000 * (100 + xIndex) + (200 + yIndex); 
            new G4PVPlacement(0, G4ThreeVector(xPosdE, yPosdE, 0.),
                            logicDSpixel, "DSpixel_front_" + std::to_string(detID), logicDSWafer, 
                            false, copyNo, flagOverlaps);
        }
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyDetectorConstruction::ConstructSDandField()
{

    G4SDManager* sdManager = G4SDManager::GetSDMpointer();

    MySensitiveDetector* sd_dE = new MySensitiveDetector("DSSSD", userInp);
    sdManager->AddNewDetector(sd_dE);

    if (flag_SingleSide) {
        for (auto ssd : logicSSD) {
            ssd->SetVisAttributes(G4Colour::Blue());
            ssd->SetSensitiveDetector(sd_dE);
        }
    }
    if (flag_DoubleSide) {
        for (auto ssd : logicDSSSD) {
            ssd->SetVisAttributes(G4Colour::Brown());
            ssd->SetSensitiveDetector(sd_dE);
        }
        
    }

}