#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction() override;

    virtual G4VPhysicalVolume* Construct() override;
    virtual void ConstructSDandField() override;

private:
    // --- Modüler Kurulum Fonksiyonları ---
    void DefineMaterials();
    void SetupOpticalProperties();
    void ConstructDOMTemplate();
    void PlaceStringsAndDOMs();

    // --- Sınıf İçi (Member) Hacim Değişkenleri ---
    // (Geant4 standartlarında sınıf değişkenleri 'f' öneki ile başlar)
    G4LogicalVolume* fLogicWorld;
    G4LogicalVolume* fLogicHexTank;
    G4LogicalVolume* fLogicDOM;
    G4LogicalVolume* fLogicPMT;
    G4LogicalVolume* fLogicPCB;

    G4VPhysicalVolume* fPhysWorld;

    // --- Dedektör Parametreleri ---
    G4int    fTargetStringCount;     // Hedeflenen kablo sayısı (Optimize: 10)
    G4int    fDOMsPerString;         // Kablo başına DOM (60)
    G4double fStringPitch;           // Kablolar arası yatay mesafe (125 m)
    G4double fDOMVerticalSpacing;    // DOM'lar arası dikey mesafe (17 m)
};

#endif