#include "CCDSensitiveDetector.hh"
#include "G4AnalysisManager.hh" 
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "Randomize.hh"

// Constants for your CCD
const G4double PIXEL_SIZE = 15.0 * um;
const G4int N_PIX_X = 6000; // 90 mm / 15 um
const G4int N_PIX_Y = 1500; // 22.5 mm / 15 um

CCDSensitiveDetector::CCDSensitiveDetector(const G4String& name, const G4String& hitsCollectionName)
 : G4VSensitiveDetector(name)
{}

CCDSensitiveDetector::~CCDSensitiveDetector() {}

void CCDSensitiveDetector::Initialize(G4HCofThisEvent*)
{
  // Clear the map at the start of every event
  fHitMap.clear();
}

G4bool CCDSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4Track* track = step->GetTrack();
  G4double edep = step->GetTotalEnergyDeposit();

  // --- THE GATEKEEPER (Corrected) ---

  // Logic: If it is NOT the primary particle AND it was born weak (< 100 eV), reject it.
  if (track->GetTrackID() != 1 && 
      track->GetVertexKineticEnergy() < 100 * CLHEP::eV) {
      return false; 
  }
    
  // Standard check for zero energy steps
  if (edep <= 0.0) return false;
    
    
  // 1. Get Local Coordinates
  // We transform the global position of the hit into the coordinate system 
  // of the CCD volume (where 0,0 is the center).
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4TouchableHandle touchable = preStepPoint->GetTouchableHandle();
  G4ThreeVector worldPos = preStepPoint->GetPosition();
  G4ThreeVector localPos = touchable->GetHistory()->GetTopTransform().TransformPoint(worldPos);

  // 2. Convert to Pixel Indices (i, j)
  // Local pos ranges from -HalfSize to +HalfSize.
  // We shift by HalfSize to make it 0 to FullSize.
  G4double halfX = (N_PIX_X * PIXEL_SIZE) / 2.0;
  G4double halfY = (N_PIX_Y * PIXEL_SIZE) / 2.0;

  // Floor() ensures we bin into integer indices
  G4int ix = std::floor((localPos.x() + halfX) / PIXEL_SIZE);
  G4int iy = std::floor((localPos.y() + halfY) / PIXEL_SIZE);

  // Safety check to keep indices inside the array (glancing hits on edges)
  if (ix < 0) ix = 0; if (ix >= N_PIX_X) ix = N_PIX_X - 1;
  if (iy < 0) iy = 0; if (iy >= N_PIX_Y) iy = N_PIX_Y - 1;

  // 3. Create Unique Pixel ID (Flattened Index)
  G4int pixelID = iy * N_PIX_X + ix;

  // 4. Accumulate Energy
  fHitMap[pixelID] += edep;

  return true;
}

void CCDSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
  // If no pixels were hit, do nothing
  if (fHitMap.empty()) return;

  auto analysisManager = G4AnalysisManager::Instance();
  G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

  // Physics Constants
  const G4double epsilon = 3.74 * eV;
  const G4double Fano = 0.115;

  // Loop over every pixel that saw energy this event
  for (auto const& [pixelID, energy] : fHitMap) {
      
      // Calculate Charge for THIS pixel
      G4double mean_N = energy / epsilon;
      G4double sigma_N = std::sqrt(Fano * mean_N);
      G4double measured_N = G4RandGauss::shoot(mean_N, sigma_N);

      // Reconstruct Indices
      G4int iy = pixelID / N_PIX_X;
      G4int ix = pixelID % N_PIX_X;

      // Fill CSV
      // Col 0: Event ID (So you can group pixels back into one "image")
      analysisManager->FillNtupleIColumn(0, 0, eventID);
      
      // Col 1: Pixel X
      analysisManager->FillNtupleIColumn(0, 1, ix);
      
      // Col 2: Pixel Y
      analysisManager->FillNtupleIColumn(0, 2, iy);
      
      // Col 3: Energy (keV)
      analysisManager->FillNtupleDColumn(0, 3, energy / keV);
      
      // Col 4: Charge (e-)
      analysisManager->FillNtupleDColumn(0, 4, measured_N);

      analysisManager->AddNtupleRow(0);
  }
}
