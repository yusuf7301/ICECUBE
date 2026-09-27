#include "PMTSD.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

PMTSD::PMTSD(G4String name) : G4VSensitiveDetector(name) {}
PMTSD::~PMTSD() {}

G4bool PMTSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4Track* track = aStep->GetTrack();
    
    // Sadece optik fotonları (Çerenkov) kaydetmek istiyoruz
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
        return false;
    }

    // Foton PMT hacmine girdi (hit oluştu)
    G4double timeOfFlight = track->GetGlobalTime();
    G4double photonEnergy = track->GetKineticEnergy();
    
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    // Verileri ROOT dosyasına (RunAction'da oluşturulan Ntuple'a) yazıyoruz
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleIColumn(0, eventID);
    analysisManager->FillNtupleDColumn(1, timeOfFlight);
    analysisManager->FillNtupleDColumn(2, photonEnergy);
    analysisManager->AddNtupleRow();

    // Fotonu tespit ettikten sonra simülasyonda yok et (PMT fotonu absorbe etti)
    track->SetTrackStatus(fStopAndKill);

    return true;
}