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
    new distributions::TheoristsW("num_ccqe_thw", filt_num_ccqe),
    new distributions::TheoristsBjorkenX("num_ccqe_thbjorkenx", filt_num_ccqe),
    new distributions::TheoristsInelasticityY("num_ccqe_thinely", filt_num_ccqe),
    new distributions::ExperimentalistsW("num_ccqe_expw", filt_num_ccqe),
    new distributions::ExperimentalistsBjorkenX("num_ccqe_expbjorkenx", filt_num_ccqe),
    new distributions::ExperimentalistsInelasticityY("num_ccqe_expinely", filt_num_ccqe),
    new distributions::TheoristsNu("num_ccqe_thnu",filt_num_ccqe),
    new distributions::ExperimentalistsNu("num_ccqe_expnu",filt_num_ccqe),
    new distributions::BindingE("num_ccqe_be",filt_num_ccqe),
    new distributions::PLep("num_ccqe_plep", filt_num_ccqe),
    new distributions::ThetaLep("num_ccqe_tlep", filt_num_ccqe),
    new distributions::PThetaLep("num_ccqe_ptlep", filt_num_ccqe),
    new distributions::PPLead("num_ccqe_pp", filt_num_ccqe),
    new distributions::ThetaPLead("num_ccqe_tp", filt_num_ccqe),
    new distributions::ThetaPLead("num_ccqe_tp_40MeV", filt_num_ccqe, 0.04),
    new distributions::ThetaLepPLead("num_ccqe_tlepp", filt_num_ccqe),
    new distributions::ThetaLepPLead("num_ccqe_tlepp_40MeV", filt_num_ccqe, 0.04),
    new distributions::dPhiLepPLead("num_ccqe_dphilp", filt_num_ccqe),
    new distributions::dPhiLepPLead("num_ccqe_dphilp_40MeV", filt_num_ccqe, 0.04),
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
    new distributions::TheoristsW("nue_ccqe_thw", filt_nue_ccqe),
    new distributions::TheoristsBjorkenX("nue_ccqe_thbjorkenx", filt_nue_ccqe),
    new distributions::TheoristsInelasticityY("nue_ccqe_thinely", filt_nue_ccqe),
    new distributions::ExperimentalistsW("nue_ccqe_expw", filt_nue_ccqe),
    new distributions::ExperimentalistsBjorkenX("nue_ccqe_expbjorkenx", filt_nue_ccqe),
    new distributions::ExperimentalistsInelasticityY("nue_ccqe_expinely", filt_nue_ccqe),
    new distributions::TheoristsNu("nue_ccqe_thnu",filt_nue_ccqe),
    new distributions::ExperimentalistsNu("nue_ccqe_expnu",filt_nue_ccqe),
    new distributions::BindingE("nue_ccqe_be",filt_nue_ccqe),
    new distributions::PLep("nue_ccqe_plep", filt_nue_ccqe),
    new distributions::ThetaLep("nue_ccqe_tlep", filt_nue_ccqe),
    new distributions::PThetaLep("nue_ccqe_ptlep", filt_nue_ccqe),
    new distributions::PPLead("nue_ccqe_pp", filt_nue_ccqe),
    new distributions::ThetaPLead("nue_ccqe_tp", filt_nue_ccqe),
    new distributions::ThetaPLead("nue_ccqe_tp_40MeV", filt_nue_ccqe, 0.04),
    new distributions::ThetaLepPLead("nue_ccqe_tlepp", filt_nue_ccqe),
    new distributions::ThetaLepPLead("nue_ccqe_tlepp_40MeV", filt_nue_ccqe, 0.04),
    new distributions::dPhiLepPLead("nue_ccqe_dphilp", filt_nue_ccqe),
    new distributions::dPhiLepPLead("nue_ccqe_dphilp_40MeV", filt_nue_ccqe, 0.04),
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
    new distributions::PPLead("num_ccmec_pp", filt_num_ccmec),
    new distributions::ThetaPLead("num_ccmec_tp", filt_num_ccmec),
    new distributions::ThetaPLead("num_ccmec_tp_40MeV", filt_num_ccmec, 0.04),
    new distributions::ThetaLepPLead("num_ccmec_tlepp", filt_num_ccmec),
    new distributions::ThetaLepPLead("num_ccmec_tlepp_40MeV", filt_num_ccmec, 0.04),
    new distributions::dPhiLepPLead("num_ccmec_dphilp", filt_num_ccmec),
    new distributions::dPhiLepPLead("num_ccmec_dphilp_40MeV", filt_num_ccmec, 0.04),

    // numCCRes
    new distributions::Q0Q3("num_ccres_q0q3", filt_num_ccres),
    new distributions::TheoristsW("num_ccres_thw", filt_num_ccres),
    new distributions::TheoristsBjorkenX("num_ccres_thbjorkenx", filt_num_ccres),
    new distributions::TheoristsInelasticityY("num_ccres_thinely", filt_num_ccres),
    new distributions::ExperimentalistsW("num_ccres_expw", filt_num_ccres),
    new distributions::ExperimentalistsBjorkenX("num_ccres_expbjorkenx", filt_num_ccres),
    new distributions::ExperimentalistsInelasticityY("num_ccres_expinely", filt_num_ccres),
    new distributions::TheoristsNu("num_ccres_thnu",filt_num_ccres),
    new distributions::ExperimentalistsNu("num_ccres_expnu",filt_num_ccres),
    new distributions::BindingE("num_ccres_be",filt_num_ccres),
    new distributions::PLep("num_ccres_plep", filt_num_ccres),
    new distributions::ThetaLep("num_ccres_tlep", filt_num_ccres),
    new distributions::PThetaLep("num_ccres_ptlep", filt_num_ccres),
    new distributions::PPiLead("num_ccres_ppi", filt_num_ccres),
    new distributions::ThetaPiLead("num_ccres_tpi", filt_num_ccres),
    new distributions::ThetaLepPiLead("num_ccres_tlpi", filt_num_ccres),
    new distributions::PPLead("num_ccres_pp", filt_num_ccres),
    new distributions::ThetaPLead("num_ccres_tp", filt_num_ccres),
    new distributions::ThetaPLead("num_ccres_tp_40MeV", filt_num_ccres, 0.04),
    new distributions::ThetaLepPLead("num_ccres_tlepp", filt_num_ccres),
    new distributions::ThetaLepPLead("num_ccres_tlepp_40MeV", filt_num_ccres, 0.04),
    new distributions::dPhiLepPLead("num_ccres_dphilp", filt_num_ccres),
    new distributions::dPhiLepPLead("num_ccres_dphilp_40MeV", filt_num_ccres, 0.04),
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
