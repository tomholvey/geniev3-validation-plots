#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include <iostream>

// TH: quick script to add Nuisance flat vector tree information
//     to a NuSystematics weight file 
//
//     Both files should be generated from the same GENIE file with 
//     the same number of events

void AddFriend(string NuisFilename, string NuSysFilename){
  TFile* NuisFile = new TFile(NuisFilename.c_str(), "READ");
  TFile* NuSysFile = new TFile(NuSysFilename.c_str(), "UPDATE");

  TTree* NuisTree = (TTree*)NuisFile->Get("FlatTree_VARS");
  TTree* NuSysTree = (TTree*)NuSysFile->Get("events");

  TTree* newtree = NuisTree->CloneTree();

  NuSysFile->cd();
  newtree->AddFriend(NuSysTree);
  newtree->Write();

  NuSysFile->Close();

}
