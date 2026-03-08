#include "DetectorConstruction.hh"
#include "CCDSensitiveDetector.hh" // We need this to attach the SD

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"

DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction()
{}

DetectorConstruction::~DetectorConstruction()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Get Materials
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* mat_Si = nist->FindOrBuildMaterial("G4_Si");
    G4Material* mat_Al = nist->FindOrBuildMaterial("G4_Al");
    G4Material* mat_Mylar = nist->FindOrBuildMaterial("G4_MYLAR");
    G4Material* mat_Vac = nist->FindOrBuildMaterial("G4_Galactic");

    // 1. World
    G4double world_size = 30*cm;
    G4Box* solidWorld = new G4Box("World", world_size, world_size, world_size);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, mat_Vac, "World");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0);

    // 2. CCD Target (Active Region)
    // Dimensions: 6k * 15um = 90mm | 1.5k * 15um = 22.5mm | 650um
    G4double ccd_x = 90.0 * mm;
    G4double ccd_y = 22.5 * mm;
    G4double ccd_z = 0.650 * mm;

    G4Box* solidCCD = new G4Box("CCD", ccd_x/2, ccd_y/2, ccd_z/2);
    G4LogicalVolume* logicCCD = new G4LogicalVolume(solidCCD, mat_Si, "CCD");
    new G4PVPlacement(0, G4ThreeVector(0,0,0), logicCCD, "CCD", logicWorld, false, 0, true);


    // 3. Source Encapsulation (The "Bead") to stop Np-237 recoil and alphas
    // A 1mm radius plastic bead at the source position.
    // G4Material* mat_Acrylic = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
    // G4Sphere* solidSource = new G4Sphere("SourceBead", 0, 1*mm, 0, 360*deg, 0, 180*deg);
    // G4LogicalVolume* logicSource = new G4LogicalVolume(solidSource, mat_Acrylic, "SourceBead");

    // must be placed where the GPSource is
    // new G4PVPlacement(0, G4ThreeVector(0, 0, -5.0*cm), logicSource, "SourceBead", logicWorld, false, 0, true);


    // 4. CCD shielding made of double aluminized mylar foil

    // a) First cover CCD in 10 micron thick aluminum box

    // Need to subtract off volume of CCD!!!
    G4double AlTot1_x = 150 * mm;
    G4double AlTot1_y = 150 * mm;
    G4double AlTot1_z = 0.650+0.02 * mm;

    G4double AlInt1_x = 90 * mm;
    G4double AlInt1_y = 22.5 * mm;
    G4double AlInt1_z = 0.650 * mm;

    G4Box* solidAlTot1 = new G4Box("SolidAlTot1", AlTot1_x/2, AlTot1_y/2, AlTot1_z/2);
    G4Box* solidAlInt1 = new G4Box("SolidAlInt1", AlInt1_x/2, AlInt1_y/2, AlInt1_z/2);
    G4SubtractionSolid* AlSheet1 = new G4SubtractionSolid("AlSheet1", solidAlTot1, solidAlInt1);
    G4LogicalVolume* logicAlSheet1 = new G4LogicalVolume(AlSheet1, mat_Al, "MylarSheet");
    new G4PVPlacement(0, G4ThreeVector(0,0,-1.0*cm), logicAlSheet1, "AlSheet1", logicWorld, false, 0, true);


    // a) First cover CCD in 10 micron thick mylar sheet
    // Sheet is made by subtracting interior solid mylar box from larger solid mylar box
    G4double MylarTot_x = 150.02 * mm;
    G4double MylarTot_y = 150.02 * mm;
    G4double MylarTot_z = 0.650+0.04 * mm;

    G4double MylarInt_x = 150 * mm;
    G4double MylarInt_y = 150 * mm;
    G4double MylarInt_z = 0.650+0.02 * mm;

    G4Box* solidMylarTot = new G4Box("SolidMylarTot", MylarTot_x/2, MylarTot_y/2, MylarTot_z/2);
    G4Box* solidMylarInt = new G4Box("SolidMylarInt", MylarInt_x/2, MylarInt_y/2, MylarInt_z/2);
    G4SubtractionSolid* MylarSheet = new G4SubtractionSolid("MylarSheet", solidMylarTot, solidMylarInt);
    G4LogicalVolume* logicMylarSheet = new G4LogicalVolume(MylarSheet, mat_Mylar, "MylarSheet");
    new G4PVPlacement(0, G4ThreeVector(0,0,-1.0*cm), logicMylarSheet, "MylarSheet", logicWorld, false, 0, true);

    // b) Outer layer of aluminum to make double sided aluminized mylar foil
    // Made by subtracting interior volume from total volume 
    G4double AlTot2_x = 150.04 * mm;
    G4double AlTot2_y = 150.04 * mm;
    G4double AlTot2_z = 0.650+0.06 * mm;

    G4double AlInt2_x = 150.02 * mm;
    G4double AlInt2_y = 150.02 * mm;
    G4double AlInt2_z = 0.650+0.04 * mm;

    G4Box* solidAlTot2 = new G4Box("SolidAlTot", AlTot2_x/2, AlTot2_y/2, AlTot2_z/2);
    G4Box* solidAlInt2 = new G4Box("SolidAlInt", AlInt2_x/2, AlInt2_y/2, AlInt2_z/2);
    G4SubtractionSolid* AlSheet2 = new G4SubtractionSolid("AlSheet2", solidAlTot2, solidAlInt2);
    G4LogicalVolume* logicAlSheet2 = new G4LogicalVolume(AlSheet2, mat_Al, "AlSheet");
    new G4PVPlacement(0, G4ThreeVector(0,0,-1.0*cm), logicAlSheet2, "AlSheet2", logicWorld, false, 0, true);


    // 6. CCD dead layer
    // A 2-um-thick layer of inactive silicon (approximating the partial-charge-collection layer and backside contact)
    G4double DeadLayer_z = 2.0*um;
    G4Box* solidSiDeadBox = new G4Box("SiDeadBox",ccd_x/2,ccd_y/2,DeadLayer_z);
    G4LogicalVolume* logicSiDeadBox = new G4LogicalVolume(solidSiDeadBox, mat_Si, "SiDeadBox");
    new G4PVPlacement(0, G4ThreeVector(0,0,-1.0*ccd_z), logicSiDeadBox, "SiDeadBox", logicWorld, false, 0, true);


    return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
    // Create the Sensitive Detector
    auto ccdSD = new CCDSensitiveDetector("CCD_SD", "HitsCollection");
    
    // Register it with the SD Manager
    G4SDManager::GetSDMpointer()->AddNewDetector(ccdSD);
    
    // Attach it to the Logical Volume named "CCD"
    SetSensitiveDetector("CCD", ccdSD);
}
