#include "RunAction.hh"
#include "Run.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4SystemOfUnits.hh"

// Select output format for Analysis Manager
#include "Analysis.hh"

#include <stdio.h>
#include <time.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(DetectorConstruction* det, PrimaryGeneratorAction* primary):G4UserRunAction(),
detector(det), particleGun(primary)
{
	// Set printing event number per each event
  	G4RunManager::GetRunManager()->SetPrintProgress(1E6);  
  	
	// Set starting seed for the Random Number Generator
	long seeds[2];
	time_t systime = time(NULL);
	seeds[0] = (long) systime;
	seeds[1] = (long) (systime*G4UniformRand());  
    G4Random::setTheSeeds(seeds);
  	
  	// Create analysis manager 
  	// The choice of analysis technology is done via selection of an appropriate namespace
  	// (g4root, g4xml, or g4csv)
  	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance(); 
  	//G4cout << "Using " << analysisManager->GetType() << G4endl; 
  	
	// Default settings 
  	analysisManager->SetVerboseLevel(0); 

	// Merge the ntuples from all of the worker threads
	analysisManager->SetNtupleMerging(true);

	// Create histograms
	G4double ELow = 10.;
    G4double EHigh = 10000.;
    G4int nBins = 90;

    G4double dlog = (std::log10(EHigh) - std::log10(ELow))/nBins;
    G4double dx = std::pow(10, dlog);
    G4double binValue = ELow;

	std::vector<G4double> Edges;

    // Set Bin Edges for Logarithmically Binned Histogram
    while ( G4int(Edges.size()) <= nBins ) {
        Edges.push_back(binValue);
        binValue *= dx;
	} 

	analysisManager->CreateH1("Source Fluence (Gamma)", "Source Fluence Spectrum for Gammas", Edges);
	analysisManager->CreateH1("Source Fluence (Electron)", "Source Fluence Spectrum for Electrons", Edges);
	analysisManager->CreateH1("Detector Measured Spectrum", "Detector Measured Energy Spectrum", Edges);
	analysisManager->CreateH2("Energy Migration Matrix (Gamma)", "Energy Migration Matrix for Gammas", Edges, Edges);
	analysisManager->CreateH2("Energy Migration Matrix (Electron)", "Energy Migration Matrix for Electrons", Edges, Edges);
  	
  	// Create ntuple
	/*
 	analysisManager->CreateNtuple("G4SiDetector", "Etrue Edep");
	analysisManager->CreateNtupleDColumn("eTrue");
 	analysisManager->CreateNtupleDColumn("eDep");
 	analysisManager->FinishNtuple();
	*/
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
	// Delete analysis manager
	delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run* RunAction::GenerateRun()
{ 
	return new Run(detector, particleGun); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* aRun)
{
	// Get analysis manager
  	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  	// Open an AnalysisManager data file for the worker threads
  	if (!IsMaster()){
		// Filename for AnalysisManager is provided in the macro file using
		// /analysis/setFileName command
		analysisManager->OpenFile();
  	}
  	
  	// For the master let's create an info file
  	if (IsMaster()){
		// Filename for AnalysisManager is provided in the macro file using
		// /analysis/setFileName command and the RunID is appended at the end
		
		//analysisManager->SetFileName(analysisManager->GetFileName()(0:10) + "_" + G4UIcommand::ConvertToString(aRun->GetRunID()));
		analysisManager->OpenFile();
		
		// Get the local time at the start of the simulation
		time_t now = time(0);
		
		// Create an information file for the run using the same filename as the Analysis Manager
    	outputFile_INFO = analysisManager->GetFileName() + ".info";
    	
    	// Open the Information File
		pFile_INFO = fopen(outputFile_INFO,"w+");
		std::ofstream outFile_INFO(outputFile_INFO);
		
		// Export Source Information
		outFile_INFO << "============================    Simulation Information    ============================" << G4endl;
		outFile_INFO << "Start Time: \t\t" <<  ctime(&now);
  	}
  	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* aRun)
{ 	
  	// Output & close analysis file 
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance(); 
	analysisManager->Write();
  	analysisManager->CloseFile();
  	
  	// Append Source Information to the INFO file
  	if (IsMaster()){ 
		// Open the Information File
		std::ofstream outFile_INFO(outputFile_INFO,std::ios::out|std::ios::app);
		
		// Get the local time at the end of the simulation
		time_t now = time(0);
    	
    	// Export Source Information
    	outFile_INFO << "End Time: \t\t" <<  ctime(&now);
		outFile_INFO << "============================    Source Information    ============================" << G4endl;
		outFile_INFO << "Source Radius: \t\t" << detector->GetSourceRadius()/cm << " cm" << G4endl;	
		outFile_INFO << "Number of Events: \t" << aRun->GetNumberOfEvent() << G4endl;	
		outFile_INFO << "============================    Detector Information    ============================" << G4endl;
		outFile_INFO << "Detector Angle: \t" << detector->GetDetectorAngle()/degree << " deg" << G4endl;	
		outFile_INFO << "==================================================================================" << G4endl; 
		
		// Close file
		fclose(pFile_INFO);
  	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
