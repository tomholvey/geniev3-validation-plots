#ifndef __DISTRIBUTIONS__
#define __DISTRIBUTIONS__

/**
 * Kinematic distributions.
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/19
 */

#include <string>
#include "nusimdata/SimulationBase/MCTruth.h"

class Filter;
class TCanvas;
class TH1;

/**
 * A kinematic distribution (base class).
 */
struct Distribution {
  Distribution(std::string _name, Filter* _filter)
      : filter(_filter), name(_name) {}

  Distribution(std::string _name, std::string _title,
               TH1* _hist, Filter* _filter);

  virtual void Fill(const simb::MCTruth& truth, float w=1.0) = 0;

  void Write();

  void Save(TCanvas* c=NULL);

  TH1* hist;
  Filter* filter;
  std::string name;
  std::string title;
};


namespace distributions {

  /** Q2 distribution */
  struct Q2 : public Distribution {
    Q2(std::string _name, Filter* _filter);
    void Fill(const simb::MCTruth& truth, float w=1.0);
  };


  /** W distribution */
  struct W : public Distribution {
    W(std::string _name, Filter* _filter);
    void Fill(const simb::MCTruth& truth, float w=1.0);
  };


  /** Bjorken x = Q^2/(2M*(Enu-Elep)) distribution */
  struct BjorkenX : public Distribution {
    BjorkenX(std::string _name, Filter* _filter);
    void Fill(const simb::MCTruth& truth, float w=1.0);
  };


  /** Inelasticity y = 1-(Elep/Enu) distribution */
  struct InelasticityY : public Distribution {
    InelasticityY(std::string _name, Filter* _filter);
    void Fill(const simb::MCTruth& truth, float w=1.0);
  };


  /** Lepton momentum distribution */
  struct PLep : public Distribution {
    PLep(std::string _name, Filter* _filter);
    void Fill(const simb::MCTruth& truth, float w=1.0);
  };


  /** Lepton angle distribution */
  struct ThetaLep : public Distribution {
    ThetaLep(std::string _name, Filter* _filter);
    void Fill(const simb::MCTruth& truth, float w=1.0);
  };


  /** q0/q3 distribution */
  struct Q0Q3 : public Distribution {
    Q0Q3(std::string _name, Filter* _filter);
    void Fill(const simb::MCTruth& truth, float w=1.0);
  };


  /** Lepton p/theta distribution */
  struct PThetaLep : public Distribution {
    PThetaLep(std::string _name, Filter* _filter);
    void Fill(const simb::MCTruth& truth, float w=1.0);
  };


  /** Leading/subleading proton KE */
  struct Pke : public Distribution {
    Pke(std::string _name, Filter* _filter);
    void Fill(const simb::MCTruth& truth, float w=1.0);
  };


  /** Leading/subleading proton KE */
  struct MultP : public Distribution {
    MultP(std::string _name, Filter* _filter, float ethreshold=0);
    void Fill(const simb::MCTruth& truth, float w=1.0);
    float ethreshold;  //!< KE threshold (GeV)
  };

}  // namespace distributions

#endif  // __DISTRIBUTIONS__

