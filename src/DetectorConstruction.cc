//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// ********************************************************************
// G4SiDetector.cc
//
// Description: Definition of the Canberra PD450-15-500AM Passivated
// Implanted Planar Silicon (PIPS) detector used by McMaster University
// to perform dosimetry measurements for the lens of the eye.
//
// NOTE1: McMaster is actually using the ORTEC CR-020-450-500 detector
// but the two models are essentially identical in terms of response.
//
// NOTE2: The BNC/Microdot connector on the back of the housing has not
// been included as part of this simulation.
// ********************************************************************

#include "DetectorConstruction.hh"
#include <cmath>

// Units and constants
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

// Manager classes
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4GeometryManager.hh"
#include "G4SDManager.hh"

// Store classes
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

// Geometry classes
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4VisAttributes.hh"

// Primitive geometry types
#include "G4Box.hh"
#include "G4Tubs.hh"

// Boolean operations on volumes
#include "G4UnionSolid.hh"

// Regions
#include "G4Region.hh"

// Messenger classes
#include "G4GenericMessenger.hh"

// Scoring Components
#include "G4MultiFunctionalDetector.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSTrackLength.hh"
#include "G4PSPassageTrackLength.hh"
#include "G4PSPassageCellCurrent.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction(): G4VUserDetectorConstruction(), fCheckOverlaps(true),
WorldPhysical(0)
{	
	// Geometry Parameters (Default)
	
    // Housing
	HOUSING_OD = 32.0*mm;					// Housing outer diameter
    HOUSING_H = 12.3*mm;                    // Housing height
    HOUSING_FRONT_T = 0.5*mm;               // Housing front thickness
    HOUSING_FRONT_OD = 23.9*mm;             // Housing front opening outer diameter
    HOUSING_BACK_T = 4.0*mm;                // Housing back cover thickness
    HOUSING_SIDE_T = 1.0*mm;                // Housing side thickness
		
	// Rotation Angle
	rotX = 0.0*deg;		
			 
	// Define Materials
	DefineMaterials();
	
	// Define commands to control the geometry
   	DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
    
    // NIST Manager
	G4NistManager* nistManager = G4NistManager::Instance();
	nistManager->SetVerbose(0);

  	// NIST materials
  	G4Material* galactic = nistManager->FindOrBuildMaterial("G4_Galactic");
    G4Material* Stainless_Steel = nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    G4Material* Brass = nistManager->FindOrBuildMaterial("G4_BRASS");
	  
  	// Set the materials for the Geometry
  	fMatWorld = galactic;
    fMatHousing = Stainless_Steel;
    //fMatChip = ;
    //fMatElastomer = ;
    //fMatLBI = ;
    fMatRC = Brass;
    //fMatRI = ;
  	
  	// Print materials
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{ 	
	// Cleanup old geometry
  	G4GeometryManager::GetInstance()->OpenGeometry();
  	G4PhysicalVolumeStore::GetInstance()->Clean();
  	G4LogicalVolumeStore::GetInstance()->Clean();
  	G4SolidStore::GetInstance()->Clean();	
  	
	////////////////////////////////////////////////////////////////////////
	// Construct The World Volume

	G4double world_X = 6*cm;
	G4double world_Y = world_X;
	G4double world_Z = world_X;
	
	G4Box* WorldSolid = new G4Box(	"World", world_X/2, world_Y/2, world_Z/2);
  
	WorldLogical = 
		new G4LogicalVolume(WorldSolid,						// The Solid
							fMatWorld,						// Material
							"World");						// Name
  
	WorldPhysical = 
		new G4PVPlacement(	0,								// Rotation
							G4ThreeVector(),				// Translation vector
							WorldLogical,					// Logical volume
							"World",						// Name
							0,								// Mother volume
							false,							// Unused boolean parameter
							0,								// Copy number
							fCheckOverlaps);				// Overlap Check	
							
	////////////////////////////////////////////////////////////////////////
	// Construct the detector housing	
	
	G4VSolid* Housing_Solid = new G4Tubs("Housing_Solid", 0., HOUSING_OD/2, HOUSING_H/2, 0., 360.*deg);
	
	HousingLogical = 
		new G4LogicalVolume(Housing_Solid,					// The Solid
							fMatHousing,    				// Material
							"Housing_Logical");     		// Name

    // Rotation matrix for the detector
    G4RotationMatrix Housing_Rotation = G4RotationMatrix();
    Housing_Rotation.rotateX(rotX);

	// Rotation, Translation, and Transformation of the detector			
	G4RotationMatrix Housing_Rot = G4RotationMatrix();
	Housing_Rot.rotateX(rotX);
	
	G4ThreeVector Housing_Trans = G4ThreeVector(0,0,0);

						
	HousingPhysical = 
		new G4PVPlacement(	G4Transform3D(Housing_Rot,Housing_Trans),	// Translation
							HousingLogical,					// Logical volume
							"Housing_Physical",		        // Name
							WorldLogical,					// Mother volume
							false,							// Unused boolean parameter
							0,								// Copy number
							fCheckOverlaps);				// Overlap Check
	
	////////////////////////////////////////////////////////////////////////
  	// Visualisation attributes
  	
  	// World Volume (White)
  	G4VisAttributes* Vis_World = new G4VisAttributes(G4Colour(1.,1.,1.,0.1));
  	Vis_World->SetForceWireframe(true);
  	WorldLogical->SetVisAttributes(Vis_World);

    // Housing Volume (Gray)
    G4VisAttributes* Vis_Housing = new G4VisAttributes(G4Colour(0.5,0.5,0.5,1.));
    Vis_Housing->SetForceWireframe(true);
    HousingLogical->SetVisAttributes(Vis_Housing);

	////////////////////////////////////////////////////////////////////////
	// Return world volume
	return WorldPhysical; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
	////////////////////////////////////////////////////////////////////////
	// Construct the Multi Functional Detector for the Si chip
	
	G4MultiFunctionalDetector* SiScorer = new G4MultiFunctionalDetector("Si");
	G4SDManager::GetSDMpointer()->AddNewDetector(SiScorer);	
	G4SDManager::GetSDMpointer()->SetVerboseLevel(0);
	//HousingLogical->SetSensitiveDetector(SiScorer);
 	
 	G4PSEnergyDeposit* eDep = new G4PSEnergyDeposit("eDep");
    SiScorer->RegisterPrimitive(eDep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetDetectorAngle(G4double val)
{
	if(WorldPhysical) {    
    	G4Exception ("DetectorConstruction::SetDetectorAngle()", "G4SiDetector", 
                 	JustWarning, 
                 	"Attempt to change already constructed geometry is ignored");
  	} else {
   		rotX = val;
  	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double DetectorConstruction::GetDetectorAngle()
{
	// Return the detector angle
	return rotX;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineCommands()
{
    // Define /CNPTEPC/ command directory using generic messenger class
    fMessenger = new G4GenericMessenger(this, "/G4SiDetector/", "Geometry control");

    // Converter Thickness Command
    G4GenericMessenger::Command& DetectorAngleCmd
      = fMessenger->DeclareMethodWithUnit("DetectorAngle","deg",
                                  &DetectorConstruction::SetDetectorAngle, 
                                  "Set the angle of the detector within the world volume.");
    DetectorAngleCmd.SetParameterName("angle", true);
    DetectorAngleCmd.SetDefaultValue("0.0");
}
