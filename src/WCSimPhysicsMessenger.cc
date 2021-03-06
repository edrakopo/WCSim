#include "WCSimPhysicsMessenger.hh"
#include "WCSimPhysicsList.hh"

#include "G4UIdirectory.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4UIcmdWithAString.hh"

WCSimPhysicsMessenger::WCSimPhysicsMessenger(WCSimPhysicsList* WCSimPhys)
  :WCSimPhysics(WCSimPhys)
{
  
  WCSimDir = new G4UIdirectory("/WCSim/physics/secondaries/");
  WCSimDir->SetGuidance("Commands to change secondary interaction model for protons");

  hadmodelCmd = new G4UIcmdWithAString("/WCSim/physics/secondaries/model",this);
  hadmodelCmd->SetGuidance("Available options: BERTINI BINARY");
  hadmodelCmd->SetGuidance("Description:");
  hadmodelCmd->SetGuidance("BERTINI = Bertini cascade model (Geant4.10 default)");
  hadmodelCmd->SetGuidance("BINARY  = Binary cascade model (2KM default)");
  hadmodelCmd->SetParameterName("secondaries", true, false);
  hadmodelCmd->SetDefaultValue("BERTINI");
  hadmodelCmd->SetCandidates("BERTINI BINARY");

}

WCSimPhysicsMessenger::~WCSimPhysicsMessenger()
{
  delete hadmodelCmd;
  delete WCSimDir;
}

void WCSimPhysicsMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == hadmodelCmd)
    WCSimPhysics->SetSecondaryHad(newValue);

}
