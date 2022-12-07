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
  double NucleonTonneScale = 6.02831;
  
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
  filters::NuMode* filt_numu_ccqe = new filters::NuMode(14, enums::kCC, enums::kQE);
  filters::NuMode* filt_nue_ccqe = new filters::NuMode(12, enums::kCC, enums::kQE);
  filters::NuMode* filt_numu_ccmec = new filters::NuMode(14,  enums::kCC,enums::kMEC);
  filters::NuMode* filt_numu_ccres = new filters::NuMode(14,  enums::kCC,enums::kRes);

  filters::NuMode* filt_numu_nc = new filters::NuMode(14, enums::kNC, enums::kUndefined);
  filters::NuMode* filt_nue_nc = new filters::NuMode(12, enums::kNC, enums::kUndefined);

  // Define distributions (plots)
  std::vector<Distribution*> dists = {
    // numCCQE --------------------------------------------------------------------//
	new distributions::Enu_true("numu_ccqe_enu", filt_numu_ccqe),
	new distributions::q0("numu_ccqe", filt_numu_ccqe),
	new distributions::q0Reco("numu_ccqe", filt_numu_ccqe),
	new distributions::q3("numu_ccqe", filt_numu_ccqe),
	new distributions::q3Reco("numu_ccqe", filt_numu_ccqe),
	new distributions::TheoristsNu("numu_ccqe", filt_numu_ccqe),
	new distributions::Q2("numu_ccqe", filt_numu_ccqe),
	new distributions::Q2Reco("numu_ccqe", filt_numu_ccqe),
	new distributions::TheoristsW("numu_ccqe", filt_numu_ccqe),
	new distributions::ExperimentalistsW("numu_ccqe", filt_numu_ccqe),
	new distributions::TheoristsBjorkenX("numu_ccqe", filt_numu_ccqe),
	new distributions::ExperimentalistsBjorkenX("numu_ccqe", filt_numu_ccqe),
	new distributions::TheoristsInelasticityY("numu_ccqe", filt_numu_ccqe),
	new distributions::ExperimentalistsInelasticityY("numu_ccqe", filt_numu_ccqe),
	new distributions::RecoInelasticityY("numu_ccqe", filt_numu_ccqe),
	new distributions::PLep("numu_ccqe", filt_numu_ccqe),
	new distributions::ThetaLep("numu_ccqe", filt_numu_ccqe),
	new distributions::ErecRelBias("numu_ccqe", filt_numu_ccqe),
	new distributions::ErecAbsBias("numu_ccqe", filt_numu_ccqe),
	new distributions::Erec("numu_ccqe", filt_numu_ccqe),
	new distributions::Emiss("numu_ccqe", filt_numu_ccqe),
	new distributions::Pmiss("numu_ccqe", filt_numu_ccqe),
	new distributions::PPLead("numu_ccqe", filt_numu_ccqe),
	new distributions::PLead_KE("numu_ccqe", filt_numu_ccqe),
	new distributions::ThetaPLead("numu_ccqe", filt_numu_ccqe),
	new distributions::PNLead("numu_ccqe", filt_numu_ccqe),
	new distributions::NLead_KE("numu_ccqe", filt_numu_ccqe),
	new distributions::ThetaNLead("numu_ccqe", filt_numu_ccqe),
	//new distributions::PPiLead("numu_ccqe", filt_numu_ccqe),
	//new distributions::ThetaPiLead("numu_ccqe", filt_numu_ccqe),
    new distributions::Mult("numu_ccqe_multp", filt_numu_ccqe, 2212),
    new distributions::Mult("numu_ccqe_multp_30MeV", filt_numu_ccqe, 2212, 0.03),
    new distributions::Mult("numu_ccqe_multn", filt_numu_ccqe, 2112),
    new distributions::Mult("numu_ccqe_multpip", filt_numu_ccqe, 211),
    new distributions::Mult("numu_ccqe_multpim", filt_numu_ccqe, -211),
    new distributions::Mult("numu_ccqe_multpi0", filt_numu_ccqe, 111),
    new distributions::Mult("numu_ccqe_multkp", filt_numu_ccqe, 321),
    new distributions::Mult("numu_ccqe_multkm", filt_numu_ccqe, -321),
    new distributions::Mult("numu_ccqe_multk0", filt_numu_ccqe, 311),
    new distributions::Mult_Nucl("numu_ccqe_multNucleons", filt_numu_ccqe),
	new distributions::tki_dpt("numu_ccqe", filt_numu_ccqe),
	new distributions::tki_dat("numu_ccqe", filt_numu_ccqe),
	new distributions::tki_dphit("numu_ccqe", filt_numu_ccqe),
	new distributions::pmuT("numu_ccqe", filt_numu_ccqe),
	new distributions::pmuL("numu_ccqe", filt_numu_ccqe),
	new distributions::ErecYrec("numu_ccqe", filt_numu_ccqe),
	new distributions::EmissPmiss("numu_ccqe", filt_numu_ccqe),
	new distributions::Q0Q3("numu_ccqe", filt_numu_ccqe),
	new distributions::PThetaLep("numu_ccqe", filt_numu_ccqe),
	new distributions::PLep_TL("numu_ccqe", filt_numu_ccqe),
	new distributions::Q2W("numu_ccqe", filt_numu_ccqe),
	new distributions::BjorkenX_InelY("numu_ccqe", filt_numu_ccqe),
	new distributions::Enu_Erec("numu_ccqe", filt_numu_ccqe),

	// numuRES ------------------------------------------------------------------------//
	new distributions::Enu_true("numu_ccres_enu", filt_numu_ccres),
	new distributions::q0("numu_ccres", filt_numu_ccres),
	new distributions::q0Reco("numu_ccres", filt_numu_ccres),
	new distributions::q3("numu_ccres", filt_numu_ccres),
	new distributions::q3Reco("numu_ccres", filt_numu_ccres),
	new distributions::TheoristsNu("numu_ccres", filt_numu_ccres),
	new distributions::Q2("numu_ccres", filt_numu_ccres),
	new distributions::Q2Reco("numu_ccres", filt_numu_ccres),
	new distributions::TheoristsW("numu_ccres", filt_numu_ccres),
	new distributions::ExperimentalistsW("numu_ccres", filt_numu_ccres),
	new distributions::TheoristsBjorkenX("numu_ccres", filt_numu_ccres),
	new distributions::ExperimentalistsBjorkenX("numu_ccres", filt_numu_ccres),
	new distributions::TheoristsInelasticityY("numu_ccres", filt_numu_ccres),
	new distributions::ExperimentalistsInelasticityY("numu_ccres", filt_numu_ccres),
	new distributions::RecoInelasticityY("numu_ccres", filt_numu_ccres),
	new distributions::PLep("numu_ccres", filt_numu_ccres),
	new distributions::ThetaLep("numu_ccres", filt_numu_ccres),
	new distributions::ErecRelBias("numu_ccres", filt_numu_ccres),
	new distributions::ErecAbsBias("numu_ccres", filt_numu_ccres),
	new distributions::Erec("numu_ccres", filt_numu_ccres),
	new distributions::Emiss("numu_ccres", filt_numu_ccres),
	new distributions::Pmiss("numu_ccres", filt_numu_ccres),
	new distributions::PPLead("numu_ccres", filt_numu_ccres),
	new distributions::PLead_KE("numu_ccres", filt_numu_ccres),
	new distributions::ThetaPLead("numu_ccres", filt_numu_ccres),
	new distributions::PNLead("numu_ccres", filt_numu_ccres),
	new distributions::NLead_KE("numu_ccres", filt_numu_ccres),
	new distributions::ThetaNLead("numu_ccres", filt_numu_ccres),
	new distributions::PPiLead("numu_ccres", filt_numu_ccres),
	new distributions::ThetaPiLead("numu_ccres", filt_numu_ccres),
    new distributions::Mult("numu_ccres_multp", filt_numu_ccres, 2212),
    new distributions::Mult("numu_ccres_multp_30MeV", filt_numu_ccres, 2212, 0.03),
    new distributions::Mult("numu_ccres_multn", filt_numu_ccres, 2112),
    new distributions::Mult("numu_ccres_multpip", filt_numu_ccres, 211),
    new distributions::Mult("numu_ccres_multpim", filt_numu_ccres, -211),
    new distributions::Mult("numu_ccres_multpi0", filt_numu_ccres, 111),
    new distributions::Mult("numu_ccres_multkp", filt_numu_ccres, 321),
    new distributions::Mult("numu_ccres_multkm", filt_numu_ccres, -321),
    new distributions::Mult("numu_ccres_multk0", filt_numu_ccres, 311),
    new distributions::Mult_Nucl("numu_ccres_multNucleons", filt_numu_ccres),
	new distributions::tki_dpt("numu_ccres", filt_numu_ccres),
	new distributions::tki_dat("numu_ccres", filt_numu_ccres),
	new distributions::tki_dphit("numu_ccres", filt_numu_ccres),
	new distributions::pmuT("numu_ccres", filt_numu_ccres),
	new distributions::pmuL("numu_ccres", filt_numu_ccres),
	new distributions::ErecYrec("numu_ccres", filt_numu_ccres),
	new distributions::EmissPmiss("numu_ccres", filt_numu_ccres),
	new distributions::Q0Q3("numu_ccres", filt_numu_ccres),
	new distributions::PThetaLep("numu_ccres", filt_numu_ccres),
	new distributions::PLep_TL("numu_ccres", filt_numu_ccres),
	new distributions::Q2W("numu_ccres", filt_numu_ccres),
	new distributions::BjorkenX_InelY("numu_ccres", filt_numu_ccres),
	new distributions::Enu_Erec("numu_ccres", filt_numu_ccres),
	
	
	// numuMEC ------------------------------------------------------------------------//
	new distributions::Enu_true("numu_ccmec_enu", filt_numu_ccmec),
	new distributions::q0("numu_ccmec", filt_numu_ccmec),
	new distributions::q0Reco("numu_ccmec", filt_numu_ccmec),
	new distributions::q3("numu_ccmec", filt_numu_ccmec),
	new distributions::q3Reco("numu_ccmec", filt_numu_ccmec),
	//new distributions::TheoristsNu("numu_ccmec", filt_numu_ccmec),
	new distributions::Q2("numu_ccmec", filt_numu_ccmec),
	new distributions::Q2Reco("numu_ccmec", filt_numu_ccmec),
	//new distributions::TheoristsW("numu_ccmec", filt_numu_ccmec),
	new distributions::ExperimentalistsW("numu_ccmec", filt_numu_ccmec),
	//new distributions::TheoristsBjorkenX("numu_ccmec", filt_numu_ccmec),
	new distributions::ExperimentalistsBjorkenX("numu_ccmec", filt_numu_ccmec),
	//new distributions::TheoristsInelasticityY("numu_ccmec", filt_numu_ccmec),
	new distributions::ExperimentalistsInelasticityY("numu_ccmec", filt_numu_ccmec),
	new distributions::RecoInelasticityY("numu_ccmec", filt_numu_ccmec),
	new distributions::PLep("numu_ccmec", filt_numu_ccmec),
	new distributions::ThetaLep("numu_ccmec", filt_numu_ccmec),
	new distributions::ErecRelBias("numu_ccmec", filt_numu_ccmec),
	new distributions::ErecAbsBias("numu_ccmec", filt_numu_ccmec),
	new distributions::Erec("numu_ccmec", filt_numu_ccmec),
	new distributions::Emiss("numu_ccmec", filt_numu_ccmec),
	new distributions::Pmiss("numu_ccmec", filt_numu_ccmec),
	new distributions::PPLead("numu_ccmec", filt_numu_ccmec),
	new distributions::PLead_KE("numu_ccmec", filt_numu_ccmec),
	new distributions::ThetaPLead("numu_ccmec", filt_numu_ccmec),
	new distributions::PNLead("numu_ccmec", filt_numu_ccmec),
	new distributions::NLead_KE("numu_ccmec", filt_numu_ccmec),
	new distributions::ThetaNLead("numu_ccmec", filt_numu_ccmec),
	new distributions::PPiLead("numu_ccmec", filt_numu_ccmec),
	new distributions::ThetaPiLead("numu_ccmec", filt_numu_ccmec),
    new distributions::Mult("numu_ccmec_multp", filt_numu_ccmec, 2212),
    new distributions::Mult("numu_ccmec_multp_30MeV", filt_numu_ccmec, 2212, 0.03),
    new distributions::Mult("numu_ccmec_multn", filt_numu_ccmec, 2112),
    new distributions::Mult("numu_ccmec_multpip", filt_numu_ccmec, 211),
    new distributions::Mult("numu_ccmec_multpim", filt_numu_ccmec, -211),
    new distributions::Mult("numu_ccmec_multpi0", filt_numu_ccmec, 111),
    new distributions::Mult("numu_ccmec_multkp", filt_numu_ccmec, 321),
    new distributions::Mult("numu_ccmec_multkm", filt_numu_ccmec, -321),
    new distributions::Mult("numu_ccmec_multk0", filt_numu_ccmec, 311),
    new distributions::Mult_Nucl("numu_ccmec_multNucleons", filt_numu_ccmec),
	new distributions::tki_dpt("numu_ccmec", filt_numu_ccmec),
	new distributions::tki_dat("numu_ccmec", filt_numu_ccmec),
	new distributions::tki_dphit("numu_ccmec", filt_numu_ccmec),
	//new distributions::pmuT("numu_ccmec", filt_numu_ccmec),
	//new distributions::pmuL("numu_ccmec", filt_numu_ccmec),
	new distributions::ErecYrec("numu_ccmec", filt_numu_ccmec),
	new distributions::EmissPmiss("numu_ccmec", filt_numu_ccmec),
	new distributions::Q0Q3("numu_ccmec", filt_numu_ccmec),
	new distributions::PThetaLep("numu_ccmec", filt_numu_ccmec),
	//new distributions::PLep_TL("numu_ccmec", filt_numu_ccmec),
	new distributions::Q2W("numu_ccmec", filt_numu_ccmec),
	new distributions::BjorkenX_InelY("numu_ccmec", filt_numu_ccmec),
	new distributions::Enu_Erec("numu_ccmec", filt_numu_ccmec),

	
	// nueCCQE ------------------------------------------------------------------------//
	new distributions::Enu_true("nue_ccqe_enu", filt_nue_ccqe),
	new distributions::q0("nue_ccqe", filt_nue_ccqe),
	new distributions::q0Reco("nue_ccqe", filt_nue_ccqe),
	new distributions::q3("nue_ccqe", filt_nue_ccqe),
	new distributions::q3Reco("nue_ccqe", filt_nue_ccqe),
	new distributions::TheoristsNu("nue_ccqe", filt_nue_ccqe),
	new distributions::Q2("nue_ccqe", filt_nue_ccqe),
	new distributions::Q2Reco("nue_ccqe", filt_nue_ccqe),
	new distributions::TheoristsW("nue_ccqe", filt_nue_ccqe),
	new distributions::ExperimentalistsW("nue_ccqe", filt_nue_ccqe),
	new distributions::TheoristsBjorkenX("nue_ccqe", filt_nue_ccqe),
	new distributions::ExperimentalistsBjorkenX("nue_ccqe", filt_nue_ccqe),
	new distributions::TheoristsInelasticityY("nue_ccqe", filt_nue_ccqe),
	new distributions::ExperimentalistsInelasticityY("nue_ccqe", filt_nue_ccqe),
	new distributions::RecoInelasticityY("nue_ccqe", filt_nue_ccqe),
	new distributions::PLep("nue_ccqe", filt_nue_ccqe),
	new distributions::ThetaLep("nue_ccqe", filt_nue_ccqe),
	new distributions::ErecRelBias("nue_ccqe", filt_nue_ccqe),
	new distributions::ErecAbsBias("nue_ccqe", filt_nue_ccqe),
	new distributions::Erec("nue_ccqe", filt_nue_ccqe),
	new distributions::Emiss("nue_ccqe", filt_nue_ccqe),
	new distributions::Pmiss("nue_ccqe", filt_nue_ccqe),
	new distributions::PPLead("nue_ccqe", filt_nue_ccqe),
	new distributions::PLead_KE("nue_ccqe", filt_nue_ccqe),
	new distributions::ThetaPLead("nue_ccqe", filt_nue_ccqe),
	new distributions::PNLead("nue_ccqe", filt_nue_ccqe),
	new distributions::NLead_KE("nue_ccqe", filt_nue_ccqe),
	new distributions::ThetaNLead("nue_ccqe", filt_nue_ccqe),
	new distributions::PPiLead("nue_ccqe", filt_nue_ccqe),
	new distributions::ThetaPiLead("nue_ccqe", filt_nue_ccqe),
    new distributions::Mult("nue_ccqe_multp", filt_nue_ccqe, 2212),
    new distributions::Mult("nue_ccqe_multp_30MeV", filt_nue_ccqe, 2212, 0.03),
    new distributions::Mult("nue_ccqe_multn", filt_nue_ccqe, 2112),
    new distributions::Mult("nue_ccqe_multpip", filt_nue_ccqe, 211),
    new distributions::Mult("nue_ccqe_multpim", filt_nue_ccqe, -211),
    new distributions::Mult("nue_ccqe_multpi0", filt_nue_ccqe, 111),
    new distributions::Mult("nue_ccqe_multkp", filt_nue_ccqe, 321),
    new distributions::Mult("nue_ccqe_multkm", filt_nue_ccqe, -321),
    new distributions::Mult("nue_ccqe_multk0", filt_nue_ccqe, 311),
    new distributions::Mult_Nucl("nue_ccqe_multNucleons", filt_nue_ccqe),
	new distributions::tki_dpt("nue_ccqe", filt_nue_ccqe),
	new distributions::tki_dat("nue_ccqe", filt_nue_ccqe),
	new distributions::tki_dphit("nue_ccqe", filt_nue_ccqe),
	new distributions::pmuT("nue_ccqe", filt_nue_ccqe),
	new distributions::pmuL("nue_ccqe", filt_nue_ccqe),
	new distributions::ErecYrec("nue_ccqe", filt_nue_ccqe),
	new distributions::EmissPmiss("nue_ccqe", filt_nue_ccqe),
	new distributions::Q0Q3("nue_ccqe", filt_nue_ccqe),
	new distributions::PThetaLep("nue_ccqe", filt_nue_ccqe),
	new distributions::PLep_TL("nue_ccqe", filt_nue_ccqe),
	new distributions::Q2W("nue_ccqe", filt_nue_ccqe),
	new distributions::BjorkenX_InelY("nue_ccqe", filt_nue_ccqe),
	new distributions::Enu_Erec("nue_ccqe", filt_nue_ccqe),
	
	// numu NC ------------------------------------------------------------------------//
	new distributions::Enu_true("numu_nc", filt_numu_nc),
	new distributions::q0("numu_nc", filt_numu_nc),
	new distributions::q0Reco("numu_nc", filt_numu_nc),
	new distributions::q3("numu_nc", filt_numu_nc),
	new distributions::q3Reco("numu_nc", filt_numu_nc),
	new distributions::TheoristsNu("numu_nc", filt_numu_nc),
	new distributions::Q2("numu_nc", filt_numu_nc),
	new distributions::Q2Reco("numu_nc", filt_numu_nc),
	new distributions::TheoristsW("numu_nc", filt_numu_nc),
	new distributions::ExperimentalistsW("numu_nc", filt_numu_nc),
	new distributions::TheoristsBjorkenX("numu_nc", filt_numu_nc),
	new distributions::ExperimentalistsBjorkenX("numu_nc", filt_numu_nc),
	new distributions::TheoristsInelasticityY("numu_nc", filt_numu_nc),
	new distributions::ExperimentalistsInelasticityY("numu_nc", filt_numu_nc),
	new distributions::RecoInelasticityY("numu_nc", filt_numu_nc),
	new distributions::PLep("numu_nc", filt_numu_nc),
	new distributions::ThetaLep("numu_nc", filt_numu_nc),
	new distributions::ErecRelBias("numu_nc", filt_numu_nc),
	new distributions::ErecAbsBias("numu_nc", filt_numu_nc),
	new distributions::Erec("numu_nc", filt_numu_nc),
	new distributions::Emiss("numu_nc", filt_numu_nc),
	new distributions::Pmiss("numu_nc", filt_numu_nc),
	new distributions::PPLead("numu_nc", filt_numu_nc),
	new distributions::PLead_KE("numu_nc", filt_numu_nc),
	new distributions::ThetaPLead("numu_nc", filt_numu_nc),
	new distributions::PNLead("numu_nc", filt_numu_nc),
	new distributions::NLead_KE("numu_nc", filt_numu_nc),
	new distributions::ThetaNLead("numu_nc", filt_numu_nc),
	new distributions::PPiLead("numu_nc", filt_numu_nc),
	new distributions::ThetaPiLead("numu_nc", filt_numu_nc),
    new distributions::Mult("numu_nc_multp", filt_numu_nc, 2212),
    new distributions::Mult("numu_nc_multp_30MeV", filt_numu_nc, 2212, 0.03),
    new distributions::Mult("numu_nc_multn", filt_numu_nc, 2112),
    new distributions::Mult("numu_nc_multpip", filt_numu_nc, 211),
    new distributions::Mult("numu_nc_multpim", filt_numu_nc, -211),
    new distributions::Mult("numu_nc_multpi0", filt_numu_nc, 111),
    new distributions::Mult("numu_nc_multkp", filt_numu_nc, 321),
    new distributions::Mult("numu_nc_multkm", filt_numu_nc, -321),
    new distributions::Mult("numu_nc_multk0", filt_numu_nc, 311),
    new distributions::Mult_Nucl("numu_nc_multNucleons", filt_numu_nc),
	//new distributions::tki_dpt("numu_nc", filt_numu_nc),
	//new distributions::tki_dat("numu_nc", filt_numu_nc),
	//new distributions::tki_dphit("numu_nc", filt_numu_nc),
	new distributions::pmuT("numu_nc", filt_numu_nc),
	new distributions::pmuL("numu_nc", filt_numu_nc),
	new distributions::ErecYrec("numu_nc", filt_numu_nc),
	new distributions::EmissPmiss("numu_nc", filt_numu_nc),
	new distributions::Q0Q3("numu_nc", filt_numu_nc),
	new distributions::PThetaLep("numu_nc", filt_numu_nc),
	new distributions::PLep_TL("numu_nc", filt_numu_nc),
	new distributions::Q2W("numu_nc", filt_numu_nc),
	new distributions::BjorkenX_InelY("numu_nc", filt_numu_nc),
	new distributions::Enu_Erec("numu_nc", filt_numu_nc),
	
	// nue NC -------------------------------------------------------------------------//
	new distributions::Enu_true("nue_nc", filt_nue_nc),
	new distributions::q0("nue_nc", filt_nue_nc),
	new distributions::q0Reco("nue_nc", filt_nue_nc),
	new distributions::q3("nue_nc", filt_nue_nc),
	new distributions::q3Reco("nue_nc", filt_nue_nc),
	new distributions::TheoristsNu("nue_nc", filt_nue_nc),
	new distributions::Q2("nue_nc", filt_nue_nc),
	new distributions::Q2Reco("nue_nc", filt_nue_nc),
	new distributions::TheoristsW("nue_nc", filt_nue_nc),
	new distributions::ExperimentalistsW("nue_nc", filt_nue_nc),
	new distributions::TheoristsBjorkenX("nue_nc", filt_nue_nc),
	new distributions::ExperimentalistsBjorkenX("nue_nc", filt_nue_nc),
	new distributions::TheoristsInelasticityY("nue_nc", filt_nue_nc),
	new distributions::ExperimentalistsInelasticityY("nue_nc", filt_nue_nc),
	new distributions::RecoInelasticityY("nue_nc", filt_nue_nc),
	new distributions::PLep("nue_nc", filt_nue_nc),
	new distributions::ThetaLep("nue_nc", filt_nue_nc),
	new distributions::ErecRelBias("nue_nc", filt_nue_nc),
	new distributions::ErecAbsBias("nue_nc", filt_nue_nc),
	new distributions::Erec("nue_nc", filt_nue_nc),
	new distributions::Emiss("nue_nc", filt_nue_nc),
	new distributions::Pmiss("nue_nc", filt_nue_nc),
	new distributions::PPLead("nue_nc", filt_nue_nc),
	new distributions::PLead_KE("nue_nc", filt_nue_nc),
	new distributions::ThetaPLead("nue_nc", filt_nue_nc),
	new distributions::PNLead("nue_nc", filt_nue_nc),
	new distributions::NLead_KE("nue_nc", filt_nue_nc),
	new distributions::ThetaNLead("nue_nc", filt_nue_nc),
	new distributions::PPiLead("nue_nc", filt_nue_nc),
	new distributions::ThetaPiLead("nue_nc", filt_nue_nc),
    new distributions::Mult("nue_nc_multp", filt_nue_nc, 2212),
    new distributions::Mult("nue_nc_multp_30MeV", filt_nue_nc, 2212, 0.03),
    new distributions::Mult("nue_nc_multn", filt_nue_nc, 2112),
    new distributions::Mult("nue_nc_multpip", filt_nue_nc, 211),
    new distributions::Mult("nue_nc_multpim", filt_nue_nc, -211),
    new distributions::Mult("nue_nc_multpi0", filt_nue_nc, 111),
    new distributions::Mult("nue_nc_multkp", filt_nue_nc, 321),
    new distributions::Mult("nue_nc_multkm", filt_nue_nc, -321),
    new distributions::Mult("nue_nc_multk0", filt_nue_nc, 311),
    new distributions::Mult_Nucl("nue_nc_multNucleons", filt_nue_nc),
	//new distributions::tki_dpt("nue_nc", filt_nue_nc),
	//new distributions::tki_dat("nue_nc", filt_nue_nc),
	//new distributions::tki_dphit("nue_nc", filt_nue_nc),
	new distributions::pmuT("nue_nc", filt_nue_nc),
	new distributions::pmuL("nue_nc", filt_nue_nc),
	new distributions::ErecYrec("nue_nc", filt_nue_nc),
	new distributions::EmissPmiss("nue_nc", filt_nue_nc),
	new distributions::Q0Q3("nue_nc", filt_nue_nc),
	new distributions::PThetaLep("nue_nc", filt_nue_nc),
	new distributions::PLep_TL("nue_nc", filt_nue_nc),
	new distributions::Q2W("nue_nc", filt_nue_nc),
	new distributions::BjorkenX_InelY("nue_nc", filt_nue_nc),
	new distributions::Enu_Erec("nue_nc", filt_nue_nc)

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
