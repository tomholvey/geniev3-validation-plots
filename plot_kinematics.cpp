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
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Utilities/InputTag.h"
#include "nusimdata/SimulationBase/GTruth.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCNeutrino.h"
#include "distributions.h"
#include "filter.h"

int main(int argc, char* argv[]) {
  // Parse command-line arguments
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " "
              << "OUTPUT.root INPUT.root [INPUT2.root ...]" << std::endl;
    return 0;
  }

  gStyle->SetOptStat(0);
  gStyle->SetHistLineColor(kBlack);

  std::string outfile = argv[1];
  std::vector<std::string> filename;
  for (int i=2; i<argc; i++) { 
    std::cout << "FILE " << argv[i] << std::endl; 
    filename.push_back(argv[i]);
  }

  // Define event filters
  filters::NuMode* filt_num_ccqe = new filters::NuMode(14, simb::kQE, simb::kCC);
  filters::NuMode* filt_nue_ccqe = new filters::NuMode(12, simb::kQE, simb::kCC);
  filters::NuMode* filt_num_ccmec = new filters::NuMode(14, simb::kMEC, simb::kCC);
  filters::NuMode* filt_num_ccres = new filters::NuMode(14, simb::kRes, simb::kCC);

  // Define distributions (plots)
  std::vector<Distribution*> dists = {
    // numCCQE
    new distributions::Q2("num_ccqe_q2", filt_num_ccqe),
    new distributions::Q0Q3("num_ccqe_q0q3", filt_num_ccqe),
    new distributions::LeadPKEQ0("num_ccqe_pkeq0", filt_num_ccqe),
    new distributions::W("num_ccqe_w", filt_num_ccqe),
    new distributions::BjorkenX("num_ccqe_bjorkenx", filt_num_ccqe),
    new distributions::InelasticityY("num_ccqe_inely", filt_num_ccqe),
    new distributions::PLep("num_ccqe_plep", filt_num_ccqe),
    new distributions::ThetaLep("num_ccqe_tlep", filt_num_ccqe),
    new distributions::PThetaLep("num_ccqe_ptlep", filt_num_ccqe),
    new distributions::Mult("num_ccqe_multp", filt_num_ccqe, 2212),
    new distributions::Mult("num_ccqe_multp_30MeV", filt_num_ccqe, 2212, 0.03),
    new distributions::Mult("num_ccqe_multn", filt_num_ccqe, 2112),
    new distributions::Mult("num_ccqe_multpip", filt_num_ccqe, 211),
    new distributions::Mult("num_ccqe_multpim", filt_num_ccqe, -211),
    new distributions::Mult("num_ccqe_multpi0", filt_num_ccqe, 111),
    new distributions::Mult("num_ccqe_multkp", filt_num_ccqe, 321),
    new distributions::Mult("num_ccqe_multkm", filt_num_ccqe, -321),
    new distributions::Mult("num_ccqe_multk0", filt_num_ccqe, 311),

    // nueCCQE
    new distributions::Q2("nue_ccqe_q2", filt_nue_ccqe),
    new distributions::Q0Q3("nue_ccqe_q0q3", filt_nue_ccqe),
    new distributions::ECons("nue_ccqe_econs", filt_nue_ccqe),
    new distributions::LeadPKEQ0("nue_ccqe_pkeq0", filt_nue_ccqe),
    new distributions::W("nue_ccqe_w", filt_nue_ccqe),
    new distributions::BjorkenX("nue_ccqe_bjorkenx", filt_nue_ccqe),
    new distributions::InelasticityY("nue_ccqe_inely", filt_nue_ccqe),
    new distributions::PLep("nue_ccqe_plep", filt_nue_ccqe),
    new distributions::ThetaLep("nue_ccqe_tlep", filt_nue_ccqe),
    new distributions::PThetaLep("nue_ccqe_ptlep", filt_nue_ccqe),
    new distributions::Mult("nue_ccqe_multp", filt_nue_ccqe, 2212),
    new distributions::Mult("nue_ccqe_multp_30MeV", filt_nue_ccqe, 2212, 0.03),
    new distributions::Mult("nue_ccqe_multn", filt_nue_ccqe, 2112),
    new distributions::Mult("nue_ccqe_multpip", filt_nue_ccqe, 211),
    new distributions::Mult("nue_ccqe_multpim", filt_nue_ccqe, -211),
    new distributions::Mult("nue_ccqe_multpi0", filt_nue_ccqe, 111),
    new distributions::Mult("nue_ccqe_multkp", filt_nue_ccqe, 321),
    new distributions::Mult("nue_ccqe_multkm", filt_nue_ccqe, -321),
    new distributions::Mult("nue_ccqe_multk0", filt_nue_ccqe, 311),

    // numCCMEC
    new distributions::Q0Q3("num_ccmec_q0q3", filt_num_ccmec),
    new distributions::Pke("num_ccmec_ppp", filt_num_ccmec),

    // numCCRes
    new distributions::Q0Q3("num_ccres_q0q3", filt_num_ccres),
    new distributions::W("num_ccres_w", filt_num_ccres),
    new distributions::PLep("num_ccres_plep", filt_num_ccres),
    new distributions::ThetaLep("num_ccres_tlep", filt_num_ccres),
    new distributions::PThetaLep("num_ccres_ptlep", filt_num_ccres),
    new distributions::PPiLead("num_ccres_ppi", filt_num_ccres),
    new distributions::ThetaPiLead("num_ccres_tpi", filt_num_ccres),
    new distributions::ThetaLepPiLead("num_ccres_tlpi", filt_num_ccres),
    new distributions::Mult("nue_ccres_multp", filt_num_ccres, 2212),
    new distributions::Mult("nue_ccres_multn", filt_num_ccres, 2112),
    new distributions::Mult("nue_ccres_multpip", filt_num_ccres, 211),
    new distributions::Mult("nue_ccres_multpim", filt_num_ccres, -211),
    new distributions::Mult("nue_ccres_multpi0", filt_num_ccres, 111),
    new distributions::Mult("nue_ccres_multkp", filt_num_ccres, 321),
    new distributions::Mult("nue_ccres_multkm", filt_num_ccres, -321),
    new distributions::Mult("nue_ccres_multk0", filt_num_ccres, 311)
  };

  size_t nevents = 0;

  // Event loop
  for (gallery::Event ev(filename) ; !ev.atEnd(); ev.next()) {
    if (nevents % 100 == 0) {
      std::cout << "EVENT " << nevents << std::endl;
    }
    nevents++;

    gallery::Handle<std::vector<simb::MCTruth> > mctruths;
    ev.getByLabel({"generator"}, mctruths);

    art::FindMany<simb::GTruth> gtruths(mctruths, ev, "generator");

    for (size_t i=0, ntruth=mctruths->size(); i<ntruth; i++) {
      std::vector<const simb::GTruth*> vgtruths;
      gtruths.get(i, vgtruths);
      assert(vgtruths.size() == 1);

      const simb::MCTruth& mctruth = mctruths->at(i);
      const simb::GTruth& gtruth = *(vgtruths[0]);

      for (Distribution* dist : dists) {
        if ((*dist->filter)(mctruth)) {
          dist->Fill(mctruth, gtruth);
        }
      }
    }
  }

  // Save histograms (to file and PDF)
  TFile* fout = new TFile(outfile.c_str(), "recreate");
  for (Distribution* dist : dists) {
    if (dist->hist->GetEntries() > 0) {
      dist->Write();
      dist->Save();
    }
  }
  fout->Close();

  return 0;
}

