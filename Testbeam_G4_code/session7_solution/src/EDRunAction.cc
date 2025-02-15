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
/// \file EDRunAction.cc
/// \brief Implementation of the EDRunAction class

#include "EDRunAction.hh"
#include "EDAnalysis.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

G4double EDRunAction::beam_energy;
G4bool EDRunAction::is_beam_smearing;
ELPHEBeam* EDRunAction::beam;

EDRunAction::EDRunAction( EDPrimaryGeneratorAction* pga ) :
  G4UserRunAction()
{
  DefineCommands();
  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in B4Analysis.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(0);
  analysisManager->SetNtupleMerging(true);
  G4cout << "Using " << analysisManager->GetType() 
         << " analysis manager" << G4endl;

  // Creating histograms per layer
  // Creating histograms for summary data
  // analysisManager->CreateH1("AllCharged", "Charged Edep [MeV] in all layers" , 
  //                           150, 0., 1500);

  // Creating ntuples
  //
  // ntuple id = 0

  //============================NEW one, only store the ID====================================================        
  analysisManager->CreateNtuple("Chamber1", "Chamber 1 hits");
  analysisManager->CreateNtupleIColumn("Event_ID");
  analysisManager->CreateNtupleIColumn("UpandDown");   // column id = 0 
  analysisManager->CreateNtupleIColumn("Xpos");    // column id = 1 
  analysisManager->CreateNtupleIColumn("Ypos");    // column id = 2
  analysisManager->CreateNtupleIColumn("Zpos");    // column id = 3 
  analysisManager->CreateNtupleIColumn("silicon_type");    // column id = 3   
  analysisManager->CreateNtupleDColumn("Edep");    // column id = 3 
  analysisManager->FinishNtuple();
  //============================NEW one, only store the ID====================================================

  //ntuple id =1 
  analysisManager->CreateNtuple("Beam_angle", "Beam_angle");
  analysisManager->CreateNtupleDColumn("Beam_X"); // colume id = 0
  analysisManager->CreateNtupleDColumn("Beam_Y"); // colume id = 0
  analysisManager->CreateNtupleDColumn("Beam_Z"); // colume id = 0 
  analysisManager->CreateNtupleDColumn("Beam_Theta"); // colume id = 0
  analysisManager->CreateNtupleDColumn("Beam_Phi"); // colume id = 1
  analysisManager->CreateNtupleIColumn("Event_ID"); // colume id = 1
  analysisManager->CreateNtupleDColumn("Beam_energy"); // in MeV
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("event_particle", "event_particle");
  analysisManager->CreateNtupleIColumn("PID_order"); // colume id = 0
  analysisManager->CreateNtupleIColumn("PID"); // colume id = 0
  analysisManager->CreateNtupleDColumn("PID_energy");
  analysisManager->CreateNtupleDColumn("particle_X"); // colume id = 0
  analysisManager->CreateNtupleDColumn("particle_Y");
  analysisManager->CreateNtupleDColumn("particle_Z");
  analysisManager->CreateNtupleIColumn("volume_type");
  analysisManager->CreateNtupleIColumn("Event_ID"); // colume id = 1
  analysisManager->FinishNtuple();

  // ntuple id = 1

  analysisManager->CreateNtuple("sci_trigger", "sci_trigger");
  analysisManager->CreateNtupleIColumn("Event_ID");
  analysisManager->CreateNtupleIColumn("sci_ID");
  analysisManager->CreateNtupleDColumn("sci_edep"); // colume id = 0
  analysisManager->FinishNtuple();    

}

EDRunAction::~EDRunAction()
{
  delete G4AnalysisManager::Instance();  
}

void EDRunAction::DefineCommands()
{
  fMessenger
    = new G4GenericMessenger(this, "/INTT/beam/", "Commands for beam in this application");
  //////////////////////////////////////////////////////////////////////////////
  // Switch for the momentum spread
  G4GenericMessenger::Command& setBeamSmearing
    = fMessenger->DeclareProperty( "beamSmearing", is_beam_smearing );
  setBeamSmearing.SetGuidance( "Switch to the realistic beam(true) or mono-energy beam at x=0 & y=0(false)." );
  setBeamSmearing.SetParameterName( "beamSmearing", false ); // (name, is_omittable)
  setBeamSmearing.SetDefaultValue( "false" );

  //////////////////////////////////////////////////////////////////////////////
  // Switch for the beam line
  G4GenericMessenger::Command& setBeamLine
    = fMessenger->DeclareProperty( "beamLine", beam_line );
  setBeamLine.SetGuidance( "Selection of the beam line. -23 and -30 are available." );
  setBeamLine.SetParameterName( "beamLine", true ); // (name, is_omittable)
  setBeamLine.SetDefaultValue( "-23" );

  //////////////////////////////////////////////////////////////////////////////
  // Switch for the production target
  G4GenericMessenger::Command& setProductionTarget
    = fMessenger->DeclareProperty( "productionTarget", production_target );
  setProductionTarget.SetGuidance( "Selection of th production target. Au_20um, W_200um, and Cu_8mm are available." );
  setProductionTarget.SetParameterName( "productionTarget", true ); // (name, is_omittable)
  setProductionTarget.SetDefaultValue( "Au_20um" );

  //////////////////////////////////////////////////////////////////////////////
  // Switch for the x_p limitation (restriction of horizontal position of the beam)
  G4GenericMessenger::Command& setPositionRestriction
    = fMessenger->DeclareProperty( "positionRestriction", position_restriction );
  setPositionRestriction.SetGuidance( "Selection of position restrictioin of the beam. 0(no), 1(weak), 2(moderate), and 3(strong) are available." );
  setPositionRestriction.SetParameterName( "positionRestriction", true ); // (name, is_omittable)
  setPositionRestriction.SetDefaultValue( "0" );



}

void EDRunAction::BeginOfRunAction(const G4Run* kRun )
{

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();  
  auto UImanager = G4UImanager::GetUIpointer();
  
  if( this->is_first )
    {
      beam_energy =  UImanager->GetCurrentDoubleValue( "/gun/energy" ) * GeV; // GeV is used whatever I give? changed to MeV
      is_first = false;
    }

  // define output name
  G4String particle = UImanager->GetCurrentStringValue( "/gun/particle" );
  std::stringstream ss;
  ss << beam_energy / GeV;
  G4String energy   = ss.str() + "GeV";
  //  G4String event_num = UImanager->GetCurrentStringValue( "/gun/beamOn") + "events";

  // if energy is less than 1, it should be in MeV
  if( beam_energy < 1 ){
    std::stringstream ss2;
    ss2 << beam_energy << "MeV";
    energy =  ss.str();
  }

  // Open an output file
  G4String fileName = "ED";
  analysisManager->OpenFile(fileName);  

  this->beam = new ELPHEBeam( beam_line, production_target, position_restriction, this->beam_energy );
  beam->Print( 0 );  
}

void EDRunAction::EndOfRunAction(const G4Run* kRun )
{


  //  std::cerr << "void EDRunAction::EndOfRunAction(const G4Run* kRun )" ;
  // save histograms 
  //
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
  
  //std::cerr << " -> ends" << std::endl;
}
