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
// $Id$
//
/// \file EDPrimaryGeneratorAction.hh
/// \brief Definition of the EDPrimaryGeneratorAction class

#ifndef EDPrimaryGeneratorAction_h
#define EDPrimaryGeneratorAction_h 1

#include <cmath> // for M_PI
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4MTRunManager.hh"
#include "G4VSensitiveDetector.hh"
#include "Randomize.hh"
#include "EDAnalysis.hh"
#include "EDRunAction.hh"
//#include "G4MTRandGauss.hh"
//#include <Random.h>
//#include "RandGauss.h"
//#include "TRandom.h"
//#include "TMath.h"
//#include "RandGauss.h"
#include "EDRunAction.hh"
#include "ELPHEBeam.hh"

class G4Event;
class G4ParticleGun;
class G4GenericMessenger;

/// The first primary generator action class.

class EDPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    EDPrimaryGeneratorAction();    
    virtual ~EDPrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);         

  private:
    G4GenericMessenger*  fMessenger;
    G4ParticleGun*  fParticleGun;  
    G4bool          fRandom;
  
  void DefineCommands();
};


#endif
