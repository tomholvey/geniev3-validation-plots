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
  c->SaveAs((std::string("hist_") + name + ".pdf").c_str());
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

  #ifndef __NO_LARSOFT__
  void Q2::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().QSqr(), w);
  }
  #endif

  void Q2::Fill(const NuisTree& nuistr){
    dynamic_cast<TH1F*>(hist)->Fill(nuistr.Q2,nuistr.Weight);
  }


  W::W(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("W, ") + _filter->title;
    std::string hname = "hw_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";W (GeV);Events").c_str(),
                    20, 0, 2);
  }

  #ifndef __NO_LARSOFT__
  void W::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().W(), w);
  }
  #endif

  void W::Fill(const NuisTree& nuistr) {
    dynamic_cast<TH1F*>(hist)->Fill(nuistr.W_genie,nuistr.Weight);
  }


  BjorkenX::BjorkenX(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Bjorken x, ") + _filter->title;
    std::string hname = "hbx_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Bjorken x;Events").c_str(),
                    30, 0, 3);
  }

  #ifndef __NO_LARSOFT__
  void BjorkenX::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().X(), w);
  }
  #endif

  void BjorkenX::Fill(const NuisTree& nuistr) {
    dynamic_cast<TH1F*>(hist)->Fill(nuistr.x, nuistr.Weight);
  }


  InelasticityY::InelasticityY(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Inelasticity y, ") + _filter->title;
    std::string hname = "hinely_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Inelasticity y;Events").c_str(),
                    20, 0, 1);
  }

  #ifndef __NO_LARSOFT__
  void InelasticityY::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().Y(), w);
  }
  #endif

  void InelasticityY::Fill(const NuisTree& nuistr) {
    dynamic_cast<TH1F*>(hist)->Fill(nuistr.y, nuistr.Weight);
  }


  PLep::PLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("p_{lep}, ") + _filter->title;
    std::string hname = "hpl_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{lep} (GeV);Events").c_str(),
                    20, 0, 2);
  }

  #ifndef __NO_LARSOFT__
  void PLep::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().Lepton().P(), w);
  }
  #endif

  void PLep::Fill(const NuisTree& nuistr) {
    // Find final-state lepton in stack
    int i_lep=-999;
    for (size_t i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i]==nuistr.PDGLep){
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

  #ifndef __NO_LARSOFT__
  void ThetaLep::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(cos(truth.GetNeutrino().Lepton().Momentum().Theta()), w);
  }
  #endif

  void ThetaLep::Fill(const NuisTree& nuistr) {
    dynamic_cast<TH1F*>(hist)->Fill(nuistr.CosLep,nuistr.Weight);
  }


  Q0Q3::Q0Q3(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("q^{0}/q^{3}, ") + _filter->title;
    std::string hname = "hq0q3_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + ";q^{0} (GeV);q^{3} (GeV);Events").c_str(),
                    48, 0, 1.2, 48, 0, 1.2);
  }

  #ifndef __NO_LARSOFT__
  void Q0Q3::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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

  #ifndef __NO_LARSOFT__
  void LeadPKEQ0::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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

  #ifndef __NO_LARSOFT__
  void PThetaLep::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
    const simb::MCParticle& lep = truth.GetNeutrino().Lepton();
    float p = lep.P();
    float ct = cos(lep.Momentum().Theta());
    dynamic_cast<TH2F*>(hist)->Fill(p, ct, w);
  }
  #endif

  void PThetaLep::Fill(const NuisTree& nuistr) {
    // Find final-state lepton in stack
    int i_lep=-999;
    for (size_t i=0; i<nuistr.nfsp; i++){
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

  #ifndef __NO_LARSOFT__
  void Pke::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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

  #ifndef __NO_LARSOFT__
  void PPLead::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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
    if (ethreshold > 0) title += std::string(", KE_{p} > ") + ethreshold*1000 + std::string(" MeV");
    std::string hname = "htp_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{p};Events").c_str(),
                    50, -1, 1);
  }

  #ifndef __NO_LARSOFT__
  void ThetaPLead::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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
    if (ethreshold > 0) title += std::string(", KE_{p} > ") + ethreshold*1000 + std::string(" MeV");
    std::string hname = "htlepp_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{lep,p};Events").c_str(),
                    50, -1, 1);
  }

  #ifndef __NO_LARSOFT__
  void ThetaLepPLead::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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


  Mult::Mult(std::string _name, Filter* _filter, int _pdg, float _ethreshold)
      : Distribution(_name, _filter), pdg(_pdg), ethreshold(_ethreshold) {
    char spdg[100];
    snprintf(spdg, 100, "%i", pdg);
    title = std::string("Multiplicity, PDG ") + spdg + ", " + _filter->title;
    std::string hname = std::string("hmult_") + spdg + "_" + name;
    hist = new TH1F(hname.c_str(), (title + ";N_{" + spdg + "}").c_str(), 20, 0, 20);
  }

  #ifndef __NO_LARSOFT__
  void Mult::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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

  #ifndef __NO_LARSOFT__
  void IMult::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
    size_t nf = -999;

    switch (pdg) {
      case 2212: nf = gtruth.fNumProton; break;
      case 2112: nf = gtruth.fNumNeutron; break;
      case  211: nf = gtruth.fNumPiPlus; break;
      case -211: nf = gtruth.fNumPiMinus; break;
      case  111: nf = gtruth.fNumPi0; break;
      default:
        std::cerr << "IMult::Fill: Unknown PDG " << pdg << std::endl;
        assert(false);
        break;
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

  #ifndef __NO_LARSOFT__
  void PPiLead::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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

  #ifndef __NO_LARSOFT__
  void ThetaPiLead::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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

  #ifndef __NO_LARSOFT__
  void ThetaLepPiLead::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
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

  #ifndef __NO_LARSOFT__
  void ECons::Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w) {
    float pmass = TDatabasePDG::Instance()->GetParticle(2212)->Mass();
    float nmass = TDatabasePDG::Instance()->GetParticle(2112)->Mass();

    //// Initial state
    const simb::MCNeutrino& nu = truth.GetNeutrino();

    // Neutrino energy
    float enu = nu.Nu().E();

    // Target nucleus rest mass
    float tgtmass;
    TParticlePDG* tgtparticle = TDatabasePDG::Instance()->GetParticle(gtruth.ftgtPDG);
    if (tgtparticle) {
      tgtmass = tgtparticle->Mass();
    }
    else {
      tgtmass = gtruth.ftgtZ * pmass + (gtruth.ftgtA - gtruth.ftgtZ) * nmass;
    }

    // Struck nucleon KE
    float nucmass = TDatabasePDG::Instance()->GetParticle(nu.HitNuc())->Mass();
    float enuc = gtruth.fHitNucP4.E() - nucmass;

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
