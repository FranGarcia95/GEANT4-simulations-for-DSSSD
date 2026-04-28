#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4GenericMessenger.hh"
#include "G4SubtractionSolid.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"


#include "detector.hh"
#include "UserInput.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    MyDetectorConstruction(DSSSDUserInput *u);
    ~MyDetectorConstruction();
    
    virtual G4VPhysicalVolume *Construct();
        
private:

    DSSSDUserInput *userInp;

    G4LogicalVolume *logicWorld, *targetCl, *targetLiF, *no_target, *goldlayer, *mylarlayer, *logicDSWafer;  
    G4LogicalVolume* logicStripdE; 
    G4LogicalVolume* logicDSpixel;
    std::vector<G4LogicalVolume*> logicSSD;
    std::vector<G4LogicalVolume*> logicDSSSD;
    
    G4LogicalVolume* dElogicPCB;
    
    G4Box *solidWorld, *solidStripdE;
    G4Box *solidDSWafer;  
    G4Box *solidDSpixel;  
    
    G4Tubs *Target_cl35, *Target_LiF, *solidAlRing;
    
    G4VPhysicalVolume *physWorld;	
    
    virtual void ConstructSDandField();
    void ConstructTarget_NaCl(G4LogicalVolume* mother);
    void ConstructTarget_LiF(G4LogicalVolume* mother);
    void ConstructTarget_NoTarget(G4LogicalVolume* mother);
    void ConstructDetector_OneSide_DSSSD(G4LogicalVolume* mother, G4float dimZ, G4float Distance, 
                                         G4float thetaDSSSD, G4float phiDSSSD, G4int detID, 
                                         G4float translationx, G4float translationy, G4float translationz);
    void ConstructDetector_DoubleSide_DSSSD(G4LogicalVolume* mother, G4float dimZ, G4float Distance,
                                            G4float thetaDSSSD, G4float phiDSSSD, G4int detID, 
                                            G4float translationx, G4float translationy, G4float translationz);
    void LoadMaterials();

    bool flag_SingleSide, flag_DoubleSide;

    G4RotationMatrix *targetrot;
    
    G4Material *cl35, *LiF, *pcb_material;

    G4Material *Li, *Al, *F, *Cl, *air, *Au, *B, *Na, *Si, *Ca, *O;

    G4Material *vacuum, *quartz, *vacuum_mat, *kapton, *mylar;

    
    G4double xWorld, yWorld, zWorld;
    G4int nStrips;
    
    //Definition for building the target
    G4double targetDiameter, targetThickness, targetRadius;
    
    //Definition for building the detectors
    G4double DSwaferSizeZ, DSwaferSizeY, DSwaferSizeX, DSpixelSpacing; 
    G4int DSnPixelX, DSnPixelY;  

    G4bool flagOverlaps;
    
};

#endif
