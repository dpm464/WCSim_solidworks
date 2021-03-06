#include "WCSimWCPMT.hh"
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
#include "WCSimPMTObject.hh"


#include <vector>
// for memset
#include <cstring>


extern "C" void skrn1pe_(float* );
//extern "C" void rn1pe_(float* ); // 1Kton

WCSimWCPMT::WCSimWCPMT(G4String name,
				   WCSimDetectorConstruction* myDetector)
  :G4VDigitizerModule(name)
{
  G4String colName = "WCRawPMTSignalCollection";
  this->myDetector = myDetector;
  collectionName.push_back(colName);
  DigiHitMapPMT.clear();
  

}

WCSimWCPMT::~WCSimWCPMT(){
 
}

G4double WCSimWCPMT::rn1pe(){
  G4String WCIDCollectionName = myDetector->GetIDCollectionName();
  WCSimPMTObject * PMT;
  PMT = myDetector->GetPMTPointer(WCIDCollectionName);
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


void WCSimWCPMT::Digitize()
{
  DigitsCollection = new WCSimWCDigitsCollection ("WCDigitizedCollectionPMT",collectionName[0]);
  G4String WCIDCollectionName = myDetector->GetIDCollectionName();
  G4DigiManager* DigiMan = G4DigiManager::GetDMpointer();
 
  // Get the Associated Hit collection IDs
  G4int WCHCID = DigiMan->GetHitsCollectionID(WCIDCollectionName);

  // The Hits collection
  WCSimWCHitsCollection* WCHC =
    (WCSimWCHitsCollection*)(DigiMan->GetHitsCollection(WCHCID));

  if (WCHC) {

    MakePeCorrection(WCHC);
    
  }

  StoreDigiCollection(DigitsCollection);

}


void WCSimWCPMT::MakePeCorrection(WCSimWCHitsCollection* WCHC)
{ 

  // Get the info for pmt positions
  std::vector<WCSimPmtInfo*> *pmts = myDetector->Get_Pmts();
  // It works out that the pmts here are ordered !
  // pmts->at(i) has tubeid i+1

  //Get the PMT info for hit time smearing
  G4String WCIDCollectionName = myDetector->GetIDCollectionName();
  WCSimPMTObject * PMT = myDetector->GetPMTPointer(WCIDCollectionName);

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
      G4int   iHR         = (*WCHC)[i]->GetIsHitReflector();
      G4double peSmeared = 0.0;
      double time_PMT, time_true;
      G4int  track_id      = (*WCHC)[i]->GetTrackID();
      
      
      // Set the position and rotation of the pmt (from WCSimWCAddDarkNoise.cc)
      Float_t hit_pos[3];
      Float_t hit_rot[3];
      
      WCSimPmtInfo* pmtinfo = (WCSimPmtInfo*)pmts->at( tube -1 );
      hit_pos[0] = 10*pmtinfo->Get_transx()/CLHEP::cm;
      hit_pos[1] = 10*pmtinfo->Get_transy()/CLHEP::cm;
      hit_pos[2] = 10*pmtinfo->Get_transz()/CLHEP::cm;
      hit_rot[0] = pmtinfo->Get_orienx();
      hit_rot[1] = pmtinfo->Get_orieny();
      hit_rot[2] = pmtinfo->Get_orienz();

      G4ThreeVector pmt_orientation(hit_rot[0], hit_rot[1], hit_rot[2]);
      G4ThreeVector pmt_position(hit_pos[0], hit_pos[1], hit_pos[2]);

	  for (G4int ip =0; ip < (*WCHC)[i]->GetTotalPe(); ip++){
	    time_true = (*WCHC)[i]->GetTime(ip);
	    time_PMT  = time_true; //currently no PMT time smearing applied
	    peSmeared = rn1pe();
	    int parent_id = (*WCHC)[i]->GetParentID(ip);
	    float photon_starttime = (*WCHC)[i]->GetPhotonStartTime(ip);
	    G4ThreeVector photon_startpos = (*WCHC)[i]->GetPhotonStartPos(ip);
	    G4ThreeVector photon_endpos = (*WCHC)[i]->GetPhotonEndPos(ip);
	    
	    if ( DigiHitMapPMT[tube] == 0) {			//me: this loop is managing if the hit already exists or not
	      WCSimWCDigi* Digi = new WCSimWCDigi();
	      Digi->SetLogicalVolume((*WCHC)[0]->GetLogicalVolume());
	      Digi->AddPe(time_PMT);
	      Digi->SetTubeID(tube);
	      Digi->SetIsHitReflector(iHR); //me:for logicreflector
	      Digi->SetPos(pmt_position);
	      Digi->SetOrientation(pmt_orientation);
	      Digi->SetPe(ip,peSmeared);
	      Digi->SetTime(ip,time_PMT);
	      Digi->SetTrackID(track_id);
	      Digi->SetPreSmearTime(ip,time_true);
	      Digi->SetParentID(ip,parent_id);
	      Digi->SetPhotonStartTime(ip,photon_starttime);
	      Digi->SetPhotonStartPos(ip,photon_startpos);
	      Digi->SetPhotonEndPos(ip,photon_endpos);
	     // Digi->SetReflectorPos(ip,reflectorpos);
	      DigiHitMapPMT[tube] = DigitsCollection->insert(Digi);
	    }	
	    else {
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->AddPe(time_PMT);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetLogicalVolume((*WCHC)[0]->GetLogicalVolume());
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPe(ip,peSmeared);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetTime(ip,time_PMT);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetTubeID(tube); 
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetIsHitReflector(iHR); //me:for logicreflector
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPos(pmt_position);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetOrientation(pmt_orientation);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetTrackID(track_id);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPreSmearTime(ip,time_true);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetParentID(ip,parent_id);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPhotonStartTime(ip,photon_starttime);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPhotonStartPos(ip,photon_startpos);
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetPhotonEndPos(ip,photon_endpos);
	     // (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetReflectorPos(ip,reflectorpos);
	    }
      
	   // Loop over hits in each PMT
	  for (G4int io =0; io < (*WCHC)[i]->GetNReflectorHit(); io++){
	     int reflectortrack_id = (*WCHC)[i]->GetRefTrackID(io);
	     //std::cout<<" "<<reflectortrack_id<<std::endl;
	    int reflector_id = (*WCHC)[i]->GetReflectorID(io);
	    G4ThreeVector reflectorpos = (*WCHC)[i]->GetReflectorPos(io);
	    if ( DigiHitMapPMT[tube] == 0) { //me: this loop is managing if the hit already exists or not
	      WCSimWCDigi* Digi = new WCSimWCDigi();
	      Digi->SetRefTrackID(io,reflectortrack_id);
	      Digi->SetReflectorID(io,reflector_id);
	      Digi->SetReflectorPos(io,reflectorpos);
	    }	
	    else {
	       (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetRefTrackID(io,reflectortrack_id); 
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetReflectorID(io,reflector_id); 
	      (*DigitsCollection)[DigiHitMapPMT[tube]-1]->SetReflectorPos(io,reflectorpos);
	    }
	}
    }// Loop over PMTs
}
}

