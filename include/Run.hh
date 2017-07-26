#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class PrimaryGeneratorAction;

class Run : public G4Run
{
	public:
		// Constructor
  		Run(PrimaryGeneratorAction*);
  		// Destructor
  		virtual ~Run();
		
		// Methods
		virtual void RecordEvent(const G4Event*);
// 		virtual void Merge(const G4Run*);

	private:
		PrimaryGeneratorAction* particleGun;
		G4int ID_eDep;
};

#endif