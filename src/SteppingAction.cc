#include "SteppingAction.hh"
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
}
