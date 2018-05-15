#include <memory>

// Include to get Event Loop.
#include <eventLoop.hxx>
#include <TReconTrack.hxx>
#include <TReconHit.hxx>
#include <TDataVector.hxx>
#include <TRealDatum.hxx>
#include <TManager.hxx>
#include <TVInputFile.hxx>
#include <TRootInput.hxx>


// Includes for ROOT classes

#include <HEPUnits.hxx>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TChain.h>
#include <TPad.h>
#include <TBranchElement.h>


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

    void Usage(void) {
	std::cout << "    -O useROOT  Use ROOT summary file instead of ASCII files"<< std::endl;
    }
    
    virtual bool SetOption(std::string option,std::string value="") {
	if (option == "useROOT")
	    fROOT = true;
		
	return true;
    }

    void Initialize() {

	if (!fROOT) {
	    CaptLog("Using ASCII files.");
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
				fEventN.push_back(eventN);
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
	}
	
    }

    void BeginFile(CP::TVInputFile *const input) {

	fTimeDiff = new TH1F("TimeDiff","TimeDiff",1000000,0,1000000);

	if (fROOT) {
	    CaptLog("Using ROOT file.");
	    TChain *c = new TChain("summaryTree");
	    c->Add("lowAna-pmtChain-fix4-0-0.root");
	    // TTree *summaryTree = c->GetTree();
	    // summaryTree->Print();

	    TBranch *pmtSummary  = (TBranch*) c->GetBranch("pmtSummary");
	    TBranchElement *eventPDS    = (TBranchElement*) pmtSummary->FindBranch("vevent");
	    TBranchElement *compSec     = (TBranchElement*) pmtSummary->FindBranch("vcompSec");
	    TBranchElement *compNano    = (TBranchElement*) pmtSummary->FindBranch("vcompNano");
	    TBranchElement *tof         = (TBranchElement*) pmtSummary->FindBranch("tof");
	    TBranchElement *tprompt     = (TBranchElement*) pmtSummary->FindBranch("tprompt");
	    TBranchElement *tfromRF     = (TBranchElement*) pmtSummary->FindBranch("timeToRf");
	    TBranchElement *ke          = (TBranchElement*) pmtSummary->FindBranch("ke");
	    TBranchElement *trig        = (TBranchElement*) pmtSummary->FindBranch("vtrig");
	    TBranchElement *nhits       = (TBranchElement*) pmtSummary->FindBranch("nhits");
	    TBranchElement *beamtrig    = (TBranchElement*) pmtSummary->FindBranch("beamtrig");
	    TBranchElement *deltaT      = (TBranchElement*) pmtSummary->FindBranch("deltaT");

	
	    int nEntries = compSec->GetEntries();

	    std::vector<int> vevent;
	    std::vector<int> vcompSec;
	    std::vector<Long64_t> vcompNano;
	    std::vector<double> vtof;
	    std::vector<double> vtprompt;
	    std::vector<double> vtfromRF;
	    std::vector<double> vt;
	    std::vector<double> vke;
	    std::vector<int> vtrig;
	    std::vector<int>   vnhits;
	    std::vector<int>  vbeamtrig;
	    std::vector<double> vdeltaT;
	 
	    for (int iev = 0; iev < nEntries; iev++) {
		eventPDS -> GetEntry(iev);
		compSec->GetEntry(iev);
		compNano->GetEntry(iev);
		tof -> GetEntry(iev);
		tprompt -> GetEntry(iev);
		tfromRF -> GetEntry(iev);
		ke -> GetEntry(iev);     
		trig -> GetEntry(iev);   
		nhits -> GetEntry(iev);  
		beamtrig -> GetEntry(iev);
		deltaT   -> GetEntry(iev);

	    
		for (int in=0; in<5000; in++) {
		    //std::cout<<in<<" "<<compNano->GetValue(in,5000,true)<<std::endl;
		    vcompSec.push_back(compSec->GetValue(in,5000,true));
		    vcompNano.push_back(compNano->GetValue(in,5000,true));
		    vevent.push_back(eventPDS->GetValue(in,5000,true));
		    vtof.push_back(tof->GetValue(in,5000,true));
		    vtprompt.push_back(tprompt->GetValue(in,5000,true));
		    vtfromRF.push_back(tfromRF->GetValue(in,5000,true));
		    vke.push_back(ke->GetValue(in,5000,true));
		    vtrig.push_back(trig->GetValue(in,5000,true));
		    vnhits.push_back(nhits->GetValue(in,5000,true));
		    vbeamtrig.push_back(beamtrig->GetValue(in,5000,true));
		    vdeltaT.push_back(deltaT->GetValue(in,5000,true));
		
		}
	    }
    	

	    CP::TRootInput *infile = new CP::TRootInput(input->GetFilename());

	
	    std::unique_ptr<CP::TEvent> event2(infile->FirstEvent());
	    std::cout<<event2->GetContext()<<std::endl;
	    long int evTimeS2 = event2->GetTimeStamp();
	    std::unique_ptr<CP::TEvent> event3(infile->CP::TRootInput::LastEvent());
	    std::cout<<event3->GetContext()<<std::endl;
	    long int evTimeS3 = event3->GetTimeStamp();

	    for (u_int i=0; i < vcompSec.size(); i++) {
		if ( vcompSec[i] + 10 >  evTimeS2/1000000000 && vcompSec[i] - 10 <  evTimeS3/1000000000 ) {
		    // std::cout<<"event="<<evTimeS2<< " " << vcompSec[i]/1 << std::endl;
		    // std::cout<<"event="<<evTimeS2/1000000000<< " " << vcompSec[i]/1 << std::endl;		
		    // std::cout<<"event="<<evTimeS2/1000000000 - vcompSec[i]/1 << std::endl;
		    fPmtSec.push_back(vcompSec[i]);
		    fPmtNano.push_back(vcompNano[i]);
		    fEventN.push_back(vevent[i]);
		    fTof.push_back(vtof[i]);
		    fTfromRF.push_back(vtfromRF[i]);
		    fEnergy.push_back(vke[i]);
		    fTriggerType.push_back(vtrig[i]);
		    fNHits.push_back(vnhits[i]);
		    fBeamTrig.push_back(vbeamtrig[i]);
		    fDeltaT.push_back(vdeltaT[i]);		
		}
	    }	
	}   
    }

    
    bool operator () (CP::TEvent& event) {
	// Get the list of hits from the event.  The handle is essentially
	// a pointer to the hit selection.
    
	// std::cout<<event.GetContext()<<std::endl;
	

	if(!event.FindDatum("pmtData"))
	    event.AddDatum(new CP::TDataVector("pmtData","Data from PDS system"));

	long int evTimeS = event.GetTimeStamp();
	//int evTimeN= event.GetContext().GetNanoseconds();
  
	std::vector<long int> timePMT ;

	for(u_int i=0;i<fPmtSec.size();++i){
	    long int t = (long int)fPmtSec[i]*1000000000+(long int)fPmtNano[i];
	    timePMT.push_back(t);
	}

	int count=0;
	//std::cout<<"t1="<<evTimeS<<std::endl;
	//evTimeS = evTimeS - 200000000;
	//std::cout<<"t2="<<evTimeS<<std::endl;
	
	for(std::size_t i=0;i<timePMT.size();++i){
	    long int diff = fabs(timePMT[i]-evTimeS);
	    double matchDiff = fabs((double)diff/1000000);
      
	    //fTimeDiff->Fill(matchDiff);
	    
	    if(matchDiff<100){
		// std::cout<<fEventN[i]<<" "<<matchDiff<<std::endl;
		// std::cout<<matchDiff<<std::endl;
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
		    eventPMT->AddDatum(new CP::TRealDatum("TimeFromFirstRF_ns",fTfromRF[i]));
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
	// fTimeDiff->Draw();
	// gPad->Print("TimeDiff.C");
    }

private:

    bool fROOT = false;
    
    std::vector<int> fEventN;
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
