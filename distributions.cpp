#include <iostream>
#include <string>
#include "TCanvas.h"
#include "TDatabasePDG.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TMath.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "distributions.h"
#include "filter.h"
#include <iostream>

// From GENIE: Decoding Z from the PDG code (PDG ion code convention: 10LZZZAAAI)
int IonPdgCodeToZ(int ion_pdgc) {
  int Z = (ion_pdgc/10000) - 1000*(ion_pdgc/10000000); // don't factor out!
  return Z;
}

// From GENIE: Decoding A from the PDG code (PDG ion code convention: 10LZZZAAAI)
int IonPdgCodeToA(int ion_pdgc) {
  int A = (ion_pdgc/10) - 1000*(ion_pdgc/10000); // don't factor out!
  return A;
}


Distribution::Distribution(std::string _name, std::string _title,
             TH1* _hist, Filter* _filter)
    : hist(_hist), filter(_filter), name(_name), title(_title) {}


void Distribution::Write() {
  std::cout << "WRITE " << hist->GetName() << std::endl;
  hist->Write();
}


void Distribution::Save(TCanvas* c) {
  bool own_canvas = c == NULL;
  c = (c == NULL ? new TCanvas("c1", "", 500, 500) : c);
  c->cd();
  hist->Draw("colz");
  c->SaveAs((std::string("hist_") + name + ".png").c_str());
  if (own_canvas) delete c;
}


namespace distributions {

  Q2::Q2(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Q^{2}, ") + _filter->title;
    std::string hname = "hq2_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Q^{2} (GeV^{2});Events").c_str(),
                    20, 0, 2);
  }

  #ifdef __LARSOFT__
  void Q2::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().QSqr(), w);
  }
  #endif

  void Q2::Fill(const NuisTree& nuistr){
    dynamic_cast<TH1F*>(hist)->Fill(nuistr.Q2,nuistr.Weight);
  }


  TheoristsW::TheoristsW(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Theorists W = sqrt(p.p + 2p.q - Q^2), ") + _filter->title;
    std::string hname = "hthw_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Theorists W = sqrt(p.p + 2p.q - Q^2) (GeV);Events").c_str(),
                    20, 0, 2);
  }

  #ifdef __LARSOFT__
  void TheoristsW::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();

    TLorentzVector q = nu.Nu().Momentum() - nu.Lepton().Momentum();

    float Q2 = q.Mag2()*-1;

    // To get 4-vector p we need the struck nucleon
    // Get the struck nucleon from the particle stack
    // Check particle 2 - if status code = 11, this is the struc nucleon
    // If status code != 11, we are looking at an interaction with a free nucleon, which will be saved by GENIE as particle 1.
    int i_nuc = -999;

    if (truth.GetParticle(2).StatusCode() == 11){
      i_nuc = 2;
    }
    else{
      i_nuc = 1;
    }
    int nuc_pdg = truth.GetParticle(i_nuc).PdgCode();

    assert(nuc_pdg==2212 || nuc_pdg==2112 || nuc_pdg==1000010010 || nuc_pdg==1000000010);

    TLorentzVector p = truth.GetParticle(i_nuc).Momentum();

    float w_theorist = TMath::Sqrt(p.Mag2() + 2*p.Dot(q) - Q2);

    dynamic_cast<TH1F*>(hist)->Fill(w_theorist, w);
  }
  #endif

  void TheoristsW::Fill(const NuisTree& nuistr) {
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
      if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112){
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
      if (nuistr.initp_pdg[i]==nuistr.PDGnu){
        // check this is the only neutrino we've found
        assert(i_nu==-999);
        i_nu = i;
      }
    }

    // Find lepton  in list of final state particles
    int i_lep = -999;
    for (int i=0; i<nuistr.nfsp; i++) {
      if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
        // check this is the only lepton we've found
        assert(i_lep==-999);
        i_lep = i;
      }
    }

    assert (i_nuc!=-999);
    assert (i_nu!=-999);
    assert (i_lep!=-999);

    TLorentzVector k(nuistr.initp_px[i_nu],nuistr.initp_py[i_nu],nuistr.initp_pz[i_nu],nuistr.initp_E[i_nu]);
    TLorentzVector kprime(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep],nuistr.fsp_E[i_lep]);

    TLorentzVector q = k-kprime;
    // Sanity check: q should match saved values in tree!
    assert(q.Mag2()*-1 - nuistr.Q2 < 1e-4);

    TLorentzVector p(nuistr.initp_px[i_nuc],nuistr.initp_py[i_nuc],nuistr.initp_pz[i_nuc],nuistr.initp_E[i_nuc]);

    float w_theorist = TMath::Sqrt(p.Mag2() + 2*p.Dot(q) - nuistr.Q2);

    dynamic_cast<TH1F*>(hist)->Fill(w_theorist,nuistr.Weight);
  }


  ExperimentalistsW::ExperimentalistsW(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Experimentalists W = sqrt(M^2 + 2Mq0 - Q^2), ") + _filter->title;
    std::string hname = "hexpw_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Experimentalists W = sqrt(M^2 + 2Mq0 - Q^2) (GeV);Events").c_str(),
                    20, 0.5, 1.5);
  }

  #ifdef __LARSOFT__
  void ExperimentalistsW::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().W(), w);
  }
  #endif

  void ExperimentalistsW::Fill(const NuisTree& nuistr) {
    //dynamic_cast<TH1F*>(hist)->Fill(nuistr.W_nuc_rest,nuistr.Weight);

    float Q2 = nuistr.Q2;
    float q0 = nuistr.q0;
    float M = 0.93956541; // neutron mass GeV
    float W = TMath::Sqrt(M*M + 2*M*q0 - Q2);
    dynamic_cast<TH1F*>(hist)->Fill(W,nuistr.Weight);
  }


  TheoristsBjorkenX::TheoristsBjorkenX(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Theorists Bjorken x = Q^2/(2p.q), ") + _filter->title;
    std::string hname = "hthbx_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Theorists Bjorken x = Q^2/(2p.q);Events").c_str(),
                    10, 0, 1);
  }

  #ifdef __LARSOFT__
  void TheoristsBjorkenX::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();

    TLorentzVector q = nu.Nu().Momentum() - nu.Lepton().Momentum();

    float Q2 = q.Mag2()*-1;

    // To get 4-vector p we need the struck nucleon
    // Get the struck nucleon from the particle stack
    // Check particle 2 - if status code = 11, this is the struc nucleon
    // If status code != 11, we are looking at an interaction with a free nucleon, which will be saved by GENIE as particle 1.
    int i_nuc = -999;
    if (truth.GetParticle(2).StatusCode() == 11){
      i_nuc = 2;
    }
    else{
      i_nuc = 1;
    }
    int nuc_pdg = truth.GetParticle(i_nuc).PdgCode();
    assert(nuc_pdg==2212 || nuc_pdg==2112 || nuc_pdg==1000010010 || nuc_pdg==1000000010);

    TLorentzVector p = truth.GetParticle(i_nuc).Momentum();

    float x = Q2/(2*p.Dot(q));

    dynamic_cast<TH1F*>(hist)->Fill(x, w);
  }
  #endif

  void TheoristsBjorkenX::Fill(const NuisTree& nuistr) {
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
      if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112){
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
      if (nuistr.initp_pdg[i]==nuistr.PDGnu){
        // check this is the only neutrino we've found
        assert(i_nu==-999);
        i_nu = i;
      }
    }

    // Find lepton  in list of final state particles
    int i_lep = -999;
    for (int i=0; i<nuistr.nfsp; i++) {
      if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
        // check this is the only lepton we've found
        assert(i_lep==-999);
        i_lep = i;
      }
    }

    assert (i_nuc!=-999);
    assert (i_nu!=-999);
    assert (i_lep!=-999);

    TLorentzVector k(nuistr.initp_px[i_nu],nuistr.initp_py[i_nu],nuistr.initp_pz[i_nu],nuistr.initp_E[i_nu]);
    TLorentzVector kprime(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep],nuistr.fsp_E[i_lep]);

    TLorentzVector q = k-kprime;
    // Sanity check: q should match saved values in tree!
    assert(q.Mag2()*-1 - nuistr.Q2 < 1e-4);

    TLorentzVector p(nuistr.initp_px[i_nuc],nuistr.initp_py[i_nuc],nuistr.initp_pz[i_nuc],nuistr.initp_E[i_nuc]);

    float x = nuistr.Q2/(2*p.Dot(q));

    dynamic_cast<TH1F*>(hist)->Fill(x, nuistr.Weight);
  }


  ExperimentalistsBjorkenX::ExperimentalistsBjorkenX(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Experimentalists Bjorken x = Q^2/(2Mq0), ") + _filter->title;
    std::string hname = "hexpbx_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Experimentalists Bjorken x = Q^2/(2Mq0);Events").c_str(),
                    15, 0, 1.5);
  }

  #ifdef __LARSOFT__
  void ExperimentalistsBjorkenX::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().X(), w);
  }
  #endif

  void ExperimentalistsBjorkenX::Fill(const NuisTree& nuistr) {
    //dynamic_cast<TH1F*>(hist)->Fill(nuistr.x, nuistr.Weight);

    float Q2 = nuistr.Q2;
    float q0 = nuistr.q0;
    float M = 0.93956541; // neutron mass GeV                                                                               
    float x = Q2/(2*M*q0);
    dynamic_cast<TH1F*>(hist)->Fill(x,nuistr.Weight);
  }


  TheoristsInelasticityY::TheoristsInelasticityY(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Theorists Inelasticity y = (p.q)/(p.k), ") + _filter->title;
    std::string hname = "hthinely_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Theorists Inelasticity y = (p.q)/(p.k);Events").c_str(),
                    20, 0, 1);
  }

  #ifdef __LARSOFT__
  void TheoristsInelasticityY::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();

    TLorentzVector q = nu.Nu().Momentum() - nu.Lepton().Momentum();

    // To get 4-vector p we need the struck nucleon
    // Get the struck nucleon from the particle stack
    // Check particle 2 - if status code = 11, this is the struc nucleon
    // If status code != 11, we are looking at an interaction with a free nucleon, which will be saved by GENIE as particle 1.
    int i_nuc = -999;
    if (truth.GetParticle(2).StatusCode() == 11){
      i_nuc = 2;
    }
    else{
      i_nuc = 1;
    }
    int nuc_pdg = truth.GetParticle(i_nuc).PdgCode();
    assert(nuc_pdg==2212 || nuc_pdg==2112 || nuc_pdg==1000010010 || nuc_pdg==1000000010);

    TLorentzVector p = truth.GetParticle(i_nuc).Momentum();

    TLorentzVector k = nu.Nu().Momentum();

    float y = (p.Dot(q))/(p.Dot(k));

    dynamic_cast<TH1F*>(hist)->Fill(y, w);
  }
  #endif

  void TheoristsInelasticityY::Fill(const NuisTree& nuistr) {
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
      if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112){
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
      if (nuistr.initp_pdg[i]==nuistr.PDGnu){
        // check this is the only neutrino we've found
        assert(i_nu==-999);
        i_nu = i;
      }
    }

    // Find lepton  in list of final state particles
    int i_lep = -999;
    for (int i=0; i<nuistr.nfsp; i++) {
      if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
        // check this is the only lepton we've found
        assert(i_lep==-999);
        i_lep = i;
      }
    }

    assert (i_nuc!=-999);
    assert (i_nu!=-999);
    assert (i_lep!=-999);

    TLorentzVector k(nuistr.initp_px[i_nu],nuistr.initp_py[i_nu],nuistr.initp_pz[i_nu],nuistr.initp_E[i_nu]);
    TLorentzVector kprime(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep],nuistr.fsp_E[i_lep]);

    TLorentzVector q = k-kprime;
    // Sanity check: q should match saved values in tree!
    assert(q.Mag2()*-1 - nuistr.Q2 < 1e-4);

    TLorentzVector p(nuistr.initp_px[i_nuc],nuistr.initp_py[i_nuc],nuistr.initp_pz[i_nuc],nuistr.initp_E[i_nuc]);

    float y = (p.Dot(q))/(p.Dot(k));

    dynamic_cast<TH1F*>(hist)->Fill(y, nuistr.Weight);
  }


  ExperimentalistsInelasticityY::ExperimentalistsInelasticityY(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Experimentalists Inelasticity y = 1-(Elep/Enu), ") + _filter->title;
    std::string hname = "hexpinely_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Experimentalists Inelasticity y = 1-(Elep/Enu);Events").c_str(),
                    20, 0, 1);
  }

  #ifdef __LARSOFT__
  void ExperimentalistsInelasticityY::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().Y(), w);
  }
  #endif

  void ExperimentalistsInelasticityY::Fill(const NuisTree& nuistr) {
    dynamic_cast<TH1F*>(hist)->Fill(nuistr.y, nuistr.Weight);
  }


  TheoristsNu::TheoristsNu(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Theorists nu = p.q/sqrt(p^2), ") + _filter->title;
    std::string hname = "hthnu_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Theorists nu = p.q/sqrt(p^2);Events").c_str(),
                    20, 0, 1);
  }

  #ifdef __LARSOFT__
  void TheoristsNu::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();

    TLorentzVector q = nu.Nu().Momentum() - nu.Lepton().Momentum();

    // To get 4-vector p we need the struck nucleon
    // Get the struck nucleon from the particle stack
    // Check particle 2 - if status code = 11, this is the struc nucleon
    // If status code != 11, we are looking at an interaction with a free nucleon, which will be saved by GENIE as particle 1.
    int i_nuc = -999;
    if (truth.GetParticle(2).StatusCode() == 11){
      i_nuc = 2;
    }
    else{
      i_nuc = 1;
    }
    int nuc_pdg = truth.GetParticle(i_nuc).PdgCode();
    assert(nuc_pdg==2212 || nuc_pdg==2112 || nuc_pdg==1000010010 || nuc_pdg==1000000010);

    TLorentzVector p = truth.GetParticle(i_nuc).Momentum();

    float nu_theorist = p.Dot(q)/TMath::Sqrt(p.Mag2());

    dynamic_cast<TH1F*>(hist)->Fill(nu_theorist, w);
  }
  #endif

  void TheoristsNu::Fill(const NuisTree& nuistr) {
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
      if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112){
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
      if (nuistr.initp_pdg[i]==nuistr.PDGnu){
        // check this is the only neutrino we've found
        assert(i_nu==-999);
        i_nu = i;
      }
    }

    // Find lepton  in list of final state particles
    int i_lep = -999;
    for (int i=0; i<nuistr.nfsp; i++) {
      if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
        // check this is the only lepton we've found
        assert(i_lep==-999);
        i_lep = i;
      }
    }

    assert (i_nuc!=-999);
    assert (i_nu!=-999);
    assert (i_lep!=-999);

    TLorentzVector k(nuistr.initp_px[i_nu],nuistr.initp_py[i_nu],nuistr.initp_pz[i_nu],nuistr.initp_E[i_nu]);
    TLorentzVector kprime(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep],nuistr.fsp_E[i_lep]);

    TLorentzVector q = k-kprime;
    // Sanity check: q should match saved values in tree!
    assert((q.Mag2()*-1 - nuistr.Q2) < 1e-4);

    TLorentzVector p(nuistr.initp_px[i_nuc],nuistr.initp_py[i_nuc],nuistr.initp_pz[i_nuc],nuistr.initp_E[i_nuc]);

    float nu_theorist = (p.Dot(q))/(p.Mag());

    dynamic_cast<TH1F*>(hist)->Fill(nu_theorist, nuistr.Weight);
  }


  ExperimentalistsNu::ExperimentalistsNu(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Experimentalists nu = Enu-Elep = q0, ") + _filter->title;
    std::string hname = "hexpnu_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Experimentalists nu = Enu-Elep = q0;Events").c_str(),
                    20, 0, 1);
  }

  #ifdef __LARSOFT__
  void ExperimentalistsNu::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();
    float q0 = nu.Nu().E() - nu.Lepton().E();
    dynamic_cast<TH1F*>(hist)->Fill(q0, w);
  }
  #endif

  void ExperimentalistsNu::Fill(const NuisTree& nuistr) {
    dynamic_cast<TH1F*>(hist)->Fill(nuistr.q0, nuistr.Weight);
  }


  BindingE::BindingE(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Binding Energy from energy balance (GeV), ") + _filter->title;
    std::string hname = "hbe_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Binding Energy from energy balance (GeV);Events").c_str(),
                    50, 0, 0.1);
  }

  #ifdef __LARSOFT__
  void BindingE::Fill(const simb::MCTruth& truth, float w) {
    constexpr double TARGET_MASS = 37.215526; // 40Ar, GeV
    constexpr double NEUTRON_MASS = 0.93956541; // GeV
    // constexpr double PROTON_MASS = 0.93827208; // GeV

    const simb::MCNeutrino& nu = truth.GetNeutrino();

    // To get 4-vector p we need the struck nucleon
    // Get the struck nucleon from the particle stack
    // Check particle 2 - if status code = 11, this is the struc nucleon
    // If status code != 11, we are looking at an interaction with a free nucleon, which will be saved by GENIE as particle 1.
    int i_nuc = -999;
    if (truth.GetParticle(2).StatusCode() == 11){
      i_nuc = 2;
    }
    else{
      i_nuc = 1;
    }
    int nuc_pdg = truth.GetParticle(i_nuc).PdgCode();
    assert(nuc_pdg==2212 || nuc_pdg==2112 || nuc_pdg==1000010010 || nuc_pdg==1000000010);

    TLorentzVector p4v = nu.Nu().Momentum(); // neutrino
    TLorentzVector p4Ni = truth.GetParticle(i_nuc).Momentum(); // initial hit nucleon
    TLorentzVector p4l = nu.Lepton().Momentum(); // lepton
    TLorentzVector p4i(0., 0., 0., TARGET_MASS); // target

    // Final nucleon 4-momentum: p + k = p' + k' -> p' = p + k - k' -> p' = p + q
    TLorentzVector p4Nf = p4Ni + p4v - p4l;

    // Recoil nucleus 4-momentum
    TLorentzVector p4f = p4v + p4i - p4l - p4Nf;
    // Recoiling nucleus mass (takes into account any excitation energy implied
    // by the initial bound nucleon 4-momentum)
    double mf = p4f.M();
    // Kinetic energy of the recoiling nucleus
    double Tf = p4f.E() - mf;

    // Binding energy from hit nucleon total energy
    // double Eb_from_En = NEUTRON_MASS - p4Ni.E() - Tf;
    // Binding energy from full-event energy conservation
    double reco_Eb = p4v.E() + NEUTRON_MASS - p4Nf.E() - p4l.E() - Tf;

    //std::cout << p4v.E() << ", " << p4Nf.E() << ", " << p4l.E() << ": " << reco_Eb << std::endl;

    dynamic_cast<TH1F*>(hist)->Fill(reco_Eb, w);
  }
  #endif

  void BindingE::Fill(const NuisTree& nuistr) {
    constexpr double TARGET_MASS = 37.215526; // 40Ar, GeV
    constexpr double NEUTRON_MASS = 0.93956541; // GeV
    // constexpr double PROTON_MASS = 0.93827208; // GeV
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
      if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112){
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
      if (nuistr.initp_pdg[i]==nuistr.PDGnu){
        // check this is the only neutrino we've found
        assert(i_nu==-999);
        i_nu = i;
      }
    }

    // Find lepton  in list of final state particles
    int i_lep = -999;
    for (int i=0; i<nuistr.nfsp; i++) {
      if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
        // check this is the only lepton we've found
        assert(i_lep==-999);
        i_lep = i;
      }
    }

    assert (i_nuc!=-999);
    assert (i_nu!=-999);
    assert (i_lep!=-999);

    TLorentzVector p4v(nuistr.initp_px[i_nu],nuistr.initp_py[i_nu],nuistr.initp_pz[i_nu],nuistr.initp_E[i_nu]); // neutrino
    TLorentzVector p4Ni(nuistr.initp_px[i_nuc],nuistr.initp_py[i_nuc],nuistr.initp_pz[i_nuc],nuistr.initp_E[i_nuc]); // initial hit nucleon
    TLorentzVector p4l(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep],nuistr.fsp_E[i_lep]); // lepton
    TLorentzVector p4i(0., 0., 0., TARGET_MASS); // target

    // Final nucleon 4-momentum: p + k = p' + k' -> p' = p + k - k' -> p' = p + q
    TLorentzVector p4Nf = p4Ni + p4v - p4l;

    // Recoil nucleus 4-momentum
    TLorentzVector p4f = p4v + p4i - p4l - p4Nf;
    // Recoiling nucleus mass (takes into account any excitation energy implied
    // by the initial bound nucleon 4-momentum)
    double mf = p4f.M();
    // Kinetic energy of the recoiling nucleus
    double Tf = p4f.E() - mf;

    // Binding energy from hit nucleon total energy
    // double Eb_from_En = NEUTRON_MASS - p4Ni.E() - Tf;
    // Binding energy from full-event energy conservation
    double reco_Eb = p4v.E() + NEUTRON_MASS - p4Nf.E() - p4l.E() - Tf;

    dynamic_cast<TH1F*>(hist)->Fill(reco_Eb, nuistr.Weight);
  }



  PLep::PLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("p_{lep}, ") + _filter->title;
    std::string hname = "hpl_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{lep} (GeV);Events").c_str(),
                    20, 0, 2);
  }

  #ifdef __LARSOFT__
  void PLep::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().Lepton().P(), w);
  }
  #endif

  void PLep::Fill(const NuisTree& nuistr) {
    // Find final-state lepton in stack
    int i_lep=-999;
    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
        // check this is the only lepton we've found
        if (i_lep!=-999){
          // previously found another lepton -- error!
        }
        i_lep = i;
      }
    }

    // Now get lepton momentum from components.
    TVector3 pv(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]);
    float p = pv.Mag();

    dynamic_cast<TH1F*>(hist)->Fill(p,nuistr.Weight);
  }


  ThetaLep::ThetaLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("cos#theta_{lep}, ") + _filter->title;
    std::string hname = "htl_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{lep};Events").c_str(),
                    50, -1, 1);
  }

  #ifdef __LARSOFT__
  void ThetaLep::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(cos(truth.GetNeutrino().Lepton().Momentum().Theta()), w);
  }
  #endif

  void ThetaLep::Fill(const NuisTree& nuistr) {
    //dynamic_cast<TH1F*>(hist)->Fill(nuistr.CosLep,nuistr.Weight);

    // Find lepton  in list of final state particles
    int i_lep = -999;
    for (int i=0; i<nuistr.nfsp; i++) {
      if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
        // check this is the only lepton we've found
        assert(i_lep==-999);
        i_lep = i;
      }
    }

    assert (i_lep!=-999);

    TVector3 p3l(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]); // lepton
    float costh = p3l.Unit().Dot(TVector3(0,0,1));
    dynamic_cast<TH1F*>(hist)->Fill(costh,nuistr.Weight);
  }


  Q0Q3::Q0Q3(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("q^{0}/q^{3}, ") + _filter->title;
    std::string hname = "hq0q3_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + ";q^{0} (GeV);q^{3} (GeV);Events").c_str(),
                    48, 0, 1.2, 48, 0, 1.2);
  }

  #ifdef __LARSOFT__
  void Q0Q3::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();
    float q0 = nu.Nu().E() - nu.Lepton().E();
    float q3 = (nu.Nu().Momentum().Vect() - nu.Lepton().Momentum().Vect()).Mag();
    dynamic_cast<TH2F*>(hist)->Fill(q3, q0, w);
  }
  #endif

  void Q0Q3::Fill(const NuisTree& nuistr) {
    dynamic_cast<TH2F*>(hist)->Fill(nuistr.q3, nuistr.q0, nuistr.Weight);
  }


  LeadPKEQ0::LeadPKEQ0(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Leading p KE vs. q^{0}, ") + _filter->title;
    std::string hname = "hpkeq0_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + ";Leading proton KE (GeV);q^{0} (GeV);Events").c_str(),
                    50, 0, 0.5, 50, 0, 0.5);
  }

  #ifdef __LARSOFT__
  void LeadPKEQ0::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();
    float q0 = nu.Nu().E() - nu.Lepton().E();

    float plead = 0;
    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);
      if (p.PdgCode() == 2212 && p.StatusCode() == genie::kIStStableFinalState) {
        if (p.P() > plead) {
          plead = p.E() - 0.938;
        }
      }
    }

    dynamic_cast<TH2F*>(hist)->Fill(plead, q0, w);
  }
  #endif

  void LeadPKEQ0::Fill(const NuisTree& nuistr) {
    // Loop through final-state particles and find leading proton (defined by highest KE)
    float KElead = 0;
    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i] == 2212){
        if ((nuistr.fsp_E[i] - 0.938) > KElead){
          KElead = nuistr.fsp_E[i] - 0.938;
        }
      }
    }

    dynamic_cast<TH2F*>(hist)->Fill(KElead, nuistr.q0, nuistr.Weight);
  }


  PThetaLep::PThetaLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Lepton p/#theta, ") + _filter->title;
    std::string hname = "hptl_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + ";p_{lep};cos#theta_{lep};Events").c_str(),
                    20, 0, 2, 50, -1, 1);
  }

  #ifdef __LARSOFT__
  void PThetaLep::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCParticle& lep = truth.GetNeutrino().Lepton();
    float p = lep.P();
    float ct = cos(lep.Momentum().Theta());
    dynamic_cast<TH2F*>(hist)->Fill(p, ct, w);
  }
  #endif

  void PThetaLep::Fill(const NuisTree& nuistr) {
    // Find final-state lepton in stack
    int i_lep=-999;
    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
        // check this is the only lepton we've found
        assert(i_lep==-999);
        i_lep = i;
      }
    }

    // Now get lepton momentum from components.
    TVector3 pv(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]);
    float p = pv.Mag();

    dynamic_cast<TH2F*>(hist)->Fill(p, nuistr.CosLep, nuistr.Weight);
  }


  Pke::Pke(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Proton KE, ") + _filter->title;
    std::string hname = "hpke_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + ";Leading proton KE T_{p1} (GeV);Subleading proton KE T_{p2} (GeV);Events").c_str(),
                    20, 0, 1, 20, 0, 1);
  }

  #ifdef __LARSOFT__
  void Pke::Fill(const simb::MCTruth& truth, float w) {
    float plead = 0;
    float psub = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);
      if (p.PdgCode() == 2212 && p.StatusCode() == genie::kIStStableFinalState && p.P() > psub) {
        if (p.P() > plead) {
          psub = plead;
          plead = p.E() - 0.938;
        }
        else {
          psub = p.E() - 0.938;
        }
      }
    }

    dynamic_cast<TH2F*>(hist)->Fill(plead, psub, w);
  }
  #endif

  void Pke::Fill(const NuisTree& nuistr) {

    // Loop through final-state particles and find leading and subleading proton (defined by highest KE)
    float KElead = 0;
    float KEsub = 0;
    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i] == 2212){
          float ke = nuistr.fsp_E[i] - 0.938;
          if (ke > KEsub){
            if (ke > KElead){
              KEsub = KElead;
              KElead = ke;
            }
            else{
              KEsub = ke;
            }
          }
      }
    }

    dynamic_cast<TH2F*>(hist)->Fill(KElead, KEsub, nuistr.Weight);
  }

  PPLead::PPLead(std::string _name, Filter* _filter)
      : Distribution(_name, _filter) {
    title = std::string("Leading p, p_{p}, ") + _filter->title;
    std::string hname = "hpp_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{p} (GeV);Events").c_str(),
                    20, 0, 2);
  }

  #ifdef __LARSOFT__
  void PPLead::Fill(const simb::MCTruth& truth, float w) {
    float plead = 0;
    int nprot = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);

      if (p.StatusCode() != genie::kIStStableFinalState) {
        continue;
      }

      if (p.PdgCode() == 2212) {
        nprot++;
        if (p.P() > plead) {
          plead = p.P();
        }
      }
    }

    if (nprot>0){
      dynamic_cast<TH1F*>(hist)->Fill(plead, w);
    }
  }
  #endif

  void PPLead::Fill(const NuisTree& nuistr) {

    // Loop through final-state particles and find leading proton (defined by highest KE)
    float plead = 0;
    int nprot = 0;
    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i] == 2212){
        nprot++;
        TVector3 pv(nuistr.fsp_px[i],nuistr.fsp_py[i],nuistr.fsp_pz[i]);
        float p = pv.Mag();
        if (p > plead){
          plead = p;
        }
      }
    }

    if (nprot>0){
      dynamic_cast<TH1F*>(hist)->Fill(plead, nuistr.Weight);
    }
  }


  ThetaPLead::ThetaPLead(std::string _name, Filter* _filter, float _ethreshold)
      : Distribution(_name, _filter), ethreshold(_ethreshold) {
    title = std::string("Leading p, cos#theta_{p}, ") + _filter->title;
    if (ethreshold > 0) title += std::string(", KE_{p} > ") + std::to_string(ethreshold*1000) + std::string(" MeV");
    std::string hname = "htp_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{p};Events").c_str(),
                    50, -1, 1);
  }

  #ifdef __LARSOFT__
  void ThetaPLead::Fill(const simb::MCTruth& truth, float w) {
    size_t np = 0;
    float plead = 0;
    float ctlead = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);

      if (p.StatusCode() != genie::kIStStableFinalState) {
        continue;
      }

      if (p.PdgCode() == 2212 && (p.E() - 0.938272) > ethreshold) {
        np++;
        if (p.P() > plead) {
          plead = p.P();
          ctlead = cos(p.Momentum().Theta());
        }
      }
    }

    if (np > 0) {
      dynamic_cast<TH1F*>(hist)->Fill(ctlead, w);
    }
  }
  #endif

  void ThetaPLead::Fill(const NuisTree& nuistr) {

      size_t np = 0;
      float plead = 0;
      float ctlead = 0;

      for (int i=0; i<nuistr.nfsp; i++) {

        if (nuistr.fsp_pdg[i] == 2212 && (nuistr.fsp_E[i] - 0.938272) > ethreshold) {
          np++;
          TVector3 pv(nuistr.fsp_px[i],nuistr.fsp_py[i],nuistr.fsp_pz[i]);
          float p = pv.Mag();
          if (p > plead) {
            plead = p;
            ctlead = cos(pv.Theta());
          }
        }
      }

      if (np > 0) {
        dynamic_cast<TH1F*>(hist)->Fill(ctlead, nuistr.Weight);
      }
  }


  ThetaLepPLead::ThetaLepPLead(std::string _name, Filter* _filter, float _ethreshold)
      : Distribution(_name, _filter), ethreshold(_ethreshold) {
    title = std::string("Leading p, cos#theta_{lep,p}, ") + _filter->title;
    if (ethreshold > 0) title += std::string(", KE_{p} > ") + std::to_string(ethreshold*1000) + std::string(" MeV");
    std::string hname = "htlepp_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{lep,p};Events").c_str(),
                    50, -1, 1);
  }

  #ifdef __LARSOFT__
  void ThetaLepPLead::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCParticle& lep = truth.GetNeutrino().Lepton();

    size_t np = 0;
    float plead = 0;
    float ctlep = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);

      if (p.StatusCode() != genie::kIStStableFinalState) {
        continue;
      }

      if (p.PdgCode() == 2212 && (p.E() - 0.938272) > ethreshold) {
        np++;
        if (p.P() > plead) {
          plead = p.P();
          ctlep = cos(lep.Momentum().Vect().Angle(p.Momentum().Vect()));
        }
      }
    }

    if (np > 0) {
      dynamic_cast<TH1F*>(hist)->Fill(ctlep, w);
    }
  }
  #endif

  void ThetaLepPLead::Fill(const NuisTree& nuistr) {

      size_t np = 0;
      float plead = 0;
      float ctlep = 0;
      int i_lep = -999;
      int i_leadingp = -999;

      for (int i=0; i<nuistr.nfsp; i++) {

        // Find lepton
        if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
          // check this is the only lepton we've found
          assert(i_lep==-999);
          i_lep = i;
        }

        // Find leading proton (above threshold)
        if (nuistr.fsp_pdg[i] == 2212 && (nuistr.fsp_E[i] - 0.938272) > ethreshold) {
          np++;
          TVector3 pv(nuistr.fsp_px[i],nuistr.fsp_py[i],nuistr.fsp_pz[i]);
          float p = pv.Mag();
          if (p > plead) {
            plead = p;
            i_leadingp = i;
          }
        }
      }

      if (np > 0) {
        TVector3 pv_lep(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]);
        TVector3 pv_leadingp(nuistr.fsp_px[i_leadingp],nuistr.fsp_py[i_leadingp],nuistr.fsp_pz[i_leadingp]);
        ctlep = cos(pv_lep.Angle(pv_leadingp));
        dynamic_cast<TH1F*>(hist)->Fill(ctlep, nuistr.Weight);
      }
  }


  dPhiLepPLead::dPhiLepPLead(std::string _name, Filter* _filter, float _ethreshold)
      : Distribution(_name, _filter), ethreshold(_ethreshold) {
    title = std::string("Leading p, #Delta#phi_{lep,p}, ") + _filter->title;
    if (ethreshold > 0) title += std::string(", KE_{p} > ") + std::to_string(ethreshold*1000) + std::string(" MeV");
    std::string hname = "hdphilp_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";#Delta#phi_{lep,p};Events").c_str(),
                    20, -1, 1);
  }

  #ifdef __LARSOFT__
  void dPhiLepPLead::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCParticle& lep = truth.GetNeutrino().Lepton();

    size_t np = 0;
    float plead = 0;
    float dphilep = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);

      if (p.StatusCode() != genie::kIStStableFinalState) {
        continue;
      }

      if (p.PdgCode() == 2212 && (p.E() - 0.938272) > ethreshold) {
        np++;
        if (p.P() > plead) {
          plead = p.P();
          dphilep = lep.Momentum().Vect().DeltaPhi(p.Momentum().Vect());
        }
      }
    }

    if (np > 0) {
      dynamic_cast<TH1F*>(hist)->Fill(dphilep, w);
    }
  }
  #endif

  void dPhiLepPLead::Fill(const NuisTree& nuistr) {

      size_t np = 0;
      float plead = 0;
      float dphilep = 0;
      int i_lep = -999;
      int i_leadingp = -999;

      for (int i=0; i<nuistr.nfsp; i++) {

        // Find lepton
        if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
          // check this is the only lepton we've found
          assert(i_lep==-999);
          i_lep = i;
        }

        // Find leading proton (above threshold)
        if (nuistr.fsp_pdg[i] == 2212 && (nuistr.fsp_E[i] - 0.938272) > ethreshold) {
          np++;
          TVector3 pv(nuistr.fsp_px[i],nuistr.fsp_py[i],nuistr.fsp_pz[i]);
          float p = pv.Mag();
          if (p > plead) {
            plead = p;
            i_leadingp = i;
          }
        }
      }

      if (np > 0) {
        TVector3 pv_lep(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]);
        TVector3 pv_leadingp(nuistr.fsp_px[i_leadingp],nuistr.fsp_py[i_leadingp],nuistr.fsp_pz[i_leadingp]);
        dphilep = pv_lep.DeltaPhi(pv_leadingp);
        dynamic_cast<TH1F*>(hist)->Fill(dphilep, nuistr.Weight);
      }
  }


  Mult::Mult(std::string _name, Filter* _filter, int _pdg, float _ethreshold)
      : Distribution(_name, _filter), pdg(_pdg), ethreshold(_ethreshold) {
    char spdg[100];
    snprintf(spdg, 100, "%i", pdg);
    title = std::string("Multiplicity, PDG ") + spdg + ", " + _filter->title;
    std::string hname = std::string("hmult_") + spdg + "_" + name;
    hist = new TH1F(hname.c_str(), (title + ";N_{" + spdg + "}").c_str(), 20, 0, 20);
  }

  #ifdef __LARSOFT__
  void Mult::Fill(const simb::MCTruth& truth, float w) {
    size_t nf = 0;
    mass = TDatabasePDG::Instance()->GetParticle(pdg)->Mass();

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);
      if (p.PdgCode() == pdg && p.StatusCode() == genie::kIStStableFinalState && (p.E() - mass) > ethreshold) {
        nf++;
      }
    }

    dynamic_cast<TH1F*>(hist)->Fill(nf, w);
  }
  #endif

  void Mult::Fill(const NuisTree& nuistr) {
    size_t nf = 0;
    if (TMath::Abs(pdg) == 13) mass = 0.105658; // muon
    else if (TMath::Abs(pdg) == 11) mass = 0.000510; // electron
    else if (pdg == 311) mass = 0.497648; // k0
    else if (TMath::Abs(pdg) == 321) mass = 0.493677; // k+/-
    else if (pdg == 111) mass = 0.134977; // pi0
    else if (TMath::Abs(pdg) == 211) mass = 0.139570; // pi+/-
    else if (pdg == 2112) mass = 0.939565; // neutron
    else if (pdg == 2212) mass = 0.938272; // proton
    else{
      std::cout << "Error: could not assign mass for particle with pdg " << pdg << ". Setting mass to 0 - this will mess up any thresholding you try to apply!" << std::endl;
      mass = 0;
    }\

    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i] == pdg && (nuistr.fsp_E[i] - mass) > ethreshold){
        nf++;
      }
    }

    dynamic_cast<TH1F*>(hist)->Fill(nf, nuistr.Weight);
  }


  IMult::IMult(std::string _name, Filter* _filter, int _pdg)
      : Distribution(_name, _filter), pdg(_pdg) {
    char spdg[100];
    snprintf(spdg, 100, "%i", pdg);
    title = std::string("Pre-FSI Multiplicity, PDG ") + spdg + ", " + _filter->title;
    std::string hname = std::string("himult_") + spdg + "_" + name;
    hist = new TH1F(hname.c_str(), (title + ";N_{" + spdg + "}").c_str(), 20, 0, 20);
  }

  #ifdef __LARSOFT__
  void IMult::Fill(const simb::MCTruth& truth, float w) {
    size_t nf = -999;

    assert(pdg == 2212 || pdg == 2112 || pdg == 211 || pdg == -211 || pdg ==111);

    for (int i=0; i<truth.NParticles(); i++){
      const simb::MCParticle &p = truth.GetParticle(i);
      if (p.PdgCode() == pdg && p.StatusCode() == genie::kIStHadronInTheNucleus){
	nf++;
      }
    }

    dynamic_cast<TH1F*>(hist)->Fill(nf, w);
  }
  #endif

  void IMult::Fill(const NuisTree& nuistr) {

    // Nuisance tree gives "vertex particles" -- not sure but assuming this is the same thing as intermediate particles
    // Actually, have recently found that Nuisance "vertex particles" might have a bug. Comenting this out and just filling with -999 to avoid confusion. If we decide we need this, may have to go back and check nuisance code first.

    size_t nf = -999;

    // for (int i=0; i<nuistr.nvertp; i++){
    //   if (nuistr.vertp_pdg[i] == pdg){
    //     nf++;
    //   }
    // }

    dynamic_cast<TH1F*>(hist)->Fill(nf, nuistr.Weight);
  }


  PPiLead::PPiLead(std::string _name, Filter* _filter, bool _charged)
      : Distribution(_name, _filter), charged(_charged) {
    title = std::string("Leading #pi, p_{#pi") + (charged ? "#pm" : "") + "}, " + _filter->title;
    std::string hname = "hppi_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{#pi} (GeV);Events").c_str(),
                    20, 0, 2);
  }

  #ifdef __LARSOFT__
  void PPiLead::Fill(const simb::MCTruth& truth, float w) {
    float plead = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);

      if (p.StatusCode() != genie::kIStStableFinalState) {
        continue;
      }

      if (abs(p.PdgCode()) == 211 || (!charged && p.PdgCode() == 111)) {
        if (p.P() > plead) {
          plead = p.P();
        }
      }
    }

    dynamic_cast<TH1F*>(hist)->Fill(plead, w);
  }
  #endif

  void PPiLead::Fill(const NuisTree& nuistr) {

    // Loop through final-state particles and find leading proton (defined by highest KE)
    float plead = 0;
    for (int i=0; i<nuistr.nfsp; i++){
      if (abs(nuistr.fsp_pdg[i]) == 211 || (!charged && nuistr.fsp_pdg[i] == 111)){
        TVector3 pv(nuistr.fsp_px[i],nuistr.fsp_py[i],nuistr.fsp_pz[i]);
        float p = pv.Mag();
        if (p > plead){
          plead = p;
        }
      }
    }

    dynamic_cast<TH1F*>(hist)->Fill(plead, nuistr.Weight);
  }


  ThetaPiLead::ThetaPiLead(std::string _name, Filter* _filter, bool _charged)
      : Distribution(_name, _filter), charged(_charged) {
    title = std::string("Leading #pi, cos#theta_{#pi") + (charged ? "#pm" : "") + "}, " + _filter->title;
    std::string hname = "htpi_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{#pi};Events").c_str(),
                    50, -1, 1);
  }

  #ifdef __LARSOFT__
  void ThetaPiLead::Fill(const simb::MCTruth& truth, float w) {
    size_t npi = 0;
    float plead = 0;
    float ctlead = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);

      if (p.StatusCode() != genie::kIStStableFinalState) {
        continue;
      }

      if (abs(p.PdgCode()) == 211 || (!charged && p.PdgCode() == 111)) {
        npi++;
        if (p.P() > plead) {
          plead = p.P();
          ctlead = cos(p.Momentum().Theta());
        }
      }
    }

    if (npi > 0) {
      dynamic_cast<TH1F*>(hist)->Fill(ctlead, w);
    }
  }
  #endif

  void ThetaPiLead::Fill(const NuisTree& nuistr) {

      size_t npi = 0;
      float plead = 0;
      float ctlead = 0;

      for (int i=0; i<nuistr.nfsp; i++) {

        if (abs(nuistr.fsp_pdg[i]) == 211 || (!charged && nuistr.fsp_pdg[i] == 111)) {
          npi++;
          TVector3 pv(nuistr.fsp_px[i],nuistr.fsp_py[i],nuistr.fsp_pz[i]);
          float p = pv.Mag();
          if (p > plead) {
            plead = p;
            ctlead = cos(pv.Theta());
          }
        }
      }

      if (npi > 0) {
        dynamic_cast<TH1F*>(hist)->Fill(ctlead, nuistr.Weight);
      }
  }


  ThetaLepPiLead::ThetaLepPiLead(std::string _name, Filter* _filter, bool _charged)
      : Distribution(_name, _filter), charged(_charged) {
    title = std::string("Leading #pi, cos#theta_{lep,#pi") + (charged ? "#pm" : "") + "}, " + _filter->title;
    std::string hname = "htlpi_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{lep,#pi};Events").c_str(),
                    50, -1, 1);
  }

  #ifdef __LARSOFT__
  void ThetaLepPiLead::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCParticle& lep = truth.GetNeutrino().Lepton();

    size_t npi = 0;
    float plead = 0;
    float ctlep = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);

      if (p.StatusCode() != genie::kIStStableFinalState) {
        continue;
      }

      if (abs(p.PdgCode()) == 211 || (!charged && p.PdgCode() == 111)) {
        npi++;
        if (p.P() > plead) {
          plead = p.P();
          ctlep = cos(lep.Momentum().Vect().Angle(p.Momentum().Vect()));
        }
      }
    }

    if (npi > 0) {
      dynamic_cast<TH1F*>(hist)->Fill(ctlep, w);
    }
  }
  #endif

  void ThetaLepPiLead::Fill(const NuisTree& nuistr) {

      size_t npi = 0;
      float plead = 0;
      float ctlep = 0;
      int i_lep = -999;
      int i_leadingpi = -999;

      for (int i=0; i<nuistr.nfsp; i++) {

        // Find lepton
        if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
          // check this is the only lepton we've found
          assert(i_lep==-999);
          i_lep = i;
        }

        // Find leading pion
        if (abs(nuistr.fsp_pdg[i]) == 211 || (!charged && nuistr.fsp_pdg[i] == 111)) {
          npi++;
          TVector3 pv(nuistr.fsp_px[i],nuistr.fsp_py[i],nuistr.fsp_pz[i]);
          float p = pv.Mag();
          if (p > plead) {
            plead = p;
            i_leadingpi = i;
          }
        }
      }

      if (npi > 0) {
        TVector3 pv_lep(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]);
        TVector3 pv_leadingpi(nuistr.fsp_px[i_leadingpi],nuistr.fsp_py[i_leadingpi],nuistr.fsp_pz[i_leadingpi]);
        ctlep = cos(pv_lep.Angle(pv_leadingpi));
        dynamic_cast<TH1F*>(hist)->Fill(ctlep, nuistr.Weight);
      }
  }


  ECons::ECons(std::string _name, Filter* _filter)
      : Distribution(_name, _filter) {
    title = std::string("Energy balance, ") + _filter->title;
    std::string hname = "hecons_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";#DeltaE (GeV);Events").c_str(),
                    50, -2.5, 2.5);
  }

  #ifdef __LARSOFT__
  void ECons::Fill(const simb::MCTruth& truth, float w) {
    float pmass = TDatabasePDG::Instance()->GetParticle(2212)->Mass();
    float nmass = TDatabasePDG::Instance()->GetParticle(2112)->Mass();

    //// Initial state
    const simb::MCNeutrino& nu = truth.GetNeutrino();

    // Neutrino energy
    float enu = nu.Nu().E();

    // Target nucleus rest mass
    float tgtmass;
    int tgtpdg = nu.Target();
    TParticlePDG* tgtparticle = TDatabasePDG::Instance()->GetParticle(tgtpdg);
    if (tgtparticle) {
      tgtmass = tgtparticle->Mass();
    }
    else {
      int tgtZ = IonPdgCodeToZ(tgtpdg);
      int tgtA = IonPdgCodeToA(tgtpdg);

      tgtmass = tgtZ * pmass + (tgtA - tgtZ) * nmass;
    }

    // Struck nucleon KE
    // Get the struck nucleon from the particle stack
    // Check particle 2 - if status code == 11, this is the struck nucleon
    // If status code != 11, we are looking at an interaction with a free nucleon, which will be saved by GENIE as particle 1
    int i_nuc = -999;
    if (truth.GetParticle(2).StatusCode() == 11){
      i_nuc = 2;
    }
    else{
      i_nuc = 1;
    }
    int nuc_pdg = truth.GetParticle(i_nuc).PdgCode();
    assert(nuc_pdg==2212 || nuc_pdg==2112 || nuc_pdg==1000010010 || nuc_pdg==1000000010);

    float nucmass = TDatabasePDG::Instance()->GetParticle(nuc_pdg)->Mass();
    float enuc = truth.GetParticle(i_nuc).Momentum().E() - nucmass;

    // Total
    float ei = enu + tgtmass + enuc;

    //// Final state
    // Lepton total energy
    float elep = nu.Lepton().E();

    // Nuclear remnant rest mass
    float remnantmass = 0;
    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);
      if (p.StatusCode() != genie::kIStFinalStateNuclearRemnant) {
        continue;
      }
      TParticlePDG* rpart = TDatabasePDG::Instance()->GetParticle(p.PdgCode());
      if (rpart) {
        remnantmass += tgtparticle->Mass();
      }
      else {
        int a = IonPdgCodeToA(p.PdgCode());
        int z = IonPdgCodeToZ(p.PdgCode());
        remnantmass += z * pmass + (a - z) * nmass;
      }
    }

    // Final state hadrons
    float ehad = 0;
    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);

      if (p.StatusCode() != genie::kIStStableFinalState) {
        continue;
      }

      TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(p.PdgCode());
      assert(particle || p.PdgCode() == 2000000101);
      float mass = particle ? particle->Mass() : 0;

      ehad += p.E() - mass;
    }

    // Total
    float ef = elep + remnantmass + ehad;

    // Balance
    float de = ei-ef;

    dynamic_cast<TH1F*>(hist)->Fill(de, w);
  }
  #endif

  void ECons::Fill(const NuisTree& nuistr) {
    // It's not clear that we can recreate this plot with NUISANCE trees (and I'm worried that if we try we will end up with inconsistencies of O(binding energy) with the GENIE implementation that could cause a lot of confusion) so don't try. Just fill with 0s -- if we need to make a similar plot to this in the future, can think through exactly what we want to show and whether that's possible to implement with the NUISANCE trees
    float de = 0;

    dynamic_cast<TH1F*>(hist)->Fill(de, nuistr.Weight);
  }

}  // namespace distributions
