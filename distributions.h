#ifndef __DISTRIBUTIONS__
#define __DISTRIBUTIONS__

/**
 * Kinematic distributions.
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/19
 */

#include <string>
#include "NuisTree.h"

class Filter;
class TCanvas;
class TH1;

/**
 * \class Distribution
 * \brief A kinematic distribution (base class).
 *
 * \param _name A string name
 * \param _filter The event filter to apply before filling the distribution
 */
struct Distribution {
  /** Constructor. */
  Distribution(std::string _name, Filter* _filter)
      : filter(_filter), name(_name) {}

  /** Constructor. */
  Distribution(std::string _name, std::string _title,
               TH1* _hist, Filter* _filter);

  /** Fill the distribution histogram */
  virtual void Fill(const NuisTree& nuistr) = 0;

  /** Write to a ROOT file. */
  void Write();

  /** Plot and save to a PDF. */
  void Save(TCanvas* c=NULL);

  TH1* hist;  //!< A generic ROOT histogram
  Filter* filter;  //!< The event filter function
  std::string name;  //!< Distribution name
  std::string title;  //!< Distribution ROOT/LaTeX title
};


namespace distributions {

  // Enu_true
  struct Enu_true : public Distribution {
	  Enu_true(std::string _name, Filter* _filter);
	  void Fill(const NuisTree& nuistr);
  };

  // q0
  struct q0 : public Distribution {
	  q0(std::string _name, Filter* _filter);
	  void Fill(const NuisTree& nuistr);
  };

  // q3
  struct q3 : public Distribution {
	  q3(std::string _name, Filter* _filter);
	  void Fill(const NuisTree& nuistr);
  };

  /** Theorists' nu distribution, nu = (p.q)/(sqrt(p^2)) (where p is the 4-momentum of the initial nucleon and q is 4-momentum pnu-plep) */
  struct TheoristsNu : public Distribution {
    TheoristsNu(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** Q2 */
  struct Q2 : public Distribution {
    Q2(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** Theorists' W distribution, W = sqrt(p'^2) = sqrt(p^2 + 2p.q - Q2) (where p is the 4-momentum of the initial nucleon, p' is the 4-momentum of the outgoing nucleon, q is 4-momentum pnu-plep, and Q2 = -q^2) */
  struct TheoristsW : public Distribution {
    TheoristsW(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** Experimentalists' W distribution, W = sqrt(M^2 + 2*M*q0 - Q^2) (where M is the mass of the hit nucleon, q0 = Enu-Elep, q is the 4-momentum pnu-plep, and Q^2 = -q.q) */
  struct ExperimentalistsW : public Distribution {
    ExperimentalistsW(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** Theorist's Bjorken x distribution, x = Q^2/(2p.q) (where p is the 4-momentum of the initial nucleon and q is 4-momentum pnu-plep) */
  struct TheoristsBjorkenX : public Distribution {
    TheoristsBjorkenX(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };


  /** Experimentalists' Bjorken x distribution, x = Q^2/(2M*(Enu-Elep)) */
  struct ExperimentalistsBjorkenX : public Distribution {
    ExperimentalistsBjorkenX(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
 
  /** Theorists' Inelasticity y distribution, y = (p.q)/(p.k) (where p is the 4-momentum of the initial nucleon, q is 4-momentum pnu-plep, and k is the 4-momentum of the neutrino) */
  struct TheoristsInelasticityY : public Distribution {
    TheoristsInelasticityY(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };


  /** Experimentalists' Inelasticity y distribution, y = 1-(Elep/Enu) */
  struct ExperimentalistsInelasticityY : public Distribution {
    ExperimentalistsInelasticityY(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** Experimentalists' Inelasticity y distribution, y = 1-(Elep/Enu) */
  struct RecoInelasticityY : public Distribution {
    RecoInelasticityY(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
  
  /** Lepton momentum distribution */
  struct PLep : public Distribution {
    PLep(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
 
  /** Lepton angle distribution */
  struct ThetaLep : public Distribution {
    ThetaLep(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
 
  /** Relative erec etrue bias  */
  struct ErecRelBias : public Distribution {
    ErecRelBias(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
 
  /** Abs erec etrue bias  */
  struct ErecAbsBias : public Distribution {
    ErecAbsBias(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
 
  /** Abs erec etrue bias  */
  struct Erec : public Distribution {
    Erec(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
 
  /** 1D Emiss **/
  struct Emiss : public Distribution {
	  Emiss(std::string _name, Filter* _filter);
	  void Fill(const NuisTree& nuistr);
  };

  /** 1D Pmiss **/
  struct Pmiss : public Distribution {
	  Pmiss(std::string _name, Filter* _filter);
	  void Fill(const NuisTree& nuistr);
  };
 
  /** Leading proton momentum */
  struct PPLead : public Distribution {
    PPLead(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** Leading proton T */
  struct PLead_KE : public Distribution {
    PLead_KE(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
 
  /** Leading proton costheta */
  struct ThetaPLead : public Distribution {
    ThetaPLead(std::string _name, Filter* _filter, float _ethreshold=0);
    void Fill(const NuisTree& nuistr);
    float ethreshold;  //!< KE threshold (GeV)
  };
 
  /** Leading proton momentum */
  struct PNLead : public Distribution {
    PNLead(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** Leading proton T */
  struct NLead_KE : public Distribution {
    NLead_KE(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
 
  /** Leading proton costheta */
  struct ThetaNLead : public Distribution {
    ThetaNLead(std::string _name, Filter* _filter, float _ethreshold=0);
    void Fill(const NuisTree& nuistr);
    float ethreshold;  //!< KE threshold (GeV)
  };

  /** Leading pion momentum */
  struct PPiLead : public Distribution {
    PPiLead(std::string _name, Filter* _filter, bool _charged=false);
    void Fill(const NuisTree& nuistr);
    bool charged;  //!< Consider only charged pions
  };


  /** Leading pion angle */
  struct ThetaPiLead : public Distribution {
    ThetaPiLead(std::string _name, Filter* _filter, bool _charged=false);
    void Fill(const NuisTree& nuistr);
    bool charged;  //!< Consider only charged pions
  };

  /** Final state particle multiplicity */
  struct Mult : public Distribution {
    Mult(std::string _name, Filter* _filter, int _pdg, float _ethreshold=0);
    void Fill(const NuisTree& nuistr);
    int pdg;  //!< Particle PDG code
    float ethreshold;  //!< KE threshold (GeV)
    float mass;  //!< Particle mass (from PDG database lookup)
  };

  /** TKI dpt */
  struct tki_dpt : public Distribution {
    tki_dpt(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** TKI dat */
  struct tki_dat : public Distribution {
    tki_dat(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
  
  /** TKI dphit */
  struct tki_dphit : public Distribution {
    tki_dphit(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** pmuT */
  struct pmuT : public Distribution {
    pmuT(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
  
  /** pmuL */
  struct pmuL : public Distribution {
    pmuL(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** 2D  Erec - y rec */
  struct ErecYrec : public Distribution {
    ErecYrec(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };  
  
  /** 2D Emiss-Pmiss distribution, the ground state **/
  struct EmissPmiss : public Distribution {
	  EmissPmiss(std::string _name, Filter* _filter);
	  void Fill(const NuisTree& nuistr);
  };

  /** q0/q3 distribution */
  struct Q0Q3 : public Distribution {
    Q0Q3(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };
  
  /** p_l - theta_l distribution */
  struct PThetaLep : public Distribution {
    PThetaLep(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** pmuT - pmuL distribution */
  struct PLep_TL : public Distribution {
    PLep_TL(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** Q2 - W distribution */
  struct Q2W : public Distribution {
    Q2W(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** x - y distribution */
  struct BjorkenX_InelY : public Distribution {
    BjorkenX_InelY(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };

  /** Enu - Erec distribution */
  struct Enu_Erec : public Distribution {
    Enu_Erec(std::string _name, Filter* _filter);
    void Fill(const NuisTree& nuistr);
  };


}  // namespace distributions

#endif  // __DISTRIBUTIONS__
