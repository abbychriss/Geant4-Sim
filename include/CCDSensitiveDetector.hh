#ifndef CCDSensitiveDetector_h
#define CCDSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include <map>
class G4Step;
class G4HCofThisEvent;

class CCDSensitiveDetector : public G4VSensitiveDetector
{
  public:
    CCDSensitiveDetector(const G4String& name, const G4String& hitsCollectionName);
    virtual ~CCDSensitiveDetector();
  
    // Methods required by Geant4
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    // Key: unique integer ID for the pixel (row*width + col)
    // Value: accumulated energy in each pixel
    std::map<G4int, G4double> fHitMap;
};

#endif
