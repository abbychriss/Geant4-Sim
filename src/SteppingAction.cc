#include "SteppingAction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction() : G4UserSteppingAction()
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track = step->GetTrack();

    // 1. Do NOT kill the primary particle (Track ID 1)
    // This saves your Am-241 source.
    if (track->GetTrackID() == 1) return;

    // 2. Optional: Do NOT kill Ions/Nuclei (just to be safe)
    if (track->GetDefinition()->GetParticleType() == "nucleus") return;

    // 3. The Zombie Filter (Only applied to non-primaries/non-nuclei)
    if (track->GetKineticEnergy() < 100 * eV) {
        track->SetTrackStatus(fStopAndKill);
    }

    auto analysisManager = G4AnalysisManager::Instance();
    // Get list of secondaries produced in this step
    const auto* secondaryTracks = step->GetSecondaryInCurrentStep();

        if (!secondaryTracks) return;
            for (const auto& track : *secondaryTracks) {
                // Get the particle definition (optional: filter by particle type)
                // G4String name = track->GetDefinition()->GetParticleName();
                
                // Get Kinetic Energy
                G4double eKin = track->GetKineticEnergy();
                
                // Get event ID
                G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
                
                // Fill ntuple
                analysisManager->FillNtupleDColumn(1, 0, eventID); // Ntuple 1, Column 0
                analysisManager->FillNtupleDColumn(1, 1, eKin / keV); // Ntuple 1, Column 0
                analysisManager->AddNtupleRow(1); // Save row
            }
}