#ifndef __DISTRIBUTIONS__
#define __DISTRIBUTIONS__

/**
 * Kinematic distributions.
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/19
 */

#include <string>
#include "NuisTree.h"
#ifndef __NO_LARSOFT__
#include "nusimdata/SimulationBase/GTruth.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCNeutrino.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "GENIE/Framework/GHEP/GHepStatus.h"
#endif

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
  #ifndef __NO_LARSOFT__
  virtual void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0) = 0;
  #endif
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

  /** Q2 distribution */
  struct Q2 : public Distribution {
    Q2(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** W distribution */
  struct W : public Distribution {
    W(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** Bjorken x = Q^2/(2M*(Enu-Elep)) distribution */
  struct BjorkenX : public Distribution {
    BjorkenX(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** Inelasticity y = 1-(Elep/Enu) distribution */
  struct InelasticityY : public Distribution {
    InelasticityY(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** Lepton momentum distribution */
  struct PLep : public Distribution {
    PLep(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** Lepton angle distribution */
  struct ThetaLep : public Distribution {
    ThetaLep(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** q0/q3 distribution */
  struct Q0Q3 : public Distribution {
    Q0Q3(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** Lepton p/theta distribution */
  struct PThetaLep : public Distribution {
    PThetaLep(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** Leading proton KE vs. q0 distribution */
  struct LeadPKEQ0 : public Distribution {
    LeadPKEQ0(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** Leading/subleading proton KE */
  struct Pke : public Distribution {
    Pke(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** Leading proton costheta */
  struct ThetaPLead : public Distribution {
    ThetaPLead(std::string _name, Filter* _filter, float _ethreshold=0);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
    float ethreshold;  //!< KE threshold (GeV)
  };


  /** Leading proton momentum */
  struct PPLead : public Distribution {
    PPLead(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };


  /** Leading proton costheta */
  struct ThetaLepPLead : public Distribution {
    ThetaLepPLead(std::string _name, Filter* _filter, float _ethreshold=0);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
    float ethreshold;  //!< KE threshold (GeV)
  };


  /** Final state particle multiplicity */
  struct Mult : public Distribution {
    Mult(std::string _name, Filter* _filter, int _pdg, float _ethreshold=0);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
    int pdg;  //!< Particle PDG code
    float ethreshold;  //!< KE threshold (GeV)
    float mass;  //!< Particle mass (from PDG database lookup)
  };


  /** Intermediate state particle multiplicity */
  struct IMult : public Distribution {
    IMult(std::string _name, Filter* _filter, int _pdg);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
    int pdg;  //!< Particle PDG code
  };


  /** Leading pion momentum */
  struct PPiLead : public Distribution {
    PPiLead(std::string _name, Filter* _filter, bool _charged=false);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
    bool charged;  //!< Consider only charged pions
  };


  /** Leading pion angle */
  struct ThetaPiLead : public Distribution {
    ThetaPiLead(std::string _name, Filter* _filter, bool _charged=false);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
    bool charged;  //!< Consider only charged pions
  };


  /** Leading pion/lepton angle */
  struct ThetaLepPiLead : public Distribution {
    ThetaLepPiLead(std::string _name, Filter* _filter, bool _charged=false);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
    bool charged;  //!< Consider only charged pions
  };


  /** Energy conservation. WIP, may not be possible within LArSoft. */
  struct ECons : public Distribution {
    ECons(std::string _name, Filter* _filter);
    #ifndef __NO_LARSOFT__
    void Fill(const simb::MCTruth& truth, const simb::GTruth& gtruth, float w=1.0);
    #endif
    void Fill(const NuisTree& nuistr);
  };

}  // namespace distributions

#endif  // __DISTRIBUTIONS__
