#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4GenericMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction
{
    public:
  	    // Constructor
        DetectorConstruction();
        // Destructor
        virtual ~DetectorConstruction();

        // Defines the detector geometry and returns a pointer to the physical World Volume
        virtual G4VPhysicalVolume* Construct();
    
        // Sensitive Detector 
	    virtual void ConstructSDandField();
    
        // Set Methods
        void SetDetectorAngle(G4double val);
    
        // Get Methods
        G4double GetDetectorAngle();
    
    private:
        // Defines all the detector materials
        void DefineMaterials();
    
        // Define commands to change the geometry
        void DefineCommands();
    
        G4GenericMessenger* fMessenger;
        G4bool fCheckOverlaps;
    
        // Standard Materials
        G4Material* fMatWorld;
        G4Material* fMatHousing;
        G4Material* fMatRC;
    
        // Logical Volumes
        G4LogicalVolume* WorldLogical;
        G4LogicalVolume* HousingLogical;
    
        // Physical Volumes
        G4VPhysicalVolume* WorldPhysical;
        G4VPhysicalVolume* HousingPhysical;
    
        // Geometry Parameters
        G4double HOUSING_OD;
        G4double HOUSING_H;
        G4double HOUSING_FRONT_T;
        G4double HOUSING_FRONT_OD;
        G4double HOUSING_BACK_T;
        G4double HOUSING_SIDE_T;
	
	    // Rotation Angles
	    G4double rotX;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

