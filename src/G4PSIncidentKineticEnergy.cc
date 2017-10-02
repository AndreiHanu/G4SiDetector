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
// G4PSIncidentKineticEnergy.cc
//
// Description: This is a custom primitive scorer class for scoring the
// kinetic energy of a particle entering a volume.
// ********************************************************************
#include "G4PSIncidentKineticEnergy.hh"

#include "G4StepStatus.hh"
#include "G4VProcess.hh"
#include "G4UnitsTable.hh"

G4PSIncidentKineticEnergy::G4PSIncidentKineticEnergy(G4String name, G4int depth)
    :G4VPrimitiveScorer(name,depth),HCID(-1),EvtMap(0)
{
    SetUnit("MeV");
}

G4PSIncidentKineticEnergy::G4PSIncidentKineticEnergy(G4String name, const G4String& unit, G4int depth)
    :G4VPrimitiveScorer(name,depth),HCID(-1),EvtMap(0)
{
    SetUnit(unit);
}

G4PSIncidentKineticEnergy::~G4PSIncidentKineticEnergy()
{;}

G4bool G4PSIncidentKineticEnergy::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    // Check if this is the primary particle
    if ( aStep->GetTrack()->GetParentID() != 0) {
        // It's a secondary particles, check if it came from decay
       if( aStep->GetTrack()->GetCreatorProcess()->GetProcessName() != "RadioactiveDecay") return FALSE;
    }

	if (aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary ){
        // Add the kinetic energy to the event map
        G4int index = GetIndex(aStep);
        G4double kineticEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();
        EvtMap->add(index, kineticEnergy);
        return TRUE;
    } else {return FALSE;}
}

void G4PSIncidentKineticEnergy::Initialize(G4HCofThisEvent* HCE)
{
    EvtMap = new G4THitsMap<G4double>(detector->GetName(), GetName());
    if ( HCID < 0 ) HCID = GetCollectionID(0);
    HCE->AddHitsCollection(HCID, (G4VHitsCollection*)EvtMap);
}

void G4PSIncidentKineticEnergy::EndOfEvent(G4HCofThisEvent*)
{;}

void G4PSIncidentKineticEnergy::clear(){
    EvtMap->clear();
}

void G4PSIncidentKineticEnergy::DrawAll()
{;}

void G4PSIncidentKineticEnergy::PrintAll()
{
    G4cout << " MultiFunctionalDet " << detector->GetName() << G4endl;
    G4cout << " PrimitiveScorer " << GetName() << G4endl;
    G4cout << " Number of entries " << EvtMap->entries() << G4endl;
    std::map<G4int,G4double*>::iterator itr = EvtMap->GetMap()->begin();
    for(; itr != EvtMap->GetMap()->end(); itr++) {
        G4cout << "  copy no.: " << itr->first
	      << " Kinetic Energy: " << *(itr->second)/GetUnitValue()
	      << " ["<<GetUnit()<<"]"
	      << G4endl;
    }
}


void G4PSIncidentKineticEnergy::SetUnit(const G4String& unit)
{
    CheckAndSetUnit(unit,"Energy");
}

