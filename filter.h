#ifndef __FILTER__
#define __FILTER__

/**
 * An event filter, which decides whether to process an MCTruth
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/19
 */

#include <functional>
#include <string>
#include "nusimdata/SimulationBase/MCTruth.h"

class Filter : public std::unary_function<const simb::MCTruth&, bool> {
public:
  virtual bool operator()(const simb::MCTruth&) = 0;
  std::string title;  //!< ROOT/LaTeX title
};


namespace filters {

  /** Filter for a given neutrino PDG, mode, and CC/NC. */
  class NuMode : public Filter {
  public:
    NuMode(int _pdg, int _mode, int _cc);
    virtual bool operator()(const simb::MCTruth& truth);

    int pdg;
    int mode;
    int cc;
  };

}  // namespace filters

#endif  // __FILTER__

