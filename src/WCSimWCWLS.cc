#include "WCSimWCWLS.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCHit.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "WCSimDetectorConstruction.hh"
#include "WCSimPmtInfo.hh"
#include "WCSimWLSObject.hh"


#include <vector>
// for memset
#include <cstring>

#ifndef HYPER_VERBOSITY
// #define HYPER_VERBOSITY
#endif

extern "C" void skrn1pe_(float* );
//extern "C" void rn1pe_(float* ); // 1Kton

WCSimWCWLS::WCSimWCWLS(G4String name,
                       WCSimDetectorConstruction* myDetector,
                       G4String detectorElement)
  :G4VDigitizerModule(name), detectorElement(detectorElement)
{
  // G4String colName = "WCRawPMTSignalCollection";
  // collectionName.push_back(colName);

//  if(detectorElement=="tank") collectionName.push_back("WCRawPMTSignalCollection");
//  else if(detectorElement=="OD") collectionName.push_back("WCRawPMTSignalCollection_OD");
  f(detectorElement=="OD") collectionName.push_back("WCRawWLSSignalCollection_OD");
  else G4cout << "detectorElement undefined..." << G4endl;
  this->myDetector = myDetector;
  DigiHitMapWLS.clear();

  #ifdef HYPER_VERBOSITY
  if(detectorElement=="OD") G4cout<<"WCSimWCWLS::WCSimWCWLS ☆ recording collection name "<<collectionName[0]<<G4endl;
  #endif

}

WCSimWCWLS::~WCSimWCWLS(){
 
}

G4double WCSimWCWLS::rn1pe(){
  // G4String WCIDCollectionName = myDetector->GetIDCollectionName();

  WCSimWLSObject * WLS;
  G4String WCCollectionName;
//  if(detectorElement=="tank") PMT = myDetector->GetPMTPointer(myDetector->GetIDCollectionName());
   if(detectorElement=="OD") WLS = myDetector->GetWLSPointer(myDetector->GetODCollectionName2());

  // PMT = myDetector->GetPMTPointer(WCIDCollectionName);

  G4int i;
  G4double random = G4UniformRand();
  G4double random2 = G4UniformRand(); 
  G4float *qpe0;
  qpe0 = PMT->Getqpe();
  for(i = 0; i < 501; i++){
    
    if (random <= *(qpe0+i)) break;
  }
  if(i==500)
    random = G4UniformRand();
  
  return (G4double(i-50) + random2)/22.83;
  
}


void WCSimWCWLS::Digitize()
{
  // DigitsCollection = new WCSimWCDigitsCollection ("WCDigitizedCollectionPMT",collectionName[0]);
  // G4String WCIDCollectionName = myDetector->GetIDCollectionName();

  // Create a DigitCollection and retrieve the appropriate hitCollection ID based on detectorElement
  G4String WCCollectionName;
  G4String DigitsCollectionName;
/*  if(detectorElement=="tank"){
    DigitsCollectionName="WCDigitizedCollection";
    WCCollectionName = myDetector->GetIDCollectionName();
  }*/if(detectorElement=="OD"){
    DigitsCollectionName="WCDigitizedCollection_ODWLS";
    WCCollectionName = myDetector->GetODCollectionName2();
  }

  DigitsCollection = new WCSimWCDigitsCollection (DigitsCollectionName,collectionName[0]);

  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
 
  // Get the Associated Hit collection IDs
  // G4int WCHCID = DigiMan->GetHitsCollectionID(WCIDCollectionName);
  //G4int WCHCID = DigiMan->GetHitsCollectionID(WCCollectionName);

  // The Hits collection
//  WCSimWCHitsCollection* WCHC =
//    (WCSimWCHitsCollection*)(DigiMan->GetHitsCollection(WCHCID));

  #ifdef HYPER_VERBOSITY
  if(detectorElement=="OD"){
    G4cout<<"WCSimWCWLS::Digitize ☆ Making digits collection (WCSimWCDigitsCollection*)"<<DigitsCollectionName<<" for "<<detectorElement
          <<" and calling MakePeCorrection on "<<WCCollectionName<<" to fill it."<<G4endl;
  }
  #endif

  /*if (WCHC) {

    MakePeCorrection(WCHC);
    
  }
*/
  #ifdef HYPER_VERBOSITY
  if(detectorElement=="OD"){
    G4cout<<"WCSimWCWLS::Digitize ☆ Storing "<<DigitsCollectionName<<" for "<<detectorElement
          <<", which has "<<DigitsCollection->entries()<<" entries"<<G4endl;
  }
  #endif

  StoreDigiCollection(DigitsCollection);

}


void WCSimWCWLS::MakePeCorrection(WCSimWCHitsCollection* WCHC)
{ 

  //Get the PMT info for hit time smearing
  // G4String WCIDCollectionName = myDetector->GetIDCollectionName();
  // WCSimPMTObject * PMT = myDetector->GetPMTPointer(WCIDCollectionName);

  G4String WCCollectionName;
//  if(detectorElement=="tank") WCCollectionName = myDetector->GetIDCollectionName();
   if(detectorElement=="OD") WCCollectionName = myDetector->GetODCollectionName2();

  WCSimWLSObject * WLS = myDetector->GetWLSPointer(WCCollectionName);

  #ifdef HYPER_VERBOSITY
  if(detectorElement=="OD"){
    G4cout<<"WCSimWCWLS::MakePeCorrection ☆ making PE correction for ";
  }
    if(WCHC){
      G4cout<<WCHC->entries();
    } else {
      G4cout<<"0";
    }
  G4cout<<" entries"<<G4endl;
  #endif

  for (G4int i=0; i < WCHC->entries(); i++)
    {

      //G4double peCutOff = .3;
      // MF, based on S.Mine's suggestion : global scaling factor applied to
      // all the smeared charges.
      // means that we need to increase the collected light by
      // (efficiency-1)*100% to
      // match K2K 1KT data  : maybe due to PMT curvature ?

      //G4double efficiency = 0.985; // with skrn1pe (AP tuning) & 30% QE increase in stacking action

      // Get the information from the hit
      G4int   tube         = (*WCHC)[i]->GetTubeID();
      G4double peSmeared = 0.0;
      double time_PMT, time_true;

	  for (G4int ip =0; ip < (*WCHC)[i]->GetTotalPe(); ip++){
	    time_true = (*WCHC)[i]->GetTime(ip);
	    time_PMT  = time_true; //currently no PMT time smearing applied
	    peSmeared = rn1pe();
	    int parent_id = (*WCHC)[i]->GetParentID(ip);

	    if ( DigiHitMapWLS[tube] == 0) {
	      WCSimWCDigi* Digi = new WCSimWCDigi();
	      Digi->SetLogicalVolume((*WCHC)[0]->GetLogicalVolume());
	      Digi->AddPe(time_PMT);
	      Digi->SetTubeID(tube);
	      Digi->SetPe(ip,peSmeared);
	      Digi->SetTime(ip,time_PMT);
	      Digi->SetPreSmearTime(ip,time_true);
	      Digi->SetParentID(ip,parent_id);
	      DigiHitMapWLS[tube] = DigitsCollection->insert(Digi);
	    }	
	    else {
	      (*DigitsCollection)[DigiHitMapWLS[tube]-1]->AddPe(time_PMT);
	      (*DigitsCollection)[DigiHitMapWLS[tube]-1]->SetLogicalVolume((*WCHC)[0]->GetLogicalVolume());
	      (*DigitsCollection)[DigiHitMapWLS[tube]-1]->SetTubeID(tube);
	      (*DigitsCollection)[DigiHitMapWLS[tube]-1]->SetPe(ip,peSmeared);
	      (*DigitsCollection)[DigiHitMapWLS[tube]-1]->SetTime(ip,time_PMT);
	      (*DigitsCollection)[DigiHitMapWLS[tube]-1]->SetPreSmearTime(ip,time_true);
	      (*DigitsCollection)[DigiHitMapWLS[tube]-1]->SetParentID(ip,parent_id);
	    }
      
	  } // Loop over hits in each PMT
    }// Loop over PMTs
}


