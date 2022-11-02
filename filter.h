#ifndef __FILTER__
#define __FILTER__

/**
 * An event filter, which decides whether to process an MCTruth
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/19
 */

#include <functional>
#include <string>
#include "NuisTree.h"

/**
 * \class Filter
 * \brief Base class for event filters.
 *
 * This is a unary_function that must define an operator() that returns
 * true or false depending on whether an event passes.
 */
class Filter : public std::unary_function<const NuisTree&, bool> {
public:
  /** Decide whether to process the event. */
  virtual bool operator()(const NuisTree&) = 0;

  /** Convert neutrino type to a string. */
  static std::string GetNuType(const int pdg);

  /** Convert neutrino interaction mode to a string. */
  static std::string GetNuMode(const int mode);

  std::string title;  //!< ROOT/LaTeX title
};

namespace filters {

  /*
   * \class NuMode
   * \brief Filter for a given neutrino PDG, mode, and CC/NC.
   *
   * \param _pdg Neutrino PDG code
   * \param _mode Interaction mode (see enum in simb::MCNeutrino)
   * \param _cc CC/NC flag (i.e. simb::kCC or simb::kNC)
   */
  class NuMode : public Filter {
  public:
    NuMode(int _pdg, int _cc, int _mode);
    virtual bool operator()(const NuisTree& nuistr);

    int pdg;  //!< Neutrino PDG code
    int mode;  //!< Interaction mode
    int cc;  //!< CC/NC flag
  };


  /**
   * \class CC1Pi
   * \brief Filter for true CC1pi.
   *
   * \param _pdg Neutrino PDG code
   * \param _charged Require exactly one charged pion, and no neutrals
   */
  class CC1Pi : public Filter {
  public:
    CC1Pi(int _pdg, bool _charged=false);
    virtual bool operator()(const NuisTree& nuistr);

    int pdg;  //!< Neutrino PDG code
    bool charged;  //!< Require one charged pion, no neutrals
  };

}  // namespace filters

#endif  // __FILTER__
