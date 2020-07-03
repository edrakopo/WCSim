#ifndef WCSimWLSObject_h
#define WCSimWLSObject_h 1

#include "WCSimDetectorConstruction.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>
#include <TH1F.h>
#include <TGraph.h>

class WCSimWLSObject
{

public:
  virtual G4String GetWLSName()=0;
  virtual G4double GetExposeHeight()=0;
  virtual G4double GetRadius()=0;
  virtual G4float* Getqpe()=0;
  virtual G4float* GetQE()=0;
  virtual G4float* GetQEWavelength()=0;
  virtual G4float  GetmaxQE()=0;
  virtual G4float  GetCollectionEfficiency(float);
  virtual float    HitTimeSmearing(float)=0;
  virtual G4double GetPMTGlassThickness()=0;
  virtual G4float  GetDarkRate()=0;
  virtual G4float  GetDarkRateConversionFactor()=0;
  virtual G4int    GetNbOfQEDefined()=0;
protected:
  virtual G4float* GetCollectionEfficiencyArray();
  virtual G4float* GetCollectionEfficiencyAngle();
  G4float Interpolate_func(G4float, G4int, G4float*, G4float*);
};


class basicWLS : public WCSimWLSObject
{

 public:

  basicWLS();
  ~basicWLS();

 public:
  G4String GetWLSName();
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();
};

class WCSimBasicWLSObject
{

 public:
  WCSimBasicWLSObject();
  WCSimBasicWLSObject(std::map<G4float,G4float>);
  WCSimBasicWLSObject(std::vector<G4float>,std::vector<G4float>,G4float);
  ~WCSimBasicWLSObject();

 private:
  std::vector<G4float> QE;
  std::vector<G4float> wavelength;
  std::map<G4float,G4float> mapQE;
  G4float  maxQE;
  TGraph   *gQE;

 public:
  std::vector<G4float> GetQE(){ return QE;};
  void SetQE(std::vector<G4float> qe){ QE=qe;};

  std::vector<G4float> GetWavelength(){ return wavelength;};
  void SetWavelength(std::vector<G4float> qe){ wavelength=qe;};

  std::map<G4float,G4float> GetMapQE(){ return mapQE;};
  void SetMapQE(std::map<G4float,G4float> qe){ mapQE=qe;};

  G4float GetmaxQE(){ return maxQE;};
  void SetmaxQE(G4float qe){ maxQE=qe;};

  TGraph* GetgQE(){ return gQE;};
  void SetgQE(TGraph *g){ gQE=g;};

  void DefineQEHist(std::map<G4float,G4float>);
};

#endif
