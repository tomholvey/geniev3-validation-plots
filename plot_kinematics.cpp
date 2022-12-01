/**
 * Plot kinematic distributions from GENIE events.
 *
 * For GENIEv3/v2 validation comparisons.
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/19
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TMath.h"
#include "TStyle.h"
#include "NuisTree.h"
#include "distributions.h"
#include "filter.h"

int main(int argc, char* argv[]) {
  double NDFHC_IntFlux = 0.0010489263;
  double NDRHC_IntFlux =  0.00094423594;
  //double NucleonTonneScale = 6.02831;
  double NucleonTonneScale = 5.98;
  
  // Parse command-line arguments
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " "
              << "OUTPUT.root INPUT.root" << std::endl;
    return 0;
  }

  gStyle->SetOptStat(0);
  gStyle->SetHistLineColor(kBlack);

  std::string outfile = argv[1];
  std::string infile = argv[2];

  // Get tree from input file
  TFile *fin = new TFile(infile.c_str(),"READ");
  TTree *intree = (TTree*)fin->Get("FlatTree_VARS");
  NuisTree nuistr(intree);

  // Define event filters
  filters::NuMode* filt_num_ccqe = new filters::NuMode(14, enums::kCC, enums::kQE);
  //filters::NuMode* filt_nue_ccqe = new filters::NuMode(12, enums::kCC, enums::kQE);
  //filters::NuMode* filt_num_ccmec = new filters::NuMode(14,  enums::kCC,enums::kMEC);
  //filters::NuMode* filt_num_ccres = new filters::NuMode(14,  enums::kCC,enums::kRes);

  //filters::NuMode* filt_num_nc = new filters::NuMode(14, enums::kNC, enums::kUndefined);
  //filters::NuMode* filt_nue_nc = new filters::NuMode(12, enums::kNC, enums::kUndefined);

  // Define distributions (plots)
  std::vector<Distribution*> dists = {
    // numCCQE
	new distributions::Enu_true("num_ccqe_enu", filt_num_ccqe),
	new distributions::q0("num_ccqe", filt_num_ccqe),
	new distributions::q3("num_ccqe", filt_num_ccqe),
	new distributions::TheoristsNu("num_ccqe", filt_num_ccqe),
	new distributions::Q2("num_ccqe", filt_num_ccqe),
	new distributions::TheoristsW("num_ccqe", filt_num_ccqe),
	new distributions::ExperimentalistsW("num_ccqe", filt_num_ccqe),
	new distributions::TheoristsBjorkenX("num_ccqe", filt_num_ccqe),
 
	new distributions::ExperimentalistsBjorkenX("num_ccqe", filt_num_ccqe),
	new distributions::TheoristsInelasticityY("num_ccqe", filt_num_ccqe),
	new distributions::ExperimentalistsInelasticityY("num_ccqe", filt_num_ccqe),
	new distributions::RecoInelasticityY("num_ccqe", filt_num_ccqe),
	new distributions::PLep("num_ccqe", filt_num_ccqe),
	new distributions::ThetaLep("num_ccqe", filt_num_ccqe),
	new distributions::ErecRelBias("num_ccqe", filt_num_ccqe),
	new distributions::ErecAbsBias("num_ccqe", filt_num_ccqe),
	new distributions::Erec("num_ccqe", filt_num_ccqe),
	new distributions::Emiss("num_ccqe", filt_num_ccqe),
	new distributions::Pmiss("num_ccqe", filt_num_ccqe),
	new distributions::PPLead("num_ccqe", filt_num_ccqe),
	new distributions::PLead_KE("num_ccqe", filt_num_ccqe),
	new distributions::ThetaPLead("num_ccqe", filt_num_ccqe),
	new distributions::PNLead("num_ccqe", filt_num_ccqe),
	new distributions::NLead_KE("num_ccqe", filt_num_ccqe),
	new distributions::ThetaNLead("num_ccqe", filt_num_ccqe),
	new distributions::PPiLead("num_ccqe", filt_num_ccqe),
	new distributions::ThetaPiLead("num_ccqe", filt_num_ccqe),
	
    new distributions::Mult("num_ccqe_multp", filt_num_ccqe, 2212),
    new distributions::Mult("num_ccqe_multp_30MeV", filt_num_ccqe, 2212, 0.03),
    new distributions::Mult("num_ccqe_multn", filt_num_ccqe, 2112),
    new distributions::Mult("num_ccqe_multpip", filt_num_ccqe, 211),
    new distributions::Mult("num_ccqe_multpim", filt_num_ccqe, -211),
    new distributions::Mult("num_ccqe_multpi0", filt_num_ccqe, 111),
    new distributions::Mult("num_ccqe_multkp", filt_num_ccqe, 321),
    new distributions::Mult("num_ccqe_multkm", filt_num_ccqe, -321),
    new distributions::Mult("num_ccqe_multk0", filt_num_ccqe, 311),
	new distributions::tki_dpt("num_ccqe", filt_num_ccqe),
	new distributions::tki_dat("num_ccqe", filt_num_ccqe),
	new distributions::tki_dphit("num_ccqe", filt_num_ccqe),
	new distributions::pmuT("num_ccqe", filt_num_ccqe),
	new distributions::pmuL("num_ccqe", filt_num_ccqe),
	new distributions::ErecYrec("num_ccqe", filt_num_ccqe),
	new distributions::EmissPmiss("num_ccqe", filt_num_ccqe),
	new distributions::Q0Q3("num_ccqe", filt_num_ccqe),
	new distributions::PThetaLep("num_ccqe", filt_num_ccqe),
	new distributions::PLep_TL("num_ccqe", filt_num_ccqe),
	new distributions::Q2W("num_ccqe", filt_num_ccqe),
	new distributions::BjorkenX_InelY("num_ccqe", filt_num_ccqe),
	new distributions::Enu_Erec("num_ccqe", filt_num_ccqe)
 
  };

  // Event loop
  for (int ievent=0; ievent<nuistr.GetEntries(); ievent++) {
    if (ievent % 10000 == 0) {
      std::cout << "EVENT " << ievent << std::endl;
    }
    nuistr.GetEntry(ievent);

    for (Distribution* dist : dists) {
      if ((*dist->filter)(nuistr)) {
        dist->Fill(nuistr);
      }
    }
  } // end event loop

  // Save histograms (to file and png)
  TFile* fout = new TFile(outfile.c_str(), "recreate");
  for (Distribution* dist : dists) {
    if (dist->hist->GetEntries() > 0) {
	  if (nuistr.PDGnu == 12 || nuistr.PDGnu == 14){
		  // Scale by DUNE numu FHC ND flux to get interactions per nucleon per POT
		  dist->hist->Scale(NDFHC_IntFlux * 1E-4);
		  // Scale by Ar atoms * 1 year POT * 1E-38
		  dist->hist->Scale(NucleonTonneScale * 1.1E12);
	  }
	  else if (nuistr.PDGnu == -12 || nuistr.PDGnu == -14){
		  // Scale by DUNE numubar RHC ND flux to get interactions per nucleon per POT
		  dist->hist->Scale(NDRHC_IntFlux * 1E-4 );
		  // Scale by Ar atoms * 1 year POT * 1E-38
		  dist->hist->Scale(NucleonTonneScale * 1.1E12);
	  }
	  dist->Write();
      // dist->Save();
    }
  }
  fout->Close();

  return 0;
}
