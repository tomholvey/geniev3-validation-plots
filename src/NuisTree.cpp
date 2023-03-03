#include "NuisTree.h"


NuisTree::NuisTree(TTree *intree):
  tr(intree)
  {
    // Friend tree test
    tr->SetBranchAddress("ntweaks_CorrTailRW", &ntweaks_CorrTailRW);
    tr->SetBranchAddress("tweak_responses_CorrTailRW", &tweak_responses_CorrTailRW);
    
    // Add in EMiss and PMiss
	tr->SetBranchAddress("Emiss", &Emiss);
	tr->SetBranchAddress("pmiss", &pmiss);
	tr->SetBranchAddress("Emiss_preFSI", &Emiss_preFSI);
	tr->SetBranchAddress("pmiss_preFSI", &pmiss_preFSI);
	
	tr->SetBranchAddress("Mode",&Mode);
    tr->SetBranchAddress("PDGnu",&PDGnu);
    tr->SetBranchAddress("cc",&iscc);
    tr->SetBranchAddress("tgt",&tgt);
    tr->SetBranchAddress("tgta",&tgta);
    tr->SetBranchAddress("tgtz",&tgtz);
    tr->SetBranchAddress("Enu_true",&Enu_true);
    tr->SetBranchAddress("PDGLep",&PDGLep);
    tr->SetBranchAddress("ELep",&ELep);
    tr->SetBranchAddress("CosLep",&CosLep);
    tr->SetBranchAddress("CosThetaAdler",&CosThetaAdler);
    tr->SetBranchAddress("PhiAdler",&PhiAdler);
    tr->SetBranchAddress("dalphat",&dalphat);
    tr->SetBranchAddress("dpt",&dpt);
    tr->SetBranchAddress("dphit",&dphit);
    tr->SetBranchAddress("Q2",&Q2);
    tr->SetBranchAddress("q0",&q0);
    tr->SetBranchAddress("q3",&q3);
    tr->SetBranchAddress("Enu_QE",&Enu_QE);
    tr->SetBranchAddress("Q2_QE",&Q2_QE);
    tr->SetBranchAddress("W_nuc_rest",&W_nuc_rest);
    tr->SetBranchAddress("W",&W);
    tr->SetBranchAddress("W_genie",&W_genie);
    tr->SetBranchAddress("x",&x);
    tr->SetBranchAddress("y",&y);
    tr->SetBranchAddress("Erecoil_minerva",&Erecoil_minerva);
    tr->SetBranchAddress("EavAlt",&EavAlt);
    tr->SetBranchAddress("pnreco_C",&pnreco_c);
    tr->SetBranchAddress("nfsp",&nfsp);
    tr->SetBranchAddress("px",&fsp_px);
    tr->SetBranchAddress("py",&fsp_py);
    tr->SetBranchAddress("pz",&fsp_pz);
    tr->SetBranchAddress("E",&fsp_E);
    tr->SetBranchAddress("pdg",&fsp_pdg);
    tr->SetBranchAddress("pdg_rank",&fsp_pdg_rank);
    tr->SetBranchAddress("ninitp",&ninitp);
    tr->SetBranchAddress("px_init",&initp_px);
    tr->SetBranchAddress("py_init",&initp_py);
    tr->SetBranchAddress("pz_init",&initp_pz);
    tr->SetBranchAddress("E_init",&initp_E);
    tr->SetBranchAddress("pdg_init",&initp_pdg);
    tr->SetBranchAddress("nvertp",&nvertp);
    tr->SetBranchAddress("px_vert",&vertp_px);
    tr->SetBranchAddress("py_vert",&vertp_py);
    tr->SetBranchAddress("pz_vert",&vertp_pz);
    tr->SetBranchAddress("E_vert",&vertp_E);
    tr->SetBranchAddress("pdg_vert",&vertp_pdg);
    tr->SetBranchAddress("Weight",&Weight);
    tr->SetBranchAddress("InputWeight",&InputWeight);
    tr->SetBranchAddress("RWWeight",&RWWeight);
    tr->SetBranchAddress("CustomWeight",&CustomWeight);
    tr->SetBranchAddress("CustomWeightArray",&CustomWeightArray);
    tr->SetBranchAddress("fScaleFactor",&fScaleFactor);
    tr->SetBranchAddress("flagCCINC",&flagCCINC);
    tr->SetBranchAddress("flagNCINC",&flagNCINC);
    tr->SetBranchAddress("flagCCQE",&flagCCQE);
    tr->SetBranchAddress("flagCC0pi",&flagCC0pi);
    tr->SetBranchAddress("flagCC0piMINERvA",&flagCC0piMINERvA);
    tr->SetBranchAddress("flagCCQELike",&flagCCQELike);
    tr->SetBranchAddress("flagNCEL",&flagNCEL);
    tr->SetBranchAddress("flagNC0pi",&flagNC0pi);
    tr->SetBranchAddress("flagCCcoh",&flagCCcoh);
    tr->SetBranchAddress("flagNCcoh",&flagNCcoh);
    tr->SetBranchAddress("flagCC1pip",&flagCC1pip);
    tr->SetBranchAddress("flagNC1pip",&flagNC1pip);
    tr->SetBranchAddress("flagCC1pim",&flagCC1pim);
    tr->SetBranchAddress("flagNC1pim",&flagNC1pim);
    tr->SetBranchAddress("flagCC1pi0",&flagCC1pi0);
    tr->SetBranchAddress("flagNC1pi0",&flagNC1pi0);
}

int NuisTree::GetCCNCEnum() const{
  if ((bool)iscc==true) return enums::kCC;
  else return enums::kNC;
}

bool NuisTree::IsCCQE() const {
	if (abs(Mode) == 1) return true;
	else return false;
}

bool NuisTree::IsCCMEC() const {
	if (abs(Mode) == 2) return true;
	else return false;
}

bool NuisTree::IsCCDIS() const {
	if (abs(Mode) == 21 || abs(Mode) == 26) return true;
	else return false;
}

bool NuisTree::IsCCRES() const {
	if (abs(Mode) == 11 || abs(Mode) == 12 || abs(Mode) == 13 || abs(Mode) == 17 || abs(Mode) == 22 || abs(Mode) == 23) return true;
	else return false;
}

int NuisTree::GetGENIEMode() const{
  // References:
  // 1) NEUT mode description (used by NUISANCE) http://wng.ift.uni.wroc.pl/karp45/software/NeutUsage.pdf
  // 2) GENIE->NEUT translation https://github.com/GENIE-MC/Generator/blob/master/src/Framework/GHEP/GHepUtils.cxx#L30
  // 3) NUISANCE modes https://github.com/NUISANCEMC/nuisance/blob/master/src/InputHandler/InteractionModes.h
  switch(Mode){
    case 1: // NUISANCE kCCQE = 1
    case -1:
	case 51: // NUISANCE kNCELonp = 51
	case -51:
    case 52: // NUISANCE kNCELonn = 52
	case -52:
      return enums::kQE;
      break;
    case 11: // NUISANCE kCC1piponp (Res) = 11
	case -11:
    case 12: // NUISANCE kCC1pi0onn (Res) = 12
	case -12:
    case 13: // NUISANCE kCC1piponn (Res) = 13
	case -13:
    case 17: // NUISANCE kCC1gamma (Res) = 17
	case -17:
    case 22: // NUISANCE kCC1etaonn (Res) = 22
	case -22:
    case 23: // NUISANCE kCC1kaonp (Res) = 23
	case -23:
    case 31: // NUISANCE kNC1pi0onn (Res) = 31
	case -31:
    case 32: // NUISANCE kNC1pi0onp (Res) = 32
	case -32:
    case 33: // NUISANCE kNC1pimonn (Res) = 33
	case -33:
    case 34: // NUISANCE kNC1piponp (Res) = 34
	case -34:
    case 38: // NUISANCE kNC1gamman (Res) = 38
	case -38:
    case 39: // NUISANCE kNC1gammap (Res) = 39
	case -39:
    case 42: // NUISANCE kNC1etaonn (Res) = 42
	case -42:
    case 43: // NUISANCE kNC1etaonp (Res) = 43
	case -43:
    case 44: // NUISANCE kNC1kaon0 (Res) = 44
	case -44:
    case 45: // NUISANCE kNC1kaonp (Res) = 45
	case -45:
      return enums::kRes;
      break;
    case 21: // NUISANCE kCCmultipi (multipi with W<2 GeV, not sure if res or dis but will call DIS) = 21
	case -21:
    case 26: // NUISANCE kCCDIS = 26
	case -26:
    case 41: // NUISANCE kNCmultipi (multipi with W<2 GeV, not sure if res or dis but will call DIS) = 41
	case -41:
    case 46: // NUISANCE kNCDIS = 46
	case -46:
      return enums::kDIS;
      break;
    case 16: // NUISANCE kCCCoherent = 16
	case -16:
    case 36: // NUISANCE kNCCoherent = 36
	case -36:
      return enums::kCoh;
      break;
    case 2: // NUISANCE kCC2p2h = 2
	case -2:
    case 53: // NUISANCE kNC2p2h = 53
	case -53:
      return enums::kMEC;
      break;
    default :
      return enums::kUndefined;
      break;
  }
}
