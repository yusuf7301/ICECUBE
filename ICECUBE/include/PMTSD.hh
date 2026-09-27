#ifndef PMTSD_h
#define PMTSD_h 1

#include "G4VSensitiveDetector.hh"

class PMTSD : public G4VSensitiveDetector
{
public:
    PMTSD(G4String name);
    virtual ~PMTSD();
    
    // Bir parçacık hacme girdiğinde/çarptığında çağrılır
    virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
};

#endif