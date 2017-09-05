## Welcome to G4SiDetector
G4SiDetector is an application based on the Geant4 Monte Carlo toolkit for simulating the response (i.e. geometric factor, efficiency, energy migration matrix, etc) of a Canberra PD450-15-500AM Passivated Implanted Planar Silicon (PIPS) detector. 

## Build Notes
To build this application, you should have a working version of CMake
and Geant 4.10.3.p01 or higher.

Geant4: https://geant4.web.cern.ch/geant4/ CMake: https://cmake.org/

I recommend setting up your directory structure as follows:

- $G4WORKDIR/G4SiDetector : This is the source directory and is a clone of the code from this GitHub 
- $G4WORKDIR/G4SiDetector-build : Contains directories and files for each build. This directory also contains the executable files and example macros.

The simulation works both in sequential and in multi-threaded (MT) modes of Geant4. However, a significant speedup can be achieved by running the simulation in MT mode.

## Steps to compile:
### Step 1 - Source the Geant4 environment setup script

    source /opt/Geant4/geant4.10.03.p01-install/bin/geant4.sh

### Step 2 - Create the build directory and navigate to it
    
    mkdir G4SiDetector-build && cd $_

### Step 3 - Setup CMake, make the build, and run the build

    sudo cmake -DGeant4_DIR=/opt/Geant4/geant4.10.03.p01-install/lib/Geant4-10.3.1/ ~/G4WORK/G4SiDetector; sudo make -j8; ./G4SiDetector


To recompile, I typically just re-run the command in Step 3

## Scoring Physical Quantities
Scoring of physical quantities is carried out through the G4MultiFunctionalDetector which allows multiple primitive scorers to be
assigned to a single volume. In this simulation, a G4MultiFunctionalDetector is assigned to the sensitive volume of Silicon chip. The value recorded by the primitive scorers is collected on event-by-event basis enabling calculation of the detector response.

The primitive scorer(s) registered for the sensitive volume of Si chip are:
* G4PSEnergyDeposit

During a run, the data collected on an event-by-event basis is histogrammed into four logarithmically binned ROOT histograms. 

* The first histogram, labeled "SourceTrueEnergy", records the kinetic energy of the incident source particle. 
* The second histogram, labeled "DetectorTrueEnergy", records the kinetic energy of incident particles with non-zero energy deposition in the detector. 
* The third histogram, labeled "DetectorMeasuredEnergy", records the measured or deposited energy in the detector.
* The fourth histogram, labeled "EnergyMigrationMatrix", records a 2D histogram of the true kinetic energy versus measured/deposited energy in the detector.

The first and second histograms, combined with the known source radius, can be used to calculate the geometric factor for the detector. The fourth histogram can be used by unfolding algorithms to convert the measured spectrum into a true spectrum and vice-versa.