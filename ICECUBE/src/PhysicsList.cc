#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4DecayPhysics.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
    // Standart elektromanyetik etkileşimler
    RegisterPhysics(new G4EmStandardPhysics());
    
    // Parçacık bozunmaları (Müonlar için gerekli)
    RegisterPhysics(new G4DecayPhysics());

    // Optik fizik (Çerenkov ışıması vb.)
    G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
    RegisterPhysics(opticalPhysics);
}

PhysicsList::~PhysicsList() {}