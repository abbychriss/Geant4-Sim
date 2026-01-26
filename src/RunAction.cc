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

  // 2. Set the default file name
  // This will result in a file named "Am241_Data.csv"
  analysisManager->SetFileName("Am241_Data");



  analysisManager->CreateNtuple("Hits", "Pixel Hits");

  analysisManager->CreateNtupleIColumn("EventID"); // Vital for reconstruction!
  analysisManager->CreateNtupleIColumn("PixelX");
  analysisManager->CreateNtupleIColumn("PixelY");
  analysisManager->CreateNtupleDColumn("Edep_keV");
  analysisManager->CreateNtupleDColumn("ElectronHolePairs");

  analysisManager->FinishNtuple();
  
  // Optional: Console logging to confirm setup
  G4cout << "RunAction::RunAction: Ntuple 'Hits' created." << G4endl;


} // <--- This was likely missing!

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
