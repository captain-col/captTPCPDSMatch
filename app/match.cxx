#include <memory>

// Include to get Event Loop.
#include <eventLoop.hxx>
#include <TReconTrack.hxx>
#include <TReconHit.hxx>

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
      int tprompt;
      double tof;
      if(line[0]=='#')continue;
    
      std::stringstream(line) >> eventN >> sec >> nano >> rf1 >> rf2 >> rf3 >> digitRF1 >> digitRF2 >> digitRF3 >> tprompt >> tof ;
      if(tof < 0) continue;
      fPmtSec.push_back(sec);
      fPmtNano.push_back(nano);
}    myfile.close();
}
      }

      std::cout<<"pmtSize="<<fPmtSec.size()<<std::endl;

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
   evTimeS += (long int)evTimeN;

   std::vector<long int> timePMT ;

     for(int i=0;i<fPmtSec.size();++i){
     long int t = (long int)fPmtSec[i]*1000000000+(long int)fPmtNano[i];
     timePMT.push_back(t);
   }

     int count=0;
     for(std::size_t i=0;i<timePMT.size();++i){
       long int diff = abs(timePMT[i]-evTimeS);
       double matchDiff=abs((double)diff/1000000);
      
       fTimeDiff->Fill(matchDiff);
       if(matchDiff<100){
	  std::cout<<matchDiff<<std::endl;
	 int ns=timePMT[i] % 1000000000;
	 int se=timePMT[i] / 1000000000;
	 std::string name  = "PDSEvent_"+toString(count);
	 CP::THandle<CP::TDataVector> pmtData = event.Get<CP::TDataVector>("pmtData"); 
	 CP::TEventContext pmtEv(22,22,22,22,ns,se);
	 std::unique_ptr<CP::TEvent> eventPMT(new CP::TEvent(pmtEv));
	 pmtData->AddDatum(eventPMT.release(),name.c_str());
	 count++;
       }
       
     }
     std::cout<<count<<std::endl;
      return true;
      }
    // Called at least once.  If multiple file are open, it will be called
    // for each one.   Notice there are two forms...
    void Finalize(CP::TRootOutput * const output) {
      fTimeDiff->Draw();
      // gPad->Print("TimeDiff.C");
    }

private:
  
  std::vector<int> fPmtSec;
  std::vector<int> fPmtNano;
  TH1F* fTimeDiff;
    
};

int main(int argc, char **argv) {
    TMySimpleEventLoop userCode;
    CP::eventLoop(argc,argv,userCode);
}
