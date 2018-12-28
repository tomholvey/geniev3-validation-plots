#include <iostream>
#include <string>
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCNeutrino.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "GENIE/Framework/GHEP/GHepStatus.h"
#include "distributions.h"
#include "filter.h"

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

  void Q2::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().QSqr(), w);
  }


  W::W(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("W, ") + _filter->title;
    std::string hname = "hw_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";W (GeV);Events").c_str(),
                    20, 0, 2);
  }

  void W::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().W(), w);
  }


  BjorkenX::BjorkenX(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Bjorken x, ") + _filter->title;
    std::string hname = "hbx_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Bjorken x;Events").c_str(),
                    30, 0, 3);
  }

  void BjorkenX::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().X(), w);
  }


  InelasticityY::InelasticityY(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Inelasticity y, ") + _filter->title;
    std::string hname = "hinely_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Inelasticity y;Events").c_str(),
                    20, 0, 1);
  }

  void InelasticityY::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().Y(), w);
  }


  PLep::PLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("p_{lep}, ") + _filter->title;
    std::string hname = "hpl_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{lep} (GeV);Events").c_str(),
                    20, 0, 2);
  }

  void PLep::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(truth.GetNeutrino().Lepton().P(), w);
  }


  ThetaLep::ThetaLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("cos#theta_{lep}, ") + _filter->title;
    std::string hname = "htl_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{lep};Events").c_str(),
                    50, -1, 1);
  }

  void ThetaLep::Fill(const simb::MCTruth& truth, float w) {
    dynamic_cast<TH1F*>(hist)->Fill(cos(truth.GetNeutrino().Lepton().Momentum().Theta()), w);
  }


  Q0Q3::Q0Q3(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("q^{0}/q^{3}, ") + _filter->title;
    std::string hname = "hq0q3_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + ";q^{0} (GeV);q^{3} (GeV);Events").c_str(),
                    48, 0, 1.2, 48, 0, 1.2);
  }

  void Q0Q3::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();
    float q0 = nu.Nu().E() - nu.Lepton().E();
    float q3 = (nu.Nu().Momentum().Vect() - nu.Lepton().Momentum().Vect()).Mag();
    dynamic_cast<TH2F*>(hist)->Fill(q3, q0, w);
  }


  PThetaLep::PThetaLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Lepton p/#theta, ") + _filter->title;
    std::string hname = "hptl_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + ";p_{lep};cos#theta_{lep};Events").c_str(),
                    20, 0, 2, 50, -1, 1);
  }

  void PThetaLep::Fill(const simb::MCTruth& truth, float w) {
    const simb::MCParticle& lep = truth.GetNeutrino().Lepton();
    float p = lep.P();
    float ct = cos(lep.Momentum().Theta());
    dynamic_cast<TH2F*>(hist)->Fill(p, ct, w);
  }


  Pke::Pke(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Proton KE, ") + _filter->title;
    std::string hname = "hpke_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + ";Leading proton KE T_{p1} (GeV);Subleading proton KE T_{p2} (GeV);Events").c_str(),
                    20, 0, 1, 20, 0, 1);
  }

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


  MultP::MultP(std::string _name, Filter* _filter, float _ethreshold)
      : Distribution(_name, _filter), ethreshold(_ethreshold) {
    title = std::string("Proton multiplicity, ") + _filter->title;
    std::string hname = "hmultp_" + name;
    hist = new TH1F(hname.c_str(), (title + ";N_{p}").c_str(), 20, 0, 20);
  }

  void MultP::Fill(const simb::MCTruth& truth, float w) {
    size_t np = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& p = truth.GetParticle(i);
      if (p.PdgCode() == 2212 && p.StatusCode() == genie::kIStStableFinalState && (p.E() - 0.938) > ethreshold) {
        np++;
      }
    }

    dynamic_cast<TH1F*>(hist)->Fill(np, w);
  }


  PPiLead::PPiLead(std::string _name, Filter* _filter, bool _charged)
      : Distribution(_name, _filter), charged(_charged) {
    title = std::string("Leading #pi, p_{#pi") + (charged ? "#pm" : "") + "}, " + _filter->title;
    std::string hname = "hppi_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{#pi} (GeV);Events").c_str(),
                    20, 0, 2);
  }

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


  ThetaPiLead::ThetaPiLead(std::string _name, Filter* _filter, bool _charged)
      : Distribution(_name, _filter), charged(_charged) {
    title = std::string("Leading #pi, cos#theta_{#pi") + (charged ? "#pm" : "") + "}, " + _filter->title;
    std::string hname = "htpi_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{#pi};Events").c_str(),
                    50, -1, 1);
  }

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


  ThetaLepPiLead::ThetaLepPiLead(std::string _name, Filter* _filter, bool _charged)
      : Distribution(_name, _filter), charged(_charged) {
    title = std::string("Leading #pi, cos#theta_{lep,#pi") + (charged ? "#pm" : "") + "}, " + _filter->title;
    std::string hname = "htlp_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{lep,#pi};Events").c_str(),
                    50, -1, 1);
  }

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

}  // namespace distributions

