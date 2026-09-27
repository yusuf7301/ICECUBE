#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv) {
    auto* runManager = G4RunManagerFactory::CreateRunManager();

    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new PhysicsList());
    runManager->SetUserInitialization(new ActionInitialization());

    runManager->Initialize();

    G4UIExecutive* ui = nullptr;
    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);
    }

    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

// ... (Üst kısımdaki tanımlamalar aynı kalacak)

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if (ui) {
        // Etkileşimli (Interactive) mod: Argüman verilmediğinde çalışır (sadece ./IceCubeSim yazdığında)
        UImanager->ApplyCommand("/control/execute vis.mac"); // Makroyu çalıştır (çizimi yap)
        ui->SessionStart(); // BU SATIR PROGRAMIN AÇIK KALMASINI SAĞLAR! (Kullanıcı etkileşimi başlar)
        delete ui;
    } else {
        // Toplu İş (Batch) modu: Dışarıdan makro verildiğinde çalışır (./IceCubeSim vis.mac)
        // Sorun tam olarak burada! Batch modunda dosya bitince program kapanır.
        
        // Batch modunda da UI'ı zorla başlatmak için kodu şöyle değiştirebilirsin:
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
        
        // ui işaretçisi null ise yeni bir tane oluşturup oturumu açık tutuyoruz
        ui = new G4UIExecutive(argc, argv); 
        ui->SessionStart(); // Ekranın kapanmasını engeller!
        delete ui;
    }

    delete visManager;
    delete runManager;
    return 0;
}