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

	// 1D Distributions
	// Enu_true ------------------------------------------------------------------------------------------------------------------//
	Enu_true::Enu_true(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
		title = std::string("True Neutrino Energy, ") + _filter->title;
		std::string hname = "h1D_Enu_" + name;
		hist = new TH1F(hname.c_str(),
				(title + "; E_{#nu}^{true} (GeV); Events/tonne/year").c_str(),
				50, 0., 5.);
	}

	void Enu_true::Fill(const NuisTree& nuistr){
		dynamic_cast<TH1F*>(hist)->Fill(nuistr.Enu_true, nuistr.Weight*(nuistr.fScaleFactor*1E38));
	}

	// Theorists nu/q0 -----------------------------------------------------------------------------------------------------------//
	TheoristsNu::TheoristsNu(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
		title = std::string("Theorists nu = p.q/sqrt(p^2), ") + _filter->title;
		std::string hname = "hthnu_" + name;
		hist = new TH1F(hname.c_str(),
				(title + ";Theorists nu = p.q/sqrt(p^2);Events/tonne/year").c_str(),
				20, 0, 1);
	}

	void TheoristsNu::Fill(const NuisTree& nuistr) {
		// Find neutrino and nucleon in list of initial state particles
		int i_nuc = -999;
		int i_nu = -999;
		for (int i=0; i<nuistr.ninitp; i++) {
			if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112 ||  (nuistr.Mode == 36 && nuistr.initp_pdg[i] > 999999999)){ 
				// check this is the only initial nucleon we've found
				assert(i_nuc==-999);
				i_nuc = i;
			}
			// Or NC 2p2h 
			else if (nuistr.Mode == 53) {
				assert(i_nuc = -999);
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

		if (i_nuc == -999){
			return;
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

		dynamic_cast<TH1F*>(hist)->Fill(nu_theorist, nuistr.Weight*(nuistr.fScaleFactor*1E38));
	}

	// q0/Experimentalists nu ----------------------------------------------------------------------------------//
	q0::q0(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
		title = std::string("q^{0}, ") + _filter->title;
		std::string hname = "h1D_q0_" + name;
		hist = new TH1F(hname.c_str(),
				(title + " ; q^{0} (GeV); Events/tonne/year").c_str(),
				50, 0., 1.5);
	}

	void q0::Fill(const NuisTree& nuistr){
		dynamic_cast<TH1F*>(hist)->Fill(nuistr.q0, nuistr.Weight*(nuistr.fScaleFactor*1E38));
	}

	// q0 reco --------------------------------------------------------------------------------------------------//
	q0Reco::q0Reco(std::string _name, Filter* _filter) : Distribution(_name, _filter){
		title = std::string("Reco. q0, ") + _filter->title;
		std::string hname = "h1D_q0Reco_" + name;
		hist = new TH1F(hname.c_str(),
						(title + "; Reco. q^{0} (GeV); Events/tonne/year").c_str(),
						50, 0, 1.5);
	}

	void q0Reco::Fill(const NuisTree& nuistr){
		float q0Reco = -1* (nuistr.ELep - (nuistr.ELep + nuistr.Erecoil_minerva));
		dynamic_cast<TH1F*>(hist)->Fill(q0Reco, nuistr.Weight*(nuistr.fScaleFactor*1E38));
	}

	// q3 -----------------------------------------------------------------------------------------------------//
	q3::q3(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
		title = std::string("q3, ") + _filter->title;
		std::string hname = "h1D_q3_" + name;
		hist = new TH1F(hname.c_str(),
				(title + " ; q^{3} (GeV); Events/tonne/year").c_str(),
				30, 0., 3.);
	}

	void q3::Fill(const NuisTree& nuistr){
		dynamic_cast<TH1F*>(hist)->Fill(nuistr.q3, nuistr.Weight*(nuistr.fScaleFactor*1E38));
	}

	// q3 reco --------------------------------------------------------------------------------------------------//
	q3Reco::q3Reco(std::string _name, Filter* _filter) : Distribution(_name, _filter){
		title = std::string("Reco. q3, ") + _filter->title;
		std::string hname = "h1D_q3Reco_" + name;
		hist = new TH1F(hname.c_str(),
						(title + "; Reco. q^{3} (GeV); Events/tonne/year").c_str(),
						30, 0., 3.);
	}

	void q3Reco::Fill(const NuisTree& nuistr){
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
		float Erec = nuistr.ELep + nuistr.Erecoil_minerva;

		float q3_reco = TMath::Sqrt(p*p + Erec*Erec - p*nuistr.CosLep);

		dynamic_cast<TH1F*>(hist)->Fill(q3_reco, nuistr.Weight*(nuistr.fScaleFactor*1E38));
	}


	// Q2 -------------------------------------------------------------------------------------------------------//
	Q2::Q2(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
		title = std::string("Q^{2}, ") + _filter->title;
		std::string hname = "h1D_Q2_" + name;
		hist = new TH1F(hname.c_str(),
				(title + ";Q^{2} (GeV^{2});Events/tonne/year").c_str(),
				30, 0, 3);
	}

	void Q2::Fill(const NuisTree& nuistr){
		dynamic_cast<TH1F*>(hist)->Fill(nuistr.Q2,nuistr.Weight*(nuistr.fScaleFactor*1E38));
	}

	// Q2 Reco ----------------------------------------------------------------------------------------------------//
	Q2Reco::Q2Reco(std::string _name, Filter* _filter) : Distribution(_name, _filter){
		title = std::string("Reco. Q^{2}, ") + _filter->title;
		std::string hname = "h1D_RecoQ2_" + name;
		hist = new TH1F(hname.c_str(),
						(title + ";Reco. Q^{2} (GeV^2); Events/tonne/year").c_str(),
						30, 0. ,3);
	}

	void Q2Reco::Fill(const NuisTree& nuistr){
		// Find final-state lepton in stack
		float m_lep = 0;
		int i_lep=-999;
		for (int i=0; i<nuistr.nfsp; i++){
			if (nuistr.fsp_pdg[i]==nuistr.PDGLep && nuistr.fsp_E[i]==nuistr.ELep){
			// check this is the only lepton we've found
				if (i_lep!=-999){
				// previously found another lepton -- error!
				}
			// Set lepton mass (e, mu or tau) 
			if (nuistr.fsp_pdg[i] == 11 || nuistr.fsp_pdg[i] == -11){
				m_lep = 0.000511;
			}
			else if (nuistr.fsp_pdg[i] == 13 || nuistr.fsp_pdg[i] == -13){
				m_lep = 0.10567;
			}
			else if (nuistr.fsp_pdg[i] == 15 || nuistr.fsp_pdg[i] == -15){
				m_lep = 1.7769;
			}
			i_lep = i;
			}
		}

		// Now get lepton momentum from components.
		TVector3 pv(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]);
		float p = pv.Mag();
		
		// Q2_reco = 2*Erec*(E_lep - p_lep*costheta_lep) - m_lep^2 
		float Erec = nuistr.ELep + nuistr.Erecoil_minerva;
		float Q2_reco = 2*Erec*(nuistr.ELep - p*nuistr.CosLep) - m_lep*m_lep;

		dynamic_cast<TH1F*>(hist)->Fill(Q2_reco, nuistr.Weight*(nuistr.fScaleFactor*1E38));
	}

	// Theorists W ---------------------------------------------------------------------------------------------------------//
	TheoristsW::TheoristsW(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
		title = std::string("Theorists W = sqrt(p.p + 2p.q - Q^2), ") + _filter->title;
		std::string hname = "h1D_ThW_" + name;
		hist = new TH1F(hname.c_str(),
				(title + ";Theorists W = sqrt(p.p + 2p.q - Q^2) (GeV);Events/tonne/year").c_str(),
				30, .5, 2);
	}

	void TheoristsW::Fill(const NuisTree& nuistr) {
		// Find neutrino and nucleon in list of initial state particles
		int i_nuc = -999;
		int i_nu = -999;
		for (int i=0; i<nuistr.ninitp; i++) {
			// Look for initial state proton/neutron, nucleus for Coh
			if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112 ||  (nuistr.Mode == 36 && nuistr.initp_pdg[i] > 999999999)){ 
				// check this is the only initial nucleon we've found
				assert(i_nuc==-999);
				i_nuc = i;
			}
			// Or NC 2p2h 
			else if (nuistr.Mode == 53) {
				assert(i_nuc = -999);
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

		if (i_nuc == -999){
			// Should only be for reverse muon decay and elastic electron scattering
			// Very rare!!
			return;
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

		dynamic_cast<TH1F*>(hist)->Fill(w_theorist,nuistr.Weight*(nuistr.fScaleFactor*1E38));
	}

	// Experimentalists W --------------------------------------------------------------------------------------------------//
	ExperimentalistsW::ExperimentalistsW(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
		title = std::string("Experimentalists W = sqrt(M^2 + 2Mq0 - Q^2), ") + _filter->title;
		std::string hname = "h1D_expW_" + name;
		hist = new TH1F(hname.c_str(),
				(title + ";Experimentalists W = sqrt(M^2 + 2Mq0 - Q^2) (GeV);Events/tonne/year").c_str(),
				60, 0.5, 1.5);
	}

	void ExperimentalistsW::Fill(const NuisTree& nuistr) {
		// below what previous DIRT validations did
		//dynamic_cast<TH1F*>(hist)->Fill(nuistr.W_nuc_rest,nuistr.Weight*(nuistr.fScaleFactor*1E38));

    float Q2 = nuistr.Q2;
    float q0 = nuistr.q0;
    float M = 0.93956541; // neutron mass GeV
    float W = TMath::Sqrt(M*M + 2*M*q0 - Q2);
    dynamic_cast<TH1F*>(hist)->Fill(W,nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Theorists Bjorken X --------------------------------------------------------------------------------------------------//
  TheoristsBjorkenX::TheoristsBjorkenX(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Theorists Bjorken x = Q^2/(2p.q), ") + _filter->title;
    std::string hname = "h1D_thbx_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Theorists Bjorken x = Q^2/(2p.q);Events/tonne/year").c_str(),
                    10, 0, 1);
  }

  void TheoristsBjorkenX::Fill(const NuisTree& nuistr) {
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
	  if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112 ||  (nuistr.Mode == 36 && nuistr.initp_pdg[i] > 999999999)){ 
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
	  // Or NC 2p2h 
	  else if (nuistr.Mode == 53) {
		  assert(i_nuc = -999);
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
	
	if (i_nuc == -999){
		return;
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

    dynamic_cast<TH1F*>(hist)->Fill(x, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Experimentalists Bjorken X ----------------------------------------------------------------------------------------------//
  ExperimentalistsBjorkenX::ExperimentalistsBjorkenX(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Experimentalists Bjorken x = Q^2/(2Mq0), ") + _filter->title;
    std::string hname = "h1D_expbx_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Experimentalists Bjorken x = Q^2/(2Mq0);Events/tonne/year").c_str(),
                    15, 0, 1.5);
  }

  void ExperimentalistsBjorkenX::Fill(const NuisTree& nuistr) {
    // what previous DIRT validations did
	//dynamic_cast<TH1F*>(hist)->Fill(nuistr.x, nuistr.Weight*(nuistr.fScaleFactor*1E38));

    float Q2 = nuistr.Q2;
    float q0 = nuistr.q0;
    float M = 0.93956541; // neutron mass GeV                                                                               
    float x = Q2/(2*M*q0);
    dynamic_cast<TH1F*>(hist)->Fill(x,nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Theorists Y ----------------------------------------------------------------------------------------------------------------//
  TheoristsInelasticityY::TheoristsInelasticityY(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Theorists Inelasticity y = (p.q)/(p.k), ") + _filter->title;
    std::string hname = "h1D_ThY_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Theorists Inelasticity y = (p.q)/(p.k);Events/tonne/year").c_str(),
                    20, 0, 1);
  }

  void TheoristsInelasticityY::Fill(const NuisTree& nuistr) {
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
	  if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112 ||  (nuistr.Mode == 36 && nuistr.initp_pdg[i] > 999999999)){ 
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
	  // Or NC 2p2h 
	  else if (nuistr.Mode == 53) {
		  assert(i_nuc = -999);
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
	
	if (i_nuc == -999){
		return;
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

    dynamic_cast<TH1F*>(hist)->Fill(y, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Experimentalists Y ----------------------------------------------------------------------------------------------------------------//
  ExperimentalistsInelasticityY::ExperimentalistsInelasticityY(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Experimentalists Inelasticity y = 1-(Elep/Enu), ") + _filter->title;
    std::string hname = "h1D_expY_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Experimentalists Inelasticity y = 1-(Elep/Enu);Events/tonne/year").c_str(),
                    20, 0, 1);
  }

  void ExperimentalistsInelasticityY::Fill(const NuisTree& nuistr) {
    dynamic_cast<TH1F*>(hist)->Fill(nuistr.y, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Y Reconstructed  ----------------------------------------------------------------------------------------------------------------//
  RecoInelasticityY::RecoInelasticityY(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Reconstructed Inelasticity y, ") + _filter->title;
    std::string hname = "h1D_RecoY_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";y_{reco.};Events/tonne/year").c_str(),
                    20, 0, 1);
  }

  void RecoInelasticityY::Fill(const NuisTree& nuistr) {
    float y_reco = 1. - (nuistr.Erecoil_minerva/(nuistr.Erecoil_minerva + nuistr.ELep));
	dynamic_cast<TH1F*>(hist)->Fill(y_reco, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Lepton momentum -------------------------------------------------------------------------------------------------------------------//
  PLep::PLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("p_{lep}, ") + _filter->title;
    std::string hname = "h1D_plep_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{lep} (GeV);Events/tonne/year").c_str(),
                    20, 0, 2);
  }

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

    dynamic_cast<TH1F*>(hist)->Fill(p,nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Lepton CosTheta -------------------------------------------------------------------------------------------------------------//
  ThetaLep::ThetaLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("cos#theta_{lep}, ") + _filter->title;
    std::string hname = "h1D_cosThetaLep_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{lep};Events/tonne/year").c_str(),
                    25, 0, 1);
  }

  void ThetaLep::Fill(const NuisTree& nuistr) {
    //dynamic_cast<TH1F*>(hist)->Fill(nuistr.CosLep,nuistr.Weight*(nuistr.fScaleFactor*1E38));

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
    dynamic_cast<TH1F*>(hist)->Fill(costh,nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Relative Erec Bias ---------------------------------------------------------------------------------------------------------//
  ErecRelBias::ErecRelBias(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
	  title = std::string("Relative Reco. E_{#nu} Bias, ") + _filter->title;
	  std::string hname = "h1D_ErecRelBias_" + name;
	  hist = new TH1F(hname.c_str(),
					  (title + "; Relative Reco. E_{#nu} Bias; Events/tonne/year").c_str(),
					  100, -0.2, 0.2);
  }

  void ErecRelBias::Fill(const NuisTree& nuistr){
	  float RelBias = ((nuistr.Erecoil_minerva + nuistr.ELep)/nuistr.Enu_true) -1;
	  dynamic_cast<TH1F*>(hist)->Fill(RelBias, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Absolute Erec Bias ---------------------------------------------------------------------------------------------------------//
  ErecAbsBias::ErecAbsBias(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
	  title = std::string("Absolute Reco. E_{#nu} Bias, ") + _filter->title;
	  std::string hname = "h1D_ErecAbsBias_" + name;
	  hist = new TH1F(hname.c_str(),
					  (title + "; Absolute Reco. E_{#nu} Bias (GeV); Events/tonne/year").c_str(),
					  40, -0.2, 0.2);
  }

  void ErecAbsBias::Fill(const NuisTree& nuistr){
	  float AbsBias = (nuistr.Erecoil_minerva + nuistr.ELep) - nuistr.Enu_true;
	  dynamic_cast<TH1F*>(hist)->Fill(AbsBias, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Erec ----------------------------------------------------------------------------------------------------------------------//
  Erec::Erec(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
	  title = std::string("Reconstructed Neutrino Energy, ") + _filter->title;
	  std::string hname = "h1D_Erec_" + name;
	  hist = new TH1F(hname.c_str(),
					  (title + "; E_{rec} (GeV); Events/tonne/year").c_str(),
					  50, 0., 5.);
  }

  void Erec::Fill(const NuisTree& nuistr){
	  float Erec = nuistr.Erecoil_minerva + nuistr.ELep;
	  dynamic_cast<TH1F*>(hist)->Fill(Erec, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // 1D Emiss ----------------------------------------------------------------------------------------------------------------//
  Emiss::Emiss(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
	  title = std::string("E_{miss}, ") + _filter->title;
	  std::string hname = "h1D_Emiss_" + name;
	  hist = new TH1F(hname.c_str(),
					  (title + " ; E_{miss} (GeV); Events/tonne/year").c_str(),
					   40, 0.02, 0.07);
  }

  void Emiss::Fill(const NuisTree& nuistr){
	  //std::cout << "Filling Emiss with : " << nuistr.Emiss << std::endl;
	  dynamic_cast<TH1F*>(hist)->Fill(nuistr.Emiss, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // 1D Pmiss -----------------------------------------------------------------------------------------------------------//
  Pmiss::Pmiss(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
	  title = std::string("p_{miss}, ") + _filter->title;
	  std::string hname = "h1D_pmiss_" + name;
	  hist = new TH1F(hname.c_str(),
					  (title + " ; p_{miss} (GeV); Events/tonne/year").c_str(),
						40, 0, 0.4);
  }

  void Pmiss::Fill(const NuisTree& nuistr){
	  //std::cout << "Filling Pmiss with : " << nuistr.pmiss->Mag() << std::endl;
	  dynamic_cast<TH1F*>(hist)->Fill(nuistr.pmiss->Mag(), nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Leading Proton Momentum -----------------------------------------------------------------------------//
  PPLead::PPLead(std::string _name, Filter* _filter)
      : Distribution(_name, _filter) {
    title = std::string("Leading proton momentum, ") + _filter->title;
    std::string hname = "h1D_LeadP_p_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{p} (GeV);Events/tonne/year").c_str(),
                    20, 0, 2);
  }

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
      dynamic_cast<TH1F*>(hist)->Fill(plead, nuistr.Weight*(nuistr.fScaleFactor*1E38));
    }
  }

  // Leading Proton Kinetic Energy -----------------------------------------------------------------------//
  PLead_KE::PLead_KE(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Proton KE, ") + _filter->title;
    std::string hname = "h1D_LeadP_T_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Leading proton KE T_{p} (GeV);Events/tonne/year").c_str(),
                    30, 0, 1);
  }

  void PLead_KE::Fill(const NuisTree& nuistr) { 
	// Loop through final-state particles and find leading proton (defined by highest KE)
    float Tlead = 0;
    int nprot = 0;
    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i] == 2212){
        nprot++;
        float T = nuistr.fsp_E[i] - 0.938;
        if (T > Tlead){
          Tlead = T;
        }
      }
    }

    if (nprot>0){
      dynamic_cast<TH1F*>(hist)->Fill(Tlead, nuistr.Weight*(nuistr.fScaleFactor*1E38));
    }
  }

  // Leading Proton CosTheta --------------------------------------------------------------------------------------------//
  ThetaPLead::ThetaPLead(std::string _name, Filter* _filter, float _ethreshold)
      : Distribution(_name, _filter), ethreshold(_ethreshold) {
    title = std::string("Leading p, cos#theta_{p}, ") + _filter->title;
    if (ethreshold > 0) title += std::string(", KE_{p} > ") + std::to_string(ethreshold*1000) + std::string(" MeV");
    std::string hname = "h1D_PLead_CosTheta_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{p};Events/tonne/year").c_str(),
                    50, -1, 1);
  }

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
        dynamic_cast<TH1F*>(hist)->Fill(ctlead, nuistr.Weight*(nuistr.fScaleFactor*1E38));
      }
  }

  // Leading Neutron Momentum -----------------------------------------------------------------------------//
  PNLead::PNLead(std::string _name, Filter* _filter)
      : Distribution(_name, _filter) {
    title = std::string("Leading neutron momentum, ") + _filter->title;
    std::string hname = "h1D_LeadN_p_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{n} (GeV);Events/tonne/year").c_str(),
                    20, 0, 2);
  }

  void PNLead::Fill(const NuisTree& nuistr) {

    // Loop through final-state particles and find leading neutron (defined by highest KE)
    float plead = 0;
    int nneut = 0;
    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i] == 2112){
        nneut++;
        TVector3 pv(nuistr.fsp_px[i],nuistr.fsp_py[i],nuistr.fsp_pz[i]);
        float p = pv.Mag();
        if (p > plead){
          plead = p;
        }
      }
    }

    if (nneut>0){
      dynamic_cast<TH1F*>(hist)->Fill(plead, nuistr.Weight*(nuistr.fScaleFactor*1E38));
    }
  }

  // Leading Neutron Kinetic Energy -----------------------------------------------------------------------//
  NLead_KE::NLead_KE(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Neutron KE, ") + _filter->title;
    std::string hname = "h1D_LeadN_T_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";Leading neutron KE T_{n} (GeV);Events/tonne/year").c_str(),
                    30, 0, 1);
  }

  void NLead_KE::Fill(const NuisTree& nuistr) {
    
	// Loop through final-state particles and find leading neutron (defined by highest KE)
    float Tlead = 0;
    int nneut = 0;
    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i] == 2112){
        nneut++;
        float T = nuistr.fsp_E[i] - 0.938;
        if (T > Tlead){
          Tlead = T;
        }
      }
    }

    if (nneut>0){
      dynamic_cast<TH1F*>(hist)->Fill(Tlead, nuistr.Weight*(nuistr.fScaleFactor*1E38));
    }
  }

  // Leading Neutron CosTheta --------------------------------------------------------------------------------------------//
  ThetaNLead::ThetaNLead(std::string _name, Filter* _filter, float _ethreshold)
      : Distribution(_name, _filter), ethreshold(_ethreshold) {
    title = std::string("Leading n, cos#theta_{n}, ") + _filter->title;
    if (ethreshold > 0) title += std::string(", KE_{n} > ") + std::to_string(ethreshold*1000) + std::string(" MeV");
    std::string hname = "h1D_NLead_CosTheta_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{n};Events/tonne/year").c_str(),
                    50, -1, 1);
  }

  void ThetaNLead::Fill(const NuisTree& nuistr) {

      size_t nn = 0;
      float nlead = 0;
      float ctlead = 0;

      for (int i=0; i<nuistr.nfsp; i++) {

        if (nuistr.fsp_pdg[i] == 2112 && (nuistr.fsp_E[i] - 0.938272) > ethreshold) {
          nn++;
          TVector3 pv(nuistr.fsp_px[i],nuistr.fsp_py[i],nuistr.fsp_pz[i]);
          float p = pv.Mag();
          if (p > nlead) {
            nlead = p;
            ctlead = cos(pv.Theta());
          }
        }
      }

      if (nn > 0) {
        dynamic_cast<TH1F*>(hist)->Fill(ctlead, nuistr.Weight*(nuistr.fScaleFactor*1E38));
      }
  }

  // Leading Pion Momentum -------------------------------------------------------------------------//
  PPiLead::PPiLead(std::string _name, Filter* _filter, bool _charged)
      : Distribution(_name, _filter), charged(_charged) {
    title = std::string("Leading #pi, p_{#pi") + (charged ? "#pm" : "") + "}, " + _filter->title;
    std::string hname = "h1D_LeadPi_p_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";p_{#pi} (GeV);Events/tonne/year").c_str(),
                    30, 0., 3.);
  }

  void PPiLead::Fill(const NuisTree& nuistr) {

    // Loop through final-state particles and find leading pion (defined by highest KE)
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
	std::cout << "Pion momentum = " << plead << std::endl;
    dynamic_cast<TH1F*>(hist)->Fill(plead, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }
 
  // Leading Pion CosTheta ----------------------------------------------------------------------------//
  ThetaPiLead::ThetaPiLead(std::string _name, Filter* _filter, bool _charged)
      : Distribution(_name, _filter), charged(_charged) {
    title = std::string("Leading #pi, cos#theta_{#pi") + (charged ? "#pm" : "") + "}, " + _filter->title;
    std::string hname = "h1D_LeadPi_CosTheta_" + name;
    hist = new TH1F(hname.c_str(),
                    (title + ";cos#theta_{#pi};Events/tonne/year").c_str(),
                    50, -1, 1);
  }

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
        dynamic_cast<TH1F*>(hist)->Fill(ctlead, nuistr.Weight*(nuistr.fScaleFactor*1E38));
      }
  }

  // -------------------------------------------- Multiplities ----------------------------------------//
  Mult::Mult(std::string _name, Filter* _filter, int _pdg, float _ethreshold)
      : Distribution(_name, _filter), pdg(_pdg), ethreshold(_ethreshold) {
    char spdg[100];
    snprintf(spdg, 100, "%i", pdg);
    title = std::string("Multiplicity, PDG ") + spdg + ", " + _filter->title;
    std::string hname = std::string("h1D_mult_") + spdg + "_" + name;
    hist = new TH1F(hname.c_str(), (title + ";N_{" + spdg + "}").c_str(), 15, 0, 15);
  }

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

    dynamic_cast<TH1F*>(hist)->Fill(nf, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  Mult_Nucl::Mult_Nucl(std::string _name, Filter* _filter, float _ethreshold)
      : Distribution(_name, _filter), ethreshold(_ethreshold) {
    title = std::string("Multiplicity, Nucleons ") + _filter->title;
    std::string hname = std::string("h1D_multNucleons_") + name;
    hist = new TH1F(hname.c_str(), (title + ";No. of Nucleons; Events/tonne/year").c_str(), 15, 0, 15);
  }

  void Mult_Nucl::Fill(const NuisTree& nuistr) {
    size_t nf = 0;

    for (int i=0; i<nuistr.nfsp; i++){
      if (nuistr.fsp_pdg[i] == 2112 && (nuistr.fsp_E[i] - 0.939565) > ethreshold){
        nf++;
      }
      else if (nuistr.fsp_pdg[i] == 2212 && (nuistr.fsp_E[i] - 0.938272) > ethreshold){
        nf++;
      }
    }

    dynamic_cast<TH1F*>(hist)->Fill(nf, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }
  
  // TKI variables ------------------------------------------------------------------------------------------------------//
  tki_dpt::tki_dpt(std::string _name, Filter* _filter) : Distribution(_name, _filter){
	  title = std::string("dp_{T}, ") + _filter->title;
	  std::string hname = "h1D_dpt_" + name;
      hist = new TH1F(hname.c_str(),
                      (title + ";dp_{T} (GeV);Events/tonne/year").c_str(),
                      60, 0,0.5);
  }

  void tki_dpt::Fill(const NuisTree& nuistr){ // dpt
	  dynamic_cast<TH1F*>(hist)->Fill(nuistr.dpt, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  tki_dat::tki_dat(std::string _name, Filter* _filter) : Distribution(_name, _filter){
	  title = std::string("d#alpha_{T}, ") + _filter->title;
	  std::string hname = "h1D_dat_" + name;
      hist = new TH1F(hname.c_str(),
                      (title + ";d#alpha_{T};Events/tonne/year").c_str(),
                      30, 0, 1.*TMath::Pi());
  }

  void tki_dat::Fill(const NuisTree& nuistr){ // dat
	  dynamic_cast<TH1F*>(hist)->Fill(nuistr.dalphat, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  tki_dphit::tki_dphit(std::string _name, Filter* _filter) : Distribution(_name, _filter){
	  title = std::string("d#phi_{T}, ") + _filter->title;
	  std::string hname = "h1D_dphit_" + name;
	  hist = new TH1F(hname.c_str(),
					  (title + ";d#phi_{T};Events/tonne/year").c_str(),
					  20, 0, .5*TMath::Pi());
  }

  void tki_dphit::Fill(const NuisTree& nuistr){ // dphi_T
	  dynamic_cast<TH1F*>(hist)->Fill(nuistr.dphit, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // MINERvA Variables ----------------------------------------------------------------------------------------------------// 
  // Transverse leptom momentum wrt neutrino direction 
  pmuT::pmuT(std::string _name, Filter* _filter) : Distribution(_name, _filter){
	  title = std::string("p_{l}^{T} wrt #nu direction, ") + _filter->title;
	  std::string hname = "h1D_pmuT_" + name;
	  hist = new TH1F(hname.c_str(),
					  (title + ";p_{l}^{T} (GeV);Events/tonne/year").c_str(),
					  20, 0, 2.);
  }

  void pmuT::Fill(const NuisTree& nuistr){
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
	  if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112 ||  (nuistr.Mode == 36 && nuistr.initp_pdg[i] > 999999999)){ 
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
	  // Or NC 2p2h 
	  else if (nuistr.Mode == 53) {
		  assert(i_nuc = -999);
		  i_nuc = i;
	  }
      if (nuistr.initp_pdg[i]==nuistr.PDGnu){
        // check this is the only neutrino we've found
        assert(i_nu==-999);
        i_nu = i;
      }
    }	
	
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
    TVector3 pl(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]);
	// Now get neutrino direction vector
	TVector3 pv(nuistr.initp_px[i_nu],nuistr.initp_py[i_nu],nuistr.initp_pz[i_nu]);
	pv = (1/pv.Mag()) * pv;
	
	// Get transvere component
	float pmuT = pl.Perp(pv);
	dynamic_cast<TH1F*>(hist)->Fill(pmuT, nuistr.Weight*(nuistr.fScaleFactor*1E38));		
  }

  pmuL::pmuL(std::string _name, Filter* _filter) : Distribution(_name, _filter){
	  title = std::string("p_{l}^{L} wrt #nu direction, ") + _filter->title;
	  std::string hname = "h1D_pmuL_" + name;
	  hist = new TH1F(hname.c_str(),
					  (title + ";p_{l}^{L} (GeV);Events/tonne/year").c_str(),
					  50, 0, 5.);
  }
  
  void pmuL::Fill(const NuisTree& nuistr){
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
	  if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112 ||  (nuistr.Mode == 36 && nuistr.initp_pdg[i] > 999999999)){ 
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
	  // Or NC 2p2h 
	  else if (nuistr.Mode == 53) {
		  assert(i_nuc = -999);
		  i_nuc = i;
	  }
      if (nuistr.initp_pdg[i]==nuistr.PDGnu){
        // check this is the only neutrino we've found
        assert(i_nu==-999);
        i_nu = i;
      }
    }	
	
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
    TVector3 pl(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]);
	// Now get neutrino direction vector
	TVector3 pv(nuistr.initp_px[i_nu],nuistr.initp_py[i_nu],nuistr.initp_pz[i_nu]);
	pv = (1/pv.Mag()) * pv;
	
	// Get transvere component
	float pmuL = pl.Dot(pv);
	dynamic_cast<TH1F*>(hist)->Fill(pmuL, nuistr.Weight*(nuistr.fScaleFactor*1E38));		
  }

  // 2D Distributions
  // Erec-yrec --------------------------------------------------------------------------------------------------------->
  ErecYrec::ErecYrec(std::string _name, Filter* _filter) : Distribution(_name, _filter){
	  title = std::string("E_{rec} - y_{rec}") + _filter->title;
	  std::string hname = "h2D_ErecYrec_" + name;
	  hist = new TH2F(hname.c_str(),
					  (title + "; E_{rec} (GeV); y_{rec}; Events/tonne/year").c_str(),
					  60, 0., 5., 20, 0., 1.);
  }

  void ErecYrec::Fill(const NuisTree& nuistr){
	  float erec = nuistr.Erecoil_minerva + nuistr.ELep;
	  dynamic_cast<TH2F*>(hist)->Fill(erec, nuistr.y, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // 2D Emiss-Pmiss ---------------------------------------------------------------------------------------------------->
  EmissPmiss::EmissPmiss(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
	  title = std::string("E_{miss} - p_{miss}, ") + _filter->title;
	  std::string hname = "hEmissPmiss_" + name;
	  hist = new TH2F(hname.c_str(),
					  (title + "; p_{miss} (GeV); E_{miss} (GeV); Events/tonne/year").c_str(),
					  40, 0., 0.4, 40, 0.02, 0.07);
  }

  void EmissPmiss::Fill(const NuisTree& nuistr){
	  dynamic_cast<TH2F*>(hist)->Fill(nuistr.pmiss->Mag(), nuistr.Emiss, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // q0-q3 ------------------------------------------------------------------------------------------------------------->
  Q0Q3::Q0Q3(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
	  title = std::string("q^{0} - q^{3}, ") + _filter->title;
	  std::string hname = "h2D_q0q3_" + name;
	  hist = new TH2F(hname.c_str(),
                    (title + ";q^{0} (GeV);q^{3} (GeV);Events/tonne/year").c_str(),
                    48, 0, 1.2, 48, 0, 1.2);
  }

  void Q0Q3::Fill(const NuisTree& nuistr) {
	  dynamic_cast<TH2F*>(hist)->Fill(nuistr.q0, nuistr.q3, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // plep - cosTheta_lep ------------------------------------------------------------------------------------------------>
  PThetaLep::PThetaLep(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Lepton p - Cos#theta, ") + _filter->title;
    std::string hname = "h2D_plep_thetalep_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + ";p_{lep} (GeV);cos#theta_{lep};Events/tonne/year").c_str(),
                    20, 0, 2, 25, 0, 1);
  }

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

    dynamic_cast<TH2F*>(hist)->Fill(p, nuistr.CosLep, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // plepT - plepL -------------------------------------------------------------------------------------------------------->
  PLep_TL::PLep_TL(std::string _name, Filter* _filter) : Distribution(_name, _filter){
	  title = std::string("p_{lep}^{T} - p_{lep}^{L}, wrt #nu direction") + _filter->title;
	  std::string hname = "h2D_plepT_plepL_" + name;
	  hist = new TH2F(hname.c_str(),
	                  (title + "; p_{lep}^{L} (GeV);p_{lep}^{T} (GeV);Events/tonne/year").c_str(),
					  50, 0., 5, 40, 0., 1.5);
  }

  void PLep_TL::Fill(const NuisTree& nuistr){
    // Find neutrino and nucleon in list of initial state particles
    int i_nuc = -999;
    int i_nu = -999;
    for (int i=0; i<nuistr.ninitp; i++) {
	  if (nuistr.initp_pdg[i]==2212 || nuistr.initp_pdg[i]==2112 ||  (nuistr.Mode == 36 && nuistr.initp_pdg[i] > 999999999)){ 
        // check this is the only initial nucleon we've found
        assert(i_nuc==-999);
        i_nuc = i;
      }
	  // Or NC 2p2h 
	  else if (nuistr.Mode == 53) {
		  assert(i_nuc = -999);
		  i_nuc = i;
	  }
      if (nuistr.initp_pdg[i]==nuistr.PDGnu){
        // check this is the only neutrino we've found
        assert(i_nu==-999);
        i_nu = i;
      }
    }	
	
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
    TVector3 pl(nuistr.fsp_px[i_lep],nuistr.fsp_py[i_lep],nuistr.fsp_pz[i_lep]);
	// Now get neutrino direction vector
	TVector3 pv(nuistr.initp_px[i_nu],nuistr.initp_py[i_nu],nuistr.initp_pz[i_nu]);
	pv = (1/pv.Mag()) * pv;
	
	// Get transvere component
	float pmuL = pl.Dot(pv);
	float pmuT = pl.Perp(pv);

    dynamic_cast<TH2F*>(hist)->Fill(pmuL, pmuT, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Q^2 - W --------------------------------------------------------------------------------------------------------------->
  Q2W::Q2W(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("Q^2 - W, ") + _filter->title;
    std::string hname = "h2D_Q2_W_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + "; W (GeV); Q^2 (GeV^2) ;Events/tonne/year").c_str(),
                    50, 0.5, 1.5, 40, 0.5, 3);
  }

  void Q2W::Fill(const NuisTree& nuistr){
    dynamic_cast<TH2F*>(hist)->Fill(nuistr.W_nuc_rest, nuistr.Q2, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // BjorkenX - Inel Y ----------------------------------------------------------------------------------------------------->
  BjorkenX_InelY::BjorkenX_InelY(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
    title = std::string("x - y, ") + _filter->title;
    std::string hname = "h2D_x_y_" + name;
    hist = new TH2F(hname.c_str(),
                    (title + "; Bjorken x; Inel. y ;Events/tonne/year").c_str(),
                    50, 0, 1, 50, 0., 1);
  }

  void BjorkenX_InelY::Fill(const NuisTree& nuistr){
    dynamic_cast<TH2F*>(hist)->Fill(nuistr.x, nuistr.y, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  // Enu - Erec ------------------------------------------------------------------------------------------------------------>
  Enu_Erec::Enu_Erec(std::string _name, Filter* _filter) : Distribution(_name, _filter) {
	  title = std::string("E_{#nu}^{true} - E_{reco.}, ") + _filter->title;
	  std::string hname = "h2D_etru_erec_" + name;
	  hist = new TH2F(hname.c_str(),
					  (title + " ; E_{#nu}; E_{reco.}; Events/tonne/year").c_str(),
					  50, 0, 5., 50, 0., 5.);
  }

  void Enu_Erec::Fill(const NuisTree& nuistr){
	  float erec = nuistr.Erecoil_minerva + nuistr.ELep;
	  dynamic_cast<TH2F*>(hist)->Fill(nuistr.Enu_true, erec, nuistr.Weight*(nuistr.fScaleFactor*1E38));
  }

  

}  // namespace distributions
