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
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "canvas/Utilities/InputTag.h"
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

  std::string outfile = argv[1];
  std::vector<std::string> filename;
  for (int i=2; i<argc; i++) { 
    std::cout << "FILE " << argv[i] << std::endl; 
    filename.push_back(argv[i]);
  }

  // Define event filters
  filters::NuMode* filt_num_ccqe = new filters::NuMode(14, simb::kQE, simb::kCC);
  filters::NuMode* filt_num_ccmec = new filters::NuMode(14, simb::kMEC, simb::kCC);

  // Define distributions (plots)
  std::vector<Distribution*> dists = {
    // numCCQE
    new distributions::Q2("num_ccqe_q2", filt_num_ccqe),
    new distributions::Q0Q3("num_ccqe_q0q3", filt_num_ccqe),
    new distributions::W("num_ccqe_w", filt_num_ccqe),
    new distributions::BjorkenX("num_ccqe_bjorkenx", filt_num_ccqe),
    new distributions::InelasticityY("num_ccqe_inely", filt_num_ccqe),
    new distributions::PLep("num_ccqe_plep", filt_num_ccqe),
    new distributions::ThetaLep("num_ccqe_tlep", filt_num_ccqe),
    new distributions::PThetaLep("num_ccqe_ptlep", filt_num_ccqe),
    new distributions::MultP("num_ccqe_multp", filt_num_ccqe),
    new distributions::MultP("num_ccqe_multp_30MeV", filt_num_ccqe, 0.03),

    // numuCCMEC
    new distributions::Q0Q3("num_ccmec_q0q3", filt_num_ccmec),
    new distributions::Pke("num_ccmec_ppp", filt_num_ccmec)
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

    for (const simb::MCTruth& truth : *mctruths) {
      for (Distribution* dist : dists) {
        if ((*dist->filter)(truth)) {
          dist->Fill(truth);
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

