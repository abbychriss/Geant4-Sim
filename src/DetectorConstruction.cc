#include "DetectorConstruction.hh"
#include "CCDSensitiveDetector.hh" // We need this to attach the SD

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
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
    G4Material* mat_Acrylic = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
    G4Sphere* solidSource = new G4Sphere("SourceBead", 0, 1*mm, 0, 360*deg, 0, 180*deg);
    G4LogicalVolume* logicSource = new G4LogicalVolume(solidSource, mat_Acrylic, "SourceBead");

    //must be placed where the GPSource is
    new G4PVPlacement(0, G4ThreeVector(0, 0, -5.0*cm), logicSource, "SourceBead", logicWorld, false, 0, true);


    // 4. CCD cover lid
    // A 2.5mm thick aluminum sheet covering the CCD
    G4double AlBox_x = 150 * mm;
    G4double AlBox_y = 150 * mm;
    G4double AlBox_z = 2.5 * mm;

    G4Box* solidAlBox = new G4Box("AlBox", AlBox_x/2, AlBox_y/2, AlBox_z/2);
    G4LogicalVolume* logicAlBox = new G4LogicalVolume(solidAlBox, mat_Al, "AlBox");
    new G4PVPlacement(0, G4ThreeVector(0,0,-1.0*cm), logicAlBox, "AlBox", logicWorld, false, 0, true);


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
