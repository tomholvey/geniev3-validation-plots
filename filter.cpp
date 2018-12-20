#include <string>
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCNeutrino.h"
#include "filter.h"

namespace filters {

  NuMode::NuMode(int _pdg, int _mode, int _cc)
      : pdg(_pdg), mode(_mode), cc(_cc) {
    std::string nu;
    std::string inttype;

    switch (pdg) {
      case -12: nu = "#bar#nu_{e}"; break;
      case  12: nu = "#nu_{e}"; break;
      case -14: nu = "#bar#nu_{#mu}"; break;
      case  14: nu = "#nu_{#mu}"; break;
      case -16: nu = "#bar#nu_{#tau}"; break;
      case  16: nu = "#nu_{#tau}"; break;
      default: break;
    }

    switch (mode) {
      case simb::kQE: inttype = "QE"; break;
      case simb::kRes: inttype = "Res"; break;
      case simb::kDIS: inttype = "DIS"; break;
      case simb::kCoh: inttype = "Coh"; break;
      case simb::kMEC: inttype = "MEC"; break;
      default: break;
    }

    title = nu + (cc == simb::kCC ? "CC" : "NC") + inttype;    
  }

  bool NuMode::operator()(const simb::MCTruth& truth) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();
    return (nu.Nu().PdgCode() == pdg &&
            nu.CCNC() == cc &&
            nu.Mode() == mode);
  }

}  // namespace filters

