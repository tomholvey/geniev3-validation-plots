#include <string>
#include "GENIE/Framework/GHEP/GHepStatus.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCNeutrino.h"
#include "filter.h"

std::string Filter::GetNuType(int pdg) {
  std::string nu;

  switch (pdg) {
    case -12: nu = "#bar#nu_{e}"; break;
    case  12: nu = "#nu_{e}"; break;
    case -14: nu = "#bar#nu_{#mu}"; break;
    case  14: nu = "#nu_{#mu}"; break;
    case -16: nu = "#bar#nu_{#tau}"; break;
    case  16: nu = "#nu_{#tau}"; break;
    default: break;
  }

  return nu;
}

std::string Filter::GetNuMode(int mode) {
  std::string inttype;

  switch (mode) {
    case simb::kQE: inttype = "QE"; break;
    case simb::kRes: inttype = "Res"; break;
    case simb::kDIS: inttype = "DIS"; break;
    case simb::kCoh: inttype = "Coh"; break;
    case simb::kMEC: inttype = "MEC"; break;
    default: break;
  }

  return inttype;
}


namespace filters {

  NuMode::NuMode(int _pdg, int _mode, int _cc)
      : pdg(_pdg), mode(_mode), cc(_cc) {
    std::string nu = Filter::GetNuType(pdg);
    std::string inttype = Filter::GetNuMode(mode);
    title = nu + (cc == simb::kCC ? "CC" : "NC") + inttype;    
  }

  bool NuMode::operator()(const simb::MCTruth& truth) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();
    return (nu.Nu().PdgCode() == pdg &&
            nu.CCNC() == cc &&
            nu.Mode() == mode);
  }


  CC1Pi::CC1Pi(int _pdg, bool _charged)
      : pdg(_pdg), charged(_charged) {
    std::string nu = Filter::GetNuType(pdg);
    title = nu + "CC1#pi" + (charged ? "^{#pm}" : "");
  }

  bool CC1Pi::operator()(const simb::MCTruth& truth) {
    const simb::MCNeutrino& nu = truth.GetNeutrino();
    if (nu.Nu().PdgCode() != pdg || nu.CCNC() != simb::kCC) {
      return false;
    }

    // Count final state pions
    size_t npi = 0;

    for (int i=0; i<truth.NParticles(); i++) {
      const simb::MCParticle& part = truth.GetParticle(i);
      if (part.StatusCode() != genie::kIStStableFinalState) {
        continue;
      }

      if (abs(part.PdgCode()) == 211) {
        npi++;
      }
      else if (part.PdgCode() == 111) {
        if (charged) {
          return false;
        }
        npi++;
      }
    }

    return (npi == 1);
  }

}  // namespace filters

