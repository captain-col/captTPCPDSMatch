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
	std::cout << "    -O useROOT  Use old ROOT summary file."<< std::endl;
	std::cout << "    -O useASCII  Use ASCII files."<< std::endl;
    }
    
    virtual bool SetOption(std::string option,std::string value="") {
	if (option == "useROOT") {
	    fROOT = true;
	    fNewROOT = false;
	}
	else if (option == "useASCII") {
	    fNewROOT = false;
	    fROOT = false;
	}
		
	return true;
    }

    void Initialize() {

	nHits = new TH1F("nHits","",100,0,100);
	
	if (!fROOT && !fNewROOT) {
	    CaptLog("Using ASCII files.");
	    /*	    for(int i=0; i<78;++i){
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
	    */

	    std::string name = "PDS_tAll2.txt";
	    std::ifstream myfile (name);
	    std::string line;
	    if (myfile.is_open())
	      {
		while ( getline (myfile,line) )
		  {
		    // int eventN;
		    // int sec;
		    // int nano;
		    // int rf1;
		    // int rf2;
		    // int rf3;
		    // long int digitRF1;
		    // long int digitRF2;
		    // long int digitRF3;
		    // double tprompt;
		    // double tpromptToRF;
		    // double tof;
		    // double energy;
		    // double trigt;
		    // int nhits;
		    // int beamtrig;
		    // double deltat;

		    int Row;
		    double Time;
		    int nEvt;
		    int iEvt;
		    double deltaT;

		    if(line[0]=='#')continue;
		    
		    //std::stringstream(line) >> eventN >> sec >> nano >> rf1 >> rf2 >> rf3 >> digitRF1 >> digitRF2 >> digitRF3 >> tprompt>> tpromptToRF >> tof >> energy >> trigt >> nhits >> beamtrig >> deltat;
		    std::stringstream(line) >> Row >> Time >> deltaT >> nEvt >> iEvt;	    

		    //std::cout<<line<<std::endl;
		    // if(tof < 0) continue;
		    // fEventN.push_back(eventN);
		    fPmtSec.push_back(Time);
		    // fPmtNano.push_back(nano);
		    // fTof.push_back(tof);
		    // fTfromRF.push_back(tprompt);
		    // fEnergy.push_back(energy);
		    // fTriggerType.push_back(trigt);
		    // fNHits.push_back(nhits);
		    // fBeamTrig.push_back(beamtrig);
		    fDeltaT.push_back(deltaT);
		  }    
		myfile.close();
	      }
	    
	}
	
    }

    void BeginFile(CP::TVInputFile *const input) {

	fTimeDiff = new TH1F("TimeDiff","TimeDiff",1000000,0,1000000);

	if (fROOT) {
	    CaptLog("Using ROOT file.");
	    TChain *c = new TChain("summaryTree");
	    c->Add("lowAna-pmtChain-fix5.root");
	    TChain *c2 = new TChain("pmtTree");
	    c2->Add("lowAna-pmtChain-fix5.root");
	    // TTree *summaryTree = c->GetTree();
	    // summaryTree->Print();

	    TBranch *pmtSummary  = (TBranch*) c->GetBranch("pmtSummary");
	    TBranchElement *eventPDS    = (TBranchElement*) pmtSummary->FindBranch("ventry");
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
	    TBranchElement *rf1         = (TBranchElement*) pmtSummary->FindBranch("vrf1");
	    TBranchElement *rf2         = (TBranchElement*) pmtSummary->FindBranch("vrf2");
	    TBranchElement *rf3         = (TBranchElement*) pmtSummary->FindBranch("vrf3");



	    TBranch *pmtEvent  = (TBranch*) c2->GetBranch("pmtEvent");
	    TBranchElement *qsum    = (TBranchElement*) pmtEvent->FindBranch("qsum");
	    TBranchElement *qmax    = (TBranchElement*) pmtEvent->FindBranch("qmax");
	    TBranchElement *dtime   = (TBranchElement*) pmtEvent->FindBranch("dtime");
	
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
	    std::vector<double> vqsum;
	    std::vector<double> vqmax;
	 
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
		rf1 -> GetEntry(iev);
		rf2 -> GetEntry(iev);
		rf3 -> GetEntry(iev);

		for (int in=0; in<5000; in++) {
		    // THIS IS A HACK BECAUSE WE CAN'T USE TTREEREADER
		    if (ke->GetValue(in,5000,true) == 0) {
			continue;
		    }
		    if (rf1->GetValue(in,5000,true) == 0 && rf2->GetValue(in,5000,true) == 0 && rf3->GetValue(in,5000,true) == 0) {
		    	continue;
		    }
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



	    
	    int nEntries2 = qsum->GetEntries();
	    for (int iev = 0; iev < nEntries2; iev++) {
		qsum -> GetEntry(iev);
		qmax -> GetEntry(iev);
		dtime -> GetEntry(iev);

		// Sum over all channels
		double qsumsum = 0.;
		double qmaxsum = 0.;
		for (int in=0; in<21; in++) {
		    qsumsum += qsum->GetValue(in,21,true);
		    qmaxsum += qmax->GetValue(in,21,true);
		}
		vqsum.push_back(qsumsum);
		vqmax.push_back(qmaxsum);
	    }

	    //std::cout<<"SIZES="<<vqsum.size()<<" "<<vcompSec.size()<<std::endl;
	    
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
		    fQsum.push_back(vqsum[i]);		
		    fQmax.push_back(vqmax[i]);
		}
	    }	
	}

	else if (fNewROOT) {

	    TChain *pds_chain = new TChain("digitizer1");
	    pds_chain->Add("PDS_all_LowInten_real_final.root");

	    long int timeSec = 0;
	    long int timeNan = 0;
	    long int digitizer_time = 0;
	    long int bclock = 0;
	    int event_number_global = 0;
	    std::vector<int> *RFtime = 0;
	    std::vector<int> *peakTime = 0;
	    std::vector<double> *neutronE = 0;
	    std::vector<double> *hitQ = 0;

	    std::vector<int> *coincNumber = 0;
	    
	    pds_chain->SetBranchAddress("computer_secIntoEpoch",&timeSec);
	    pds_chain->SetBranchAddress("computer_nsIntoSec",&timeNan);
	    pds_chain->SetBranchAddress("digitizer_time",&digitizer_time);
	    pds_chain->SetBranchAddress("RF_timeStart",&RFtime);
	    pds_chain->SetBranchAddress("CoincPeakTime",&peakTime);
	    pds_chain->SetBranchAddress("NeutronEnergy",&neutronE);
	    pds_chain->SetBranchAddress("bclock",&bclock);
	    pds_chain->SetBranchAddress("event_number_global",&event_number_global);
	    pds_chain->SetBranchAddress("CoincCharge",&hitQ);
	    pds_chain->SetBranchAddress("CoincNumber",&coincNumber);
	    
	    int nEntries = pds_chain->GetEntries();
	    double clight = 0.299792458;
	    double GAMMAPEAK=-628.089;
	    double L=23.2;
	    //double nmass=939.565;

	    pds_chain->GetEntry(0);
	    long int prev_digitizer_time = digitizer_time;
	    for (int iev = 0; iev < nEntries; iev++) {
		pds_chain->GetEntry(iev);
		
		//fEventN.push_back(0);
		fTriggerType.push_back(0);
		fNHits.push_back(0);
		fBeamTrig.push_back(0);

		int RF_time = 0;		
		if (RFtime->size()>0) 
		    RF_time = RFtime->at(0);
	    
		if (abs(prev_digitizer_time - digitizer_time ) > 2e7) {
		    prev_digitizer_time = digitizer_time;
		}

		for (int ipk = 0; ipk < int(peakTime->size()); ipk++) {
		    if (RF_time == -1) continue;
		    double tof = (peakTime->at(ipk) - RF_time)*4.0-GAMMAPEAK+L/clight;
		    fPmtSec.push_back(int(bclock*1.0e-9));
		    fPmtNano.push_back((bclock%1000000000));
		    fTfromRF.push_back((peakTime->at(ipk) - RF_time)*4.0);
		    fTof.push_back(tof);
		    fEnergy.push_back(neutronE->at(ipk));
		    fDeltaT.push_back(8.0*(digitizer_time - prev_digitizer_time) - 4.0*(peakTime->at(ipk) - RF_time) );
		    fDigiT.push_back(digitizer_time);	    
		    fEventN.push_back(event_number_global);
		    fHitCharge.push_back(hitQ->at(ipk));
		    //std::cout<<"CO="<<coincNumber->at(ipk)<<std::endl;
		    fCoincNumber.push_back(coincNumber->at(ipk));

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

	//long int evTimeS = event.GetTimeStamp();
	static long int evTimeS = 0;
	if(!event.FindDatum("TimeForMatching")){
		    event.AddDatum(new CP::TRealDatum("TimeForMatching",evTimeS));
	}

	std::vector<long int> timePMT ;
	
	for(u_int i=0;i<fPmtSec.size();++i){
	    long int t = (long int)fPmtSec[i]*1000000000+(long int)fPmtNano[i];
	    timePMT.push_back(t);
	}

	int count = 0;

	for(std::size_t i=0;i<timePMT.size();++i){
	    long int diff = fabs(timePMT[i]-evTimeS);
	    double matchDiff = fabs((double)diff/1000000);
    
	    //fTimeDiff->Fill(matchDiff);
	    
	    if(matchDiff<100){
		// std::cout<<fEventN[i]<<" "<<matchDiff<<std::endl;
		// std::cout<<matchDiff<<std::endl;
		// long int digTime = fDigiT[i];
		// std::cout<<"TIME="<<timePMT[i]<<" "<<evTimeS<<" "<<fDeltaT[i]<<" "<< digTime<<std::endl;
		int ns=timePMT[i] % 1000000000;
		int se=timePMT[i] / 1000000000;
		std::string name  = "PDSEvent_"+toString(count);
		CP::THandle<CP::TDataVector> pmtData = event.Get<CP::TDataVector>("pmtData"); 
		CP::TEventContext pmtEv(22,22,22,22,ns,se);
		std::unique_ptr<CP::TEvent> eventPMT(new CP::TEvent(pmtEv));

		if(!eventPMT->FindDatum("event")){
		    eventPMT->AddDatum(new CP::TRealDatum("eventNumber",fEventN[i]));
		}
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
		if(!eventPMT->FindDatum("HitCharge")){
		    eventPMT->AddDatum(new CP::TRealDatum("HitCharge",fHitCharge[i]));
		}
		if(!eventPMT->FindDatum("CoincNumber")){
		    eventPMT->AddDatum(new CP::TRealDatum("CoincNumber",fCoincNumber[i]));
		}
		// if(!eventPMT->FindDatum("qSum")){
		//     eventPMT->AddDatum(new CP::TRealDatum("qSum",fQsum[i]));
		// }
		// if(!eventPMT->FindDatum("qMax")){
		//     eventPMT->AddDatum(new CP::TRealDatum("qMax",fQmax[i]));
		// }
		pmtData->AddDatum(eventPMT.release(),name.c_str());
		count++;
	    }
	    
	}
	
	if (count > 100) count = 99;
	nHits->Fill(count);
        evTimeS = event.GetTimeStamp();
	
	return true;
    }

    void Finalize(CP::TRootOutput * const output) {
	nHits->Draw();
	gPad->Print("nHits.C");
    }

private:

    bool fROOT = false;
    bool fNewROOT = true;
    
    std::vector<int> fEventN;
    std::vector<double> fPmtSec;
    std::vector<int> fPmtNano;
    std::vector<double> fTof;
    std::vector<double> fTfromRF;
    std::vector<double> fEnergy;
    std::vector<int> fTriggerType;
    std::vector<int>   fNHits;
    std::vector<int>  fBeamTrig;
    std::vector<int>  fCoincNumber;
    std::vector<double> fDeltaT;
    std::vector<double> fHitCharge;
    std::vector<double> fDigiT;
    std::vector<double> fQsum;
    std::vector<double> fQmax;
    TH1F* fTimeDiff;
    TH1F* nHits;
    
};

int main(int argc, char **argv) {
    TMySimpleEventLoop userCode;    
    CP::eventLoop(argc,argv,userCode);
}
