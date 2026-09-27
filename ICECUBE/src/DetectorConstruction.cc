#include "DetectorConstruction.hh"
#include "PMTSD.hh"

#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4SystemOfUnits.hh"
#include "G4TwoVector.hh"

#include <vector>
#include <cmath>

// ==============================================================================
// Kurucu Metot (Constructor) - Başlangıç Parametreleri Atanır
// ==============================================================================
DetectorConstruction::DetectorConstruction() 
  : G4VUserDetectorConstruction(), 
    fLogicWorld(nullptr), fLogicHexTank(nullptr), fLogicDOM(nullptr), 
    fLogicPMT(nullptr), fLogicPCB(nullptr), fPhysWorld(nullptr),
    fTargetStringCount(86), // Aşırı yüklenmeyi önlemek için 10'da tutulmuştur
    fDOMsPerString(60),
    fStringPitch(125.0 * m),
    fDOMVerticalSpacing(17.0 * m)
{ }

DetectorConstruction::~DetectorConstruction() 
{ }

// ==============================================================================
// Ana İnşa Fonksiyonu - Alt fonksiyonları orkestra gibi yönetir
// ==============================================================================
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    DefineMaterials();
    
    // --- 1. Devasa World Hacmi ---
    G4double worldSize = 1500.0 * m; 
    G4Box* solidWorld = new G4Box("World", worldSize, worldSize, worldSize);
    
    G4Material* ice = G4Material::GetMaterial("G4_WATER");
    fLogicWorld = new G4LogicalVolume(solidWorld, ice, "World");
    fPhysWorld = new G4PVPlacement(0, G4ThreeVector(), fLogicWorld, "World", 0, false, 0, true);

    // --- 2. Altıgen Dedektör Sınırı (Hexagonal Tank) ---
    G4double zPlanes[] = { -600.0*m, 600.0*m }; 
    G4double rInner[] = { 0.0, 0.0 };           
    G4double rOuter[] = { 600.0*m, 600.0*m };   

    G4Polyhedra* solidHexTank = new G4Polyhedra("HexTank", 30.*deg, 360.*deg, 6, 2, zPlanes, rInner, rOuter);
    fLogicHexTank = new G4LogicalVolume(solidHexTank, ice, "HexTank");
    
    new G4PVPlacement(0, G4ThreeVector(), fLogicHexTank, "HexTank_Phys", fLogicWorld, false, 0, false);

    // --- 3. Bileşenleri Oluştur ---
    ConstructDOMTemplate();
    PlaceStringsAndDOMs();
    SetupOpticalProperties();

    return fPhysWorld;
}

// ==============================================================================
// Malzemelerin Tanımlanması
// ==============================================================================
void DetectorConstruction::DefineMaterials()
{
    G4NistManager* nist = G4NistManager::Instance();
    nist->FindOrBuildMaterial("G4_WATER");
    nist->FindOrBuildMaterial("G4_Pyrex_Glass");
    nist->FindOrBuildMaterial("G4_Galactic");
    nist->FindOrBuildMaterial("G4_BAKELITE");
}

// ==============================================================================
// Tek Bir DOM Şablonunun Yaratılması
// ==============================================================================
void DetectorConstruction::ConstructDOMTemplate()
{
    G4Material* glass  = G4Material::GetMaterial("G4_Pyrex_Glass");
    G4Material* vacuum = G4Material::GetMaterial("G4_Galactic");
    G4Material* pcbMat = G4Material::GetMaterial("G4_BAKELITE");

    // 1. Ana Cam Muhafaza
    G4double domRadius = 16.5 * cm;
    G4double glassThickness = 1.2 * cm;
    G4Sphere* solidDOM = new G4Sphere("DOM_Glass", 0, domRadius, 0., 360.*deg, 0., 180.*deg);
    fLogicDOM = new G4LogicalVolume(solidDOM, glass, "DOM_Glass");

    // 2. Alt Yarı - PMT (Vakum)
    G4double pmtRadius = domRadius - glassThickness;
    G4Sphere* solidPMT = new G4Sphere("PMT", 0, pmtRadius, 0., 360.*deg, 90.*deg, 180.*deg); 
    fLogicPMT = new G4LogicalVolume(solidPMT, vacuum, "PMT");
    new G4PVPlacement(0, G4ThreeVector(), fLogicPMT, "PMT_Phys", fLogicDOM, false, 0, false);

    // 3. Üst Yarı - PCB (Elektronik Kart)
    G4double pcbRadius = 14.0 * cm;
    G4double pcbHalfZ = 0.5 * cm;
    G4Tubs* solidPCB = new G4Tubs("PCB", 0., pcbRadius, pcbHalfZ, 0., 360.*deg);
    fLogicPCB = new G4LogicalVolume(solidPCB, pcbMat, "PCB");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 5.0*cm), fLogicPCB, "PCB_Phys", fLogicDOM, false, 0, false);
}

// ==============================================================================
// DOM'ların Altıgen Grid Üzerine Dizilmesi
// ==============================================================================
void DetectorConstruction::PlaceStringsAndDOMs()
{
    std::vector<G4TwoVector> stringPositions;

    // Altıgen Merkezden Dışa Doğru Kordinat Hesaplama
    for (int q = -5; q <= 5; ++q) {
        for (int r = -5; r <= 5; ++r) {
            if (std::abs(q + r) <= 5) { 
                G4double xPos = fStringPitch * (q + r / 2.0);
                G4double yPos = fStringPitch * (r * std::sqrt(3.0) / 2.0);
                stringPositions.push_back(G4TwoVector(xPos, yPos));
                
                if (stringPositions.size() >= static_cast<size_t>(fTargetStringCount)) break; 
            }
        }
        if (stringPositions.size() >= static_cast<size_t>(fTargetStringCount)) break;
    }

    // Pozisyonlara Göre Fiziksel Yerleştirme
    for (size_t strIdx = 0; strIdx < stringPositions.size(); ++strIdx) {
        G4double x = stringPositions[strIdx].x();
        G4double y = stringPositions[strIdx].y();

        for (G4int domIdx = 0; domIdx < fDOMsPerString; ++domIdx) {
            G4double zPos = (domIdx - (fDOMsPerString - 1) / 2.0) * fDOMVerticalSpacing; 
            G4int copyNo = (strIdx + 1) * 100 + domIdx; // ID formatı: StringNo + DOMNo

            new G4PVPlacement(
                0, 
                G4ThreeVector(x, y, zPos), 
                fLogicDOM, 
                "DOM_Placement", 
                fLogicHexTank, 
                false, 
                copyNo, 
                false // Çakışma denetimi performans için kapalı
            );
        }
    }
}

// ==============================================================================
// Optik Fizik (Çerenkov) Özelliklerinin Atanması
// ==============================================================================
void DetectorConstruction::SetupOpticalProperties()
{
    G4Material* ice = G4Material::GetMaterial("G4_WATER");

    G4double photonEnergy[] = { 2.034*eV, 2.480*eV, 3.100*eV, 4.136*eV }; 
    const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double);

    // Buzun Optik Malzeme Tablosu
    G4MaterialPropertiesTable* iceMPT = ice->GetMaterialPropertiesTable();
    if (!iceMPT) iceMPT = new G4MaterialPropertiesTable();

    G4double refractiveIndexIce[] = { 1.31, 1.31, 1.31, 1.31 }; 
    G4double absorptionLengthIce[] = { 100.0*m, 100.0*m, 100.0*m, 100.0*m }; 

    iceMPT->AddProperty("RINDEX", photonEnergy, refractiveIndexIce, nEntries);
    iceMPT->AddProperty("ABSLENGTH", photonEnergy, absorptionLengthIce, nEntries);
    ice->SetMaterialPropertiesTable(iceMPT);

    // PMT'nin Işık Emen Sensör Yüzeyi
    G4OpticalSurface* pmtSurface = new G4OpticalSurface("PMTSurface");
    pmtSurface->SetType(dielectric_metal); 
    pmtSurface->SetFinish(polished);       
    pmtSurface->SetModel(glisur);          

    G4double efficiency[] = { 0.25, 0.25, 0.25, 0.25 }; 
    G4double reflectivity[] = { 0.0, 0.0, 0.0, 0.0 };   
    
    G4MaterialPropertiesTable* pmtMPT = new G4MaterialPropertiesTable();
    pmtMPT->AddProperty("EFFICIENCY", photonEnergy, efficiency, nEntries);
    pmtMPT->AddProperty("REFLECTIVITY", photonEnergy, reflectivity, nEntries);
    pmtSurface->SetMaterialPropertiesTable(pmtMPT);

    new G4LogicalSkinSurface("PMTSkinSurface", fLogicPMT, pmtSurface);
}

// ==============================================================================
// Hassas Dedektör (Sensör) Bağlantısı
// ==============================================================================
void DetectorConstruction::ConstructSDandField()
{
    PMTSD* pmtSD = new PMTSD("PMTSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD);
    
    // PMT mantıksal hacmine çarpan her foton artık ROOT dosyasına yazılacak
    if (fLogicPMT != nullptr) {
        SetSensitiveDetector(fLogicPMT, pmtSD);
    }
}