#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

// Your custom classes (assumed header names)
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh" 

// Physics List
#include "G4PhysListFactory.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "FTFP_BERT.hh"

int main(int argc, char** argv)
{
    // 1. Construct the Run Manager
    // Serial is easier for debugging, Multithreaded is faster for stats.
    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);
    runManager->SetNumberOfThreads(32);   // optional set number of threads

    // 2. Set Mandatory Initialization Classes
    
    // Geometry
    runManager->SetUserInitialization(new DetectorConstruction());

    // Physics List
    // We start with a modular list (FTFP_BERT) and replace the EM part 
    // with Option4 (best for low energy/Fano factor work)
    auto* physList = new FTFP_BERT;
    physList->ReplacePhysics(new G4EmStandardPhysics_option4());
    physList->RegisterPhysics(new G4RadioactiveDecayPhysics());
    runManager->SetUserInitialization(physList);

    // User Actions (Generator, RunAction, EventAction, etc.)
    runManager->SetUserInitialization(new ActionInitialization());

    // 3. Initialize Visualization
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // 4. Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // 5. Process Inputs
    if (argc > 1) {
        // Batch mode: execute macro file (e.g., ./Am241_Sim run.mac)
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }
    else {
        // Interactive mode: define UI session
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        UImanager->ApplyCommand("/control/execute init_vis.mac"); 
        if (ui->IsGUI()) {
            UImanager->ApplyCommand("/control/execute gui.mac");
        }
        ui->SessionStart();
        delete ui;
    }

    // 6. Job Termination
    delete visManager;
    delete runManager;
    return 0;
}
