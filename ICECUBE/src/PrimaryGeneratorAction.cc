#include "PrimaryGeneratorAction.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction(), fParticleGun(0)
{
    fParticleGun = new G4ParticleGun(1); // Olay başına 1 parçacık

    // Birincil parçacık olarak Muon (-) tanımlıyoruz
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("mu-");
    fParticleGun->SetParticleDefinition(particle);

    // Yüksek enerjili bir müon (Örn: 10 GeV)
    fParticleGun->SetParticleEnergy(10.0 * GeV);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.)); // Z yönünde hareket
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -20. * m)); // Buzu delecek şekilde dışarıdan başlat
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    fParticleGun->GeneratePrimaryVertex(anEvent);
}