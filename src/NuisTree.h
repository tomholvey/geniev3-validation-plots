#ifndef __NUISTREE_H__
#define __NUISTREE_H__

#include "TTree.h"
#include "TLorentzVector.h"
#include "enums.h"

class NuisTree{
public:
	NuisTree(TTree *intree);
	~NuisTree() {};

  int GetEntries(){return tr->GetEntries();};
  bool GetEntry(int i){return tr->GetEntry(i);};

  int GetCCNCEnum() const;
  int GetGENIEMode() const;

  bool IsCCQE() const;
  bool IsCCMEC() const;
  bool IsCCDIS() const;
  bool IsCCRES() const;

  // Add Emiss and Pmiss
  float Emiss;
  TVector3* pmiss;
  float Emiss_preFSI;
  TVector3* pmiss_preFSI;
  double Emiss_GENIE;

  int ntweaks_Emiss_CorrTailRW;
  int ntweaks_Emiss_LinearRW;
  int ntweaks_Emiss_TrigRW;
  double tweak_responses_Emiss_CorrTailRW[9999];
  double tweak_responses_Emiss_LinearRW[9999];
  double tweak_responses_Emiss_TrigRW[9999];
  
  int Mode;
  int PDGnu;
  Char_t iscc; // 1 if CC event, 0 if NC (for some reason this is stored as a char in the nuisance tree, not a bool. We will have to convert it into a bool to use it...)
  int tgt;
  int tgta;
  int tgtz;
  float Enu_true;
  int PDGLep;
  float ELep;
  float CosLep;
  float CosThetaAdler;
  float PhiAdler;
  float dalphat;
  float dpt;
  float dphit;
  float Q2;
  float q0;
  float q3;
  float Enu_QE;
  float Q2_QE;
  float W_nuc_rest;
  float W;
  float W_genie;
  float x;
  float y;
  float Erecoil_minerva;
  float EavAlt;
  float pnreco_c;
  int nfsp;
  float fsp_px[9999];
  float fsp_py[9999];
  float fsp_pz[9999];
  float fsp_E[9999];
  int fsp_pdg[9999];
  int fsp_pdg_rank[9999];
  // std::vector<float> *fsp_px=nullptr;
  // std::vector<float> *fsp_py=nullptr;
  // std::vector<float> *fsp_pz=nullptr;
  // std::vector<float> *fsp_E=nullptr;
  // std::vector<int> *fsp_pdg=nullptr;
  // std::vector<int> *fsp_pdg_rank=nullptr;
  int ninitp;
  float initp_px[9999];
  float initp_py[9999];
  float initp_pz[9999];
  float initp_E[9999];
  int initp_pdg[9999];
  // std::vector<float> *initp_px=nullptr;
  // std::vector<float> *initp_py=nullptr;
  // std::vector<float> *initp_pz=nullptr;
  // std::vector<float> *initp_E=nullptr;
  // std::vector<int> *initp_pdg=nullptr;
  int nvertp;
  float vertp_px[9999];
  float vertp_py[9999];
  float vertp_pz[9999];
  float vertp_E[9999];
  int vertp_pdg[9999];
  // std::vector<float> *vertp_px=nullptr;
  // std::vector<float> *vertp_py=nullptr;
  // std::vector<float> *vertp_pz=nullptr;
  // std::vector<float> *vertp_E=nullptr;
  // std::vector<int> *vertp_pdg=nullptr;
  float Weight;
  float InputWeight;
  float RWWeight;
  float CustomWeight;
  float CustomWeightArray[9999];
  double fScaleFactor;
  bool flagCCINC;
  bool flagNCINC;
  bool flagCCQE;
  bool flagCC0pi;
  bool flagCC0piMINERvA;
  bool flagCCQELike;
  bool flagNCEL;
  bool flagNC0pi;
  bool flagCCcoh;
  bool flagNCcoh;
  bool flagCC1pip;
  bool flagNC1pip;
  bool flagCC1pim;
  bool flagNC1pim;
  bool flagCC1pi0;
  bool flagNC1pi0;


private:
  TTree *tr;
};

#endif
