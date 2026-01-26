#include "ActionInitialization.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

// --- Internal Class: Simple GPS Generator ---
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction() {
        fGPS = new G4GeneralParticleSource();
    }
    virtual ~PrimaryGeneratorAction() {
        delete fGPS;
    }
    virtual void GeneratePrimaries(G4Event* anEvent) {
        fGPS->GeneratePrimaryVertex(anEvent);
    }
private:
    G4GeneralParticleSource* fGPS;
};
// --------------------------------------------

ActionInitialization::ActionInitialization()
 : G4VUserActionInitialization()
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
    SetUserAction(new RunAction());
}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction());
    SetUserAction(new RunAction());
    SetUserAction(new SteppingAction());
}
