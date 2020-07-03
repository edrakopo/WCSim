#ifndef WCSimWCWLS_h
#define WCSimWCWLS_h 1

#include "WCSimDarkRateMessenger.hh"
#include "WCSimDetectorConstruction.hh"
#include "G4VDigitizerModule.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>


class WCSimWCWLS : public G4VDigitizerModule
{
public:

  WCSimWCWLS(G4String name, WCSimDetectorConstruction*, G4String detectorElement);
  ~WCSimWCWLS();
  
   void ReInitialize() { DigiHitMapWLS.clear(); TriggerTimes.clear(); }
    
   
public:
  
  void AddPMTDarkRate(WCSimWCDigitsCollection*);
  void MakePeCorrection(WCSimWCHitsCollection*);
  void Digitize();
  G4double GetTriggerTime(int i) { return TriggerTimes[i];}
  // void SetConversion(double iconvrate){ ConvRate = iconvrate; }
  //  static G4double GetLongTime() { return LongTime;}
  
  G4double rn1pe();
  G4double peSmeared;
  // double ConvRate; // kHz
  std::vector<G4double> TriggerTimes;
  std::map<int,int> DigiHitMapWLS; // need to check if a hit already exists..

  WCSimWCDigitsCollection*  DigitsCollection;  
  WCSimDetectorConstruction* myDetector;
  G4String detectorElement;

};

#endif








