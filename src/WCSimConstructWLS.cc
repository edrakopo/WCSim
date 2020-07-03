#include "WCSimDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

#include "G4SDManager.hh"
#include "WCSimWCSD.hh"
#include "WCSimWLSObject.hh"
#include "WCSimPMTObject.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

//PMT logical volume construction.

WCSimDetectorConstruction::WLSMap_t WCSimDetectorConstruction::WLSLogicalVolumes;

//*******************************************/
// Construct WLS plate
//*******************************************/
G4LogicalVolume* WCSimDetectorConstruction::ConstructWLSPlate(G4String WLSName, G4String CollectionName, G4String CollectionName2, G4String detectorElement){

  WLSKey_t key2(WLSName,CollectionName2);
  WLSMap_t::iterator it = WLSLogicalVolumes.find(key2);
  if (it != WLSLogicalVolumes.end()) {
     //G4cout << "Restore PMT" << G4endl;
      return it->second;
  }
  G4double expose;
  G4double radius;
  G4String Water = "Water";
  G4String WLS_Material = "Water";
  if(isWLSFilled) WLS_Material = "WLS_PVT";
  G4String WLSCladding_Material = "Water";
  if(isWLSFilled) WLSCladding_Material = "Tyvek";

  //get the PMT pointer     
  WCSimPMTObject *PMT = GetPMTPointer(CollectionName);
  expose = PMT->GetExposeHeight();
  radius = PMT->GetRadius();
  G4double sphereRadius = (expose*expose+ radius*radius)/(2*expose);

  WCSimWLSObject *WLS = GetWLSPointer(CollectionName2);
  // CLADDING
  G4double CladdingWidth= 1.*mm;
  // EVERYTHING WILL BE ORIENTATED ALONG Z-AXIS
  
  ////////////////////////////////////////////////
  // Box structure to hold the WLS and PMT object
  G4Box *container =
      new G4Box("rectangleWLS",
                (WCODWLSPlatesLength+2*CladdingWidth)/2,
                (WCODWLSPlatesLength+2*CladdingWidth)/2,
                sphereRadius/2);
  G4LogicalVolume* logicContainer =
      new G4LogicalVolume(container,
                          G4Material::GetMaterial(Water),
                          "WCODContainer",
                          0,0,0);
  G4VisAttributes* visContainer
      = new G4VisAttributes(G4Colour((0.0, 1.0, 0.0)));
  visContainer->SetForceWireframe(true);
  logicContainer->SetVisAttributes(G4VisAttributes::Invisible);
  //// Uncomment following for WLS visualization
  logicContainer->SetVisAttributes(visContainer);

  ////////////////////////////////////////////////
  //Create a WLS plate towards x,y plane and drilled hole will be around z-axis
  // WLS
    G4Box *WLSPlateAndCladding =
      new G4Box("WLSPlateAndCladding",
                (WCODWLSPlatesLength+2*CladdingWidth)/2,
                (WCODWLSPlatesLength+2*CladdingWidth)/2,
                WCODWLSPlatesThickness/2);
  G4Box *WLSPlate =
      new G4Box("WLSPlate",
                WCODWLSPlatesLength/2,
                WCODWLSPlatesLength/2,
                WCODWLSPlatesThickness/2);
  // Extruded volume for WLS
  G4Tubs* WLSHole =
      new G4Tubs("WLSHole",0,WCPMTODRadius,WCODWLSPlatesLength/2,0,twopi);
  G4SubtractionSolid* extrudedWLS =
      new G4SubtractionSolid("extrudedWLS", WLSPlate, WLSHole, NULL, G4ThreeVector(0,0,0));
  // Extruded volume for cladding
  G4SubtractionSolid* WLSCladding =
      new G4SubtractionSolid("WLSCladding", WLSPlateAndCladding, WLSPlate, NULL, G4ThreeVector(0,0,0));
  logicWCODWLSPlate =
      new G4LogicalVolume(extrudedWLS,
                          G4Material::GetMaterial(WLS_Material),
                          "WCODWLSPlate",
                          0,0,0);
  G4VisAttributes* visWLS
      = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0));
  visWLS->SetForceSolid(true);
  logicWCODWLSPlate->SetVisAttributes(G4VisAttributes::Invisible);
  //// Uncomment following for WLS visualization
  logicWCODWLSPlate->SetVisAttributes(visWLS);
  logicWCODWLSPlateCladding =
      new G4LogicalVolume(WLSCladding,
                          G4Material::GetMaterial(WLSCladding_Material),
                          "WCODWLSPlateCladding",
                          0,0,0);
  G4VisAttributes* visWLSCladding
      = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  visWLSCladding->SetForceSolid(true);

  logicWCODWLSPlateCladding->SetVisAttributes(G4VisAttributes::Invisible);
  // Uncomment following for WLS visualization
  logicWCODWLSPlateCladding->SetVisAttributes(visWLSCladding);
  ////////////////////////////////////////////////
  // PMTs
  //G4LogicalVolume* logicWCPMT = ConstructPMT(PMTName,CollectionName,detectorElement);
  
  ////////////////////////////////////////////////
  // Ali G. : Do dat placement inda box
  G4VPhysicalVolume* physiWLS =
      new G4PVPlacement(0,
                        G4ThreeVector(0, 0, -(sphereRadius-WCODWLSPlatesThickness)/2),
                        logicWCODWLSPlate,
                      //"WCCellWLSPlateOD",
                      CollectionName, 
                        logicContainer,
                        false,
                        0,
                        checkOverlaps);
  G4VPhysicalVolume* physiWLSCladding =
      new G4PVPlacement(0,
                        G4ThreeVector(0, 0, -(sphereRadius-WCODWLSPlatesThickness)/2),
                        logicWCODWLSPlateCladding,
                        //"WCCellWLSPlateODCladding",
                        CollectionName, 
		        logicContainer,
                        false,
                        0,
                        checkOverlaps);
  new G4LogicalSkinSurface("cladding_surf",   logicWCODWLSPlateCladding,   OpCladdingSurface);

/*    G4VPhysicalVolume* physiPMT =
      new G4PVPlacement(0,
                        G4ThreeVector(0, 0, -(sphereRadius)/2),
                        logicWCPMT,
                        //"WCPMTOD",
                        CollectionName, 
                        logicContainer,
                        false,
                        0,
                        checkOverlaps);
*/                   //                                                                                                                         // Instantiate a new sensitive detector 
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String SDName = "/WCSim/";
  SDName += CollectionName2;
  // If there is no such sensitive detector with that SDName yet,
  // make a new one
  if( ! SDman->FindSensitiveDetector(SDName, false) ) {
    
    aWCWLS = new WCSimWCSD(CollectionName,SDName,this,detectorElement);
    SDman->AddNewDetector( aWCWLS );
  }
   logicWCODWLSPlate->SetSensitiveDetector( aWCWLS );

   WLSLogicalVolumes[key2] = logicContainer;

  return logicContainer;

}

