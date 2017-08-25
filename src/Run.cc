#include "Run.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Track.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

// Select output format for Analysis Manager
#include "Analysis.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
Run::Run(PrimaryGeneratorAction* primary):G4Run(), particleGun(primary)
{
	G4SDManager* SDMan = G4SDManager::GetSDMpointer(); 
    	
    ID_eDep = SDMan->GetCollectionID("Si/eDep");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::~Run()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::RecordEvent(const G4Event* event)
{ 	
  	// Get hits collections
  	G4HCofThisEvent* HCE = event->GetHCofThisEvent();
  	if(!HCE) { 
    	G4ExceptionDescription msg; 
    	msg << "No hits collection of this event found.\n"; 
    	G4Exception("Run::RecordEvent()","Code001", JustWarning, msg); 
    	return; 
  	} 
  	
	// Zero out the variables
	G4double eDep = 0.;
	
	// Get the HitMaps for this event
	G4THitsMap<G4double>* event_eDep = (G4THitsMap<G4double>*)(HCE->GetHC(ID_eDep));
	
	std::map<G4int,G4double*>::iterator itr;
	
	// Get the total energy deposited for this event
	for (itr = event_eDep->GetMap()->begin(); itr != event_eDep->GetMap()->end(); itr++) {
		eDep += *(itr->second);
	}

	// Record events with non-zero deposited energy
	if (eDep > 0) {
		// Get analysis manager
		G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
			
		// Fill ntuple
		analysisManager->FillNtupleDColumn(0, particleGun->GetGPS()->GetParticleEnergy()/eV);
		analysisManager->FillNtupleDColumn(1, eDep/eV);
		analysisManager->AddNtupleRow();
	}
	
	// Invoke base class method
  	G4Run::RecordEvent(event); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
void Run::Merge(const G4Run* aRun)
{
   	const Run* localRun = static_cast<const Run*>(aRun);
   	
   	//  Invoke base class method
   	G4Run::Merge(aRun); 
} 
*/
