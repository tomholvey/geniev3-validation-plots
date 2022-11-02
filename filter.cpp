#include <string>
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
    case enums::kQE: inttype = "QE"; break;
    case enums::kRes: inttype = "Res"; break;
    case enums::kDIS: inttype = "DIS"; break;
    case enums::kCoh: inttype = "Coh"; break;
    case enums::kMEC: inttype = "MEC"; break;
    case enums::kUndefined: inttype = "INC"; break;
    default: break;
  }

  return inttype;
}


namespace filters {

  NuMode::NuMode(int _pdg, int _cc, int _mode)
      : pdg(_pdg), mode(_mode), cc(_cc) {
    std::string nu = Filter::GetNuType(pdg);
    std::string inttype = Filter::GetNuMode(mode);
    title = nu + (cc == enums::kCC ? "CC" : "NC") + inttype;
  }

  bool NuMode::operator()(const NuisTree& nuistr) {
    int _cc_tmp = nuistr.GetCCNCEnum();
    int _mode_tmp = nuistr.GetGENIEMode();
    if (mode==enums::kUndefined){
      return (nuistr.PDGnu == pdg &&
              _cc_tmp == cc);
    }
    return (nuistr.PDGnu == pdg &&
            _cc_tmp == cc &&
            _mode_tmp == mode);
  }

  CC1Pi::CC1Pi(int _pdg, bool _charged)
      : pdg(_pdg), charged(_charged) {
    std::string nu = Filter::GetNuType(pdg);
    title = nu + "CC1#pi" + (charged ? "^{#pm}" : "");
  }

  bool CC1Pi::operator()(const NuisTree& nuistr) {
    if (nuistr.PDGnu != pdg) return false;

    if (charged && (nuistr.flagCC1pip || nuistr.flagCC1pim)) return true;
    if (!charged && (nuistr.flagCC1pip || nuistr.flagCC1pim || nuistr.flagCC1pi0)) return true;

    return false;
  }
}  // namespace filters
