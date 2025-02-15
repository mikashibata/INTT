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
/// \file EDPrimaryGeneratorAction.cc
/// \brief Implementation of the EDPrimaryGeneratorAction class

#include "EDPrimaryGeneratorAction.hh"

EDPrimaryGeneratorAction::EDPrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction(),
    fMessenger(0),
    fParticleGun(0),
    fRandom(true)
{   
  G4int nofParticles = 1;
  fParticleGun  = new G4ParticleGun(nofParticles);

  DefineCommands();
  // Define particle properties
  G4String particleName = "e+";
  
  // Default particle kinematics
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
  fParticleGun->SetParticleDefinition(particle);
  
}

EDPrimaryGeneratorAction::~EDPrimaryGeneratorAction()
{}

void EDPrimaryGeneratorAction::DefineCommands()
{
  fMessenger
    = new G4GenericMessenger(this, "/INTT/beam/", "Commands for beam in this application");
    
  //////////////////////////////////////////////////////////////////////////////
  // Beam energy
  // G4GenericMessenger::Command& setBeamEnergy
  //   = fMessenger->DeclarePropertyWithUnit( "beamEnergy", "GeV", beam_energy );
  // setBeamEnergy.SetGuidance( "Beam energy in the units of GeV" );
  // setBeamEnergy.SetParameterName( "BeamEnergy", false ); // (name, is_omittable)
  // setBeamEnergy.SetDefaultValue( 1.0 );
  
  return;
}

void EDPrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  //std::cerr << " beam";
  auto UImanager = G4UImanager::GetUIpointer();

  //this function is called at the begining of ecah event

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4double energy_spectrum = EDRunAction::beam->GetMomentum();
  G4double theta =  0.0 * deg;
  G4double phi =  0.0 * deg;

  // if no beam smearing required, just generate the beam here
  if( EDRunAction::is_beam_smearing == false )
    {
      //      std::cerr << "Primary generator action, mono-energy mode, " << energy_spectrum << " "
      //<< EDRunAction::is_beam_smearing ;
      fParticleGun->SetParticleMomentumDirection( G4ThreeVector(0, 0, 1) );
      fParticleGun->SetParticlePosition( G4ThreeVector(0, 0, -1.0 * m) );
      fParticleGun->SetParticleEnergy( energy_spectrum );

      //fParticleGun->GeneratePrimaryVertex(event);
    }
  else
    {
      energy_spectrum = CLHEP::RandGauss::shoot( EDRunAction::beam->GetMomentum(), EDRunAction::beam->GetMomentumSpread() );
      //G4double energy_spectrum = 
    
      // randomized direction
      //G4double dtheta = 2. * deg; // better not to introduce parameters for smearing without knowing what value should be used
      G4double dtheta = 0.0 * deg;
      //G4double dphi = 360 * deg;
      G4double dphi = 0.0 * deg;
    
      // G4double theta = G4UniformRand() * dtheta;
      // G4double phi = G4UniformRand() * dphi;
      //G4double theta = (1.62- THETAANGLE  * 0.00025) * deg;
      //    G4double thetagaus = CLHEP::RandGauss::shoot(0,0.002);
      G4double thetagaus =  0.0 ;
      //G4double phigaus = G4MTRandGauss::shoot(90,0.002);
      //G4double phigaus = G4UniformRand() * 360;
      G4double phigaus = 0.0 ; 

      //G4double X_position = CLHEP::RandGauss::shoot(0,0.0559) * mm;
      G4double X_position = 0.0 * mm;
      //G4double Y_position = CLHEP::RandGauss::shoot(0,0.1182) * mm;
      G4double Y_position = 0.0 * mm;

      // std::cerr << std::endl;
      // std::cerr << std::string(100, '=' ) << std::endl;
      // std::cerr << "Beam smearing: " << EDRunAction::is_beam_smearing << std::endl;
      // std::cerr << "Beam energy: " << energy_spectrum << std::endl;
      // std::cerr << "Beam particle: " << fParticleGun->GetParticleDefinition()->GetParticleName() << std::endl;
      // std::cerr << std::string(100, '=' ) << std::endl;
      // std::cerr << std::endl;
      
      theta =  thetagaus * deg;
      phi =  phigaus * deg;
      //G4double theta = (126 * 0.01) * deg;
      //G4double phi = 90. * deg;
    
      // fParticleGun->SetParticleMomentumDirection(
      //   G4ThreeVector(0, aaa[eID],ccc[eID]));
      // //fParticleGun->SetParticlePosition(G4ThreeVector(0,0,-20167. * mm));
      // fParticleGun->SetParticlePosition(G4ThreeVector(0 * mm,0 * mm,-1 * bbb[eID] * mm));

      fParticleGun->SetParticleMomentumDirection( G4ThreeVector(0, 0, 1) );
      fParticleGun->SetParticlePosition( G4ThreeVector(0, 0, -1.0 * m) );
    
    }


  G4int fNtupleId = 1;
  G4int eID = 0;
  eID = event->GetEventID();
  analysisManager->FillNtupleDColumn(fNtupleId, 0, 0.0);
  analysisManager->FillNtupleDColumn(fNtupleId, 1, 0.0);
  analysisManager->FillNtupleDColumn(fNtupleId, 2, 0.0);
  analysisManager->FillNtupleDColumn(fNtupleId, 3, theta / M_PI * 180.);
  //analysisManager->FillNtupleDColumn(fNtupleId, 3, thetagaus);
  analysisManager->FillNtupleDColumn(fNtupleId, 4, phi / M_PI * 180.);
  analysisManager->FillNtupleIColumn(fNtupleId, 5, eID);
  analysisManager->FillNtupleDColumn(fNtupleId, 6, energy_spectrum);
  analysisManager->AddNtupleRow(fNtupleId);
  
  fParticleGun->GeneratePrimaryVertex(event);
  return;
}
