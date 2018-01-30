#include <memory>

// Include to get Event Loop.
#include <eventLoop.hxx>
#include <TReconTrack.hxx>
#include <TReconHit.hxx>
#include <TDataVector.hxx>
#include <TRealDatum.hxx>


// Includes for ROOT classes

#include <HEPUnits.hxx>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <TH1F.h>
#include <TPad.h>


std::string toString(int i)
{
  std::ostringstream s;
  s << i;
  return s.str();
}

class TMySimpleEventLoop: public CP::TEventLoopFunction {
public:
    TMySimpleEventLoop() {}
    virtual ~TMySimpleEventLoop() {}
    void Initialize() {


      for(int i=0; i<78;++i){
	std::stringstream ss;
	ss << i;
	std::string str = ss.str();
	std::string name = str+".txt";
      std::ifstream myfile (name);
      std::string line;
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      int eventN;
      int sec;
      int nano;
      int rf1;
      int rf2;
      int rf3;
      long int digitRF1;
      long int digitRF2;
      long int digitRF3;
      double tprompt;
      double tpromptToRF;
      double tof;
      double energy;
      double trigt;
      int nhits;
      int beamtrig;
      double deltat;
      if(line[0]=='#')continue;
    
      std::stringstream(line) >> eventN >> sec >> nano >> rf1 >> rf2 >> rf3 >> digitRF1 >> digitRF2 >> digitRF3 >> tprompt>> tpromptToRF >> tof >> energy >> trigt >> nhits >> beamtrig >> deltat;
      // if(tof < 0) continue;
      fPmtSec.push_back(sec);
      fPmtNano.push_back(nano);
      fTof.push_back(tof);
      fTfromRF.push_back(tprompt);
      fEnergy.push_back(energy);
      fTriggerType.push_back(trigt);
      fNHits.push_back(nhits);
      fBeamTrig.push_back(beamtrig);
      fDeltaT.push_back(deltat);
}    myfile.close();
}
      }

      fTimeDiff = new TH1F("TimeDiff","TimeDiff",1000000,0,1000000);
   
    }
  bool operator () (CP::TEvent& event) {
    // Get the list of hits from the event.  The handle is essentially
    // a pointer to the hit selection.
    
    
    std::cout<<event.GetContext()<<std::endl;

    if(!event.FindDatum("pmtData"))
      event.AddDatum(new CP::TDataVector("pmtData","Data from PDS system"));

   long int evTimeS = event.GetTimeStamp();
   int evTimeN= event.GetContext().GetNanoseconds();
  
   std::vector<long int> timePMT ;

     for(int i=0;i<fPmtSec.size();++i){
     long int t = (long int)fPmtSec[i]*1000000000+(long int)fPmtNano[i];
     timePMT.push_back(t);
   }

     int count=0;
     for(std::size_t i=0;i<timePMT.size();++i){
       long int diff = fabs(timePMT[i]-evTimeS);
       double matchDiff = fabs((double)diff/1000000);
      
       fTimeDiff->Fill(matchDiff);
       if(matchDiff<100){
	  std::cout<<matchDiff<<std::endl;
	 int ns=timePMT[i] % 1000000000;
	 int se=timePMT[i] / 1000000000;
	 std::string name  = "PDSEvent_"+toString(count);
	 CP::THandle<CP::TDataVector> pmtData = event.Get<CP::TDataVector>("pmtData"); 
	 CP::TEventContext pmtEv(22,22,22,22,ns,se);
	 std::unique_ptr<CP::TEvent> eventPMT(new CP::TEvent(pmtEv));
	 if(!eventPMT->FindDatum("TOF(ns)")){
	   eventPMT->AddDatum(new CP::TRealDatum("TOF_ns",fTof[i]));
	 }
	 if(!eventPMT->FindDatum("TimeFromFirsRF_ns")){
	   eventPMT->AddDatum(new CP::TRealDatum("TimeFromFirsRF_ns",fTfromRF[i]));
	 }
	 if(!eventPMT->FindDatum("Energy(MeV)")){
	   eventPMT->AddDatum(new CP::TRealDatum("Energy_MeV",fEnergy[i]));
	 }
	 if(!eventPMT->FindDatum("TriggerType")){
	   eventPMT->AddDatum(new CP::TRealDatum("TriggerType",fTriggerType[i]));
	 }
	 if(!eventPMT->FindDatum("nHits")){
	   eventPMT->AddDatum(new CP::TRealDatum("nHits",fNHits[i]));
	 }
	 if(!eventPMT->FindDatum("BeamTrig")){
	   eventPMT->AddDatum(new CP::TRealDatum("BeamTrig",fBeamTrig[i]));
	 }
	 if(!eventPMT->FindDatum("DeltaT_ns")){
	   eventPMT->AddDatum(new CP::TRealDatum("DeltaT_ns",fDeltaT[i]));
	 }
	 pmtData->AddDatum(eventPMT.release(),name.c_str());
	 count++;
       }
       
     }
      return true;
      }

    void Finalize(CP::TRootOutput * const output) {
      fTimeDiff->Draw();
      // gPad->Print("TimeDiff.C");
    }

private:
  
  std::vector<int> fPmtSec;
  std::vector<int> fPmtNano;
  std::vector<double> fTof;
  std::vector<double> fTfromRF;
  std::vector<double> fEnergy;
  std::vector<int> fTriggerType;
  std::vector<int>   fNHits;
  std::vector<int>  fBeamTrig;
  std::vector<double> fDeltaT;
  TH1F* fTimeDiff;
    
};

int main(int argc, char **argv) {
    TMySimpleEventLoop userCode;
    CP::eventLoop(argc,argv,userCode);
}
