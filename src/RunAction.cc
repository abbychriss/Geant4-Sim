#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"

RunAction::RunAction()
 : G4UserRunAction()
{
  // 1. Get the Analysis Manager instance
  // Geant4 automatically handles the output format based on extension, 
  // but we force it here to be safe.
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetDefaultFileType("csv"); 
  // analysisManager->SetNtupleMerging(true); // needed for multithreading so outputs get merged at end

  // 2. Set the default file name
  // This will result in a file named "Fe55_Data.csv"
  analysisManager->SetFileName("Fe55_1cm_DoubleMylarFoil_Data");

  // Ntuple 0: data from detector hits
  analysisManager->CreateNtuple("Hits", "Pixel Hits");
  analysisManager->CreateNtupleIColumn("EventID"); // Vital for reconstruction!
  analysisManager->CreateNtupleIColumn("PixelX");
  analysisManager->CreateNtupleIColumn("PixelY");
  analysisManager->CreateNtupleDColumn("Edep_keV");
  analysisManager->CreateNtupleDColumn("ElectronHolePairs");
  analysisManager->CreateNtupleSColumn("ParticleName");
  analysisManager->FinishNtuple(0);

  // Optional: Console logging to confirm setup
  G4cout << "RunAction::RunAction: Ntuple 'Hits' created." << G4endl;

  // 3. Second ntuple to record the energies of secondary particles generated in decays of source
  // Create ntuple 1: secondary energies
  analysisManager->CreateNtuple("Secondaries", "Secondary Particles");
  analysisManager->CreateNtupleIColumn("EventID"); // Column 0: Event ID
  analysisManager->CreateNtupleDColumn("Ekin"); // Column 1: Kinetic Energy
  analysisManager->CreateNtupleSColumn("ParticleName"); // Column 1: Kinetic Energy
  analysisManager->FinishNtuple(1);

  G4cout << "RunAction::RunAction: Ntuple 'Secondaries' created." << G4endl;

}

RunAction::~RunAction()
{
  // The Analysis Manager is a singleton, so we don't usually delete it here.
}

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  // 4. Open the file at the start of the run
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->OpenFile();
}

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // 5. Write and Close the file at the end
  auto analysisManager = G4AnalysisManager::Instance();
  
  analysisManager->Write();
  analysisManager->CloseFile();
}

