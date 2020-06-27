//Andrew Loeliger
//Input options: 
// -s disables shape uncertainties
// -e disables embedded
// -b disables bin-by-bin uncertainties
// -g uses inclusive ggH distributions
// -q uses inclusive qqH distributions
// -c run in control plot mode
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"
#include "CombineHarvester/Run2HTT_Combine/interface/InputParserUtility.h"
#include "CombineHarvester/Run2HTT_Combine/interface/UtilityFunctions.h"

using namespace std;

int main(int argc, char **argv)

{
  InputParserUtility Input(argc,argv);

  //! [part1]
  // First define the location of the "auxiliaries" directory where we can
  // source the input files containing the datacard shapes
  cout<<"test"<<endl;
  string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/auxiliaries/shapes/";
  
  //keep a handle on the file, we need it to check if shapes are empty.
  TFile* TheFile;
  if (Input.OptionExists("-c")) TheFile = new TFile((aux_shapes+"mt_controls_2016.root").c_str());
  else if (Input.OptionExists("-gf")) TheFile = new TFile((aux_shapes+"smh2016mt_GOF.root").c_str());
  else if (Input.OptionExists("-dp") or Input.OptionExists("-dn") or Input.OptionExists("-dm") or Input.OptionExists("-dljpt")) TheFile = new TFile((aux_shapes+"smh2016mt_Differential.root").c_str());
  else TheFile = new TFile((aux_shapes+"smh2016mt.root").c_str());  
    
  //categories loaded from configurations
  std::vector<std::pair<int,std::string>> cats = {};
  std::vector<std::string> CategoryArgs = Input.GetAllArguments("--Categories");
  int CatNum=1;
  for (auto it = CategoryArgs.begin(); it != CategoryArgs.end(); ++it)
    {					       
      std::cout<<"Making category for: "<<CatNum<<" "<<*it<<std::endl;
      cats.push_back({CatNum,(std::string)*it});
      CatNum++;
    }  

  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  vector<string> masses = {""};
  //! [part3]
  cb.AddObservations({"*"}, {"smh2016"}, {"13TeV"}, {"mt"}, cats);

  vector<string> bkg_procs = {"VVT","STT","TTT","jetFakes","ZL","VVL","STL","TTL"};
  if(Input.OptionExists("-e")) {bkg_procs.push_back("ZT");}
  else bkg_procs.push_back("embedded");
  if (Input.OptionExists("-dp") || Input.OptionExists("-dn") || Input.OptionExists("-dm") || Input.OptionExists("-dljpt"))
    {      
      bkg_procs.push_back("ggH_htt_nonfid125");
      bkg_procs.push_back("qqH_htt_nonfid125");
      bkg_procs.push_back("WH_htt_nonfid125");
      bkg_procs.push_back("ZH_htt_nonfid125");
    }
  else
    {
      bkg_procs.push_back("ggH_hww125");
      bkg_procs.push_back("qqH_hww125");
      bkg_procs.push_back("WH_hww125");
      bkg_procs.push_back("ZH_hww125");
    }
  cb.AddProcesses({"*"}, {"smh2016"}, {"13TeV"}, {"mt"}, bkg_procs, cats, false);

  vector<string> ggH_STXS;
  if (Input.OptionExists("-g")) ggH_STXS = {"ggH_htt125"};
  //PTH differential option
  else if (Input.OptionExists("-dp")) ggH_STXS = {
      "ggH_PTH_0_45_htt125",
      "ggH_PTH_45_80_htt125",
      "ggH_PTH_80_120_htt125",
      "ggH_PTH_120_200_htt125",
      "ggH_PTH_200_350_htt125",
      "ggH_PTH_350_450_htt125",
      "ggH_PTH_GT450_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) ggH_STXS = {
      "ggH_NJETS_0_htt125",
      "ggH_NJETS_1_htt125",
      "ggH_NJETS_2_htt125",
      "ggH_NJETS_3_htt125",
      "ggH_NJETS_GE4_htt125",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) ggH_STXS = {
      "ggH_MJJ_0_150_htt125",
      "ggH_MJJ_150_300_htt125",
      "ggH_MJJ_300_450_htt125",
      "ggH_MJJ_450_600_htt125",
      "ggH_MJJ_600_1000_htt125",
      "ggH_MJJ_1000_1400_htt125",
      "ggH_MJJ_1400_1800_htt125",
      "ggH_MJJ_GE1800_htt125",
    };
  else if (Input.OptionExists("-dljpt")) ggH_STXS = {
      "ggH_LJPT_30_60_htt125",
      "ggH_LJPT_60_120_htt125",
      "ggH_LJPT_120_200_htt125",
      "ggH_LJPT_200_350_htt125",
      "ggH_LJPT_GT350_htt125",
    };
  else ggH_STXS = {"ggH_PTH_0_200_0J_PTH_10_200_htt125",
		   "ggH_PTH_0_200_0J_PTH_0_10_htt125",
		   "ggH_PTH_0_200_1J_PTH_0_60_htt125",
		   "ggH_PTH_0_200_1J_PTH_60_120_htt125",
		   "ggH_PTH_0_200_1J_PTH_120_200_htt125",
		   "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125",		   
		   "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125",		   
		   "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125",		   
		   "ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125",		   
		   "ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125",
		   "ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125",		   
		   "ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125",		   
		   "ggH_PTH_200_300_htt125",
		   "ggH_PTH_300_450_htt125",
		   "ggH_PTH_450_650_htt125",
		   "ggH_PTH_GE650_htt125"};
  
  vector<string> qqH_STXS; 
  if(Input.OptionExists("-q")) qqH_STXS = {"qqH_htt125"};
  else if (Input.OptionExists("-dp")) qqH_STXS = {
      "qqH_PTH_0_45_htt125",
      "qqH_PTH_45_80_htt125",
      "qqH_PTH_80_120_htt125",
      "qqH_PTH_120_200_htt125",
      "qqH_PTH_200_350_htt125",
      "qqH_PTH_350_450_htt125",
      "qqH_PTH_GT450_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) qqH_STXS = {
      "qqH_NJETS_0_htt125",
      "qqH_NJETS_1_htt125",
      "qqH_NJETS_2_htt125",
      "qqH_NJETS_3_htt125",
      "qqH_NJETS_GE4_htt125",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) qqH_STXS = {
      "qqH_MJJ_0_150_htt125",
      "qqH_MJJ_150_300_htt125",
      "qqH_MJJ_300_450_htt125",
      "qqH_MJJ_450_600_htt125",
      "qqH_MJJ_600_1000_htt125",
      "qqH_MJJ_1000_1400_htt125",
      "qqH_MJJ_1400_1800_htt125",
      "qqH_MJJ_GE1800_htt125",
    };
  else if (Input.OptionExists("-dljpt")) qqH_STXS = {
      "qqH_LJPT_30_60_htt125",
      "qqH_LJPT_60_120_htt125",
      "qqH_LJPT_120_200_htt125",
      "qqH_LJPT_200_350_htt125",
      "qqH_LJPT_GT350_htt125",
    };
  else qqH_STXS = {"qqH_0J_htt125",
		   "qqH_1J_htt125",
		   "qqH_GE2J_MJJ_0_60_htt125",
		   "qqH_GE2J_MJJ_60_120_htt125",
		   "qqH_GE2J_MJJ_120_350_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
		   "qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"};

  vector<string> WH_STXS;
  if (Input.OptionExists("-q")) WH_STXS = {"WH_lep_htt125","WH_had_htt125"};
  else if (Input.OptionExists("-dp")) WH_STXS = {
      "WH_PTH_0_45_htt125",
      "WH_PTH_45_80_htt125",
      "WH_PTH_80_120_htt125",
      "WH_PTH_120_200_htt125",
      "WH_PTH_200_350_htt125",
      "WH_PTH_350_450_htt125",
      "WH_PTH_GT450_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) WH_STXS = {
      "WH_NJETS_0_htt125",
      "WH_NJETS_1_htt125",
      "WH_NJETS_2_htt125",
      "WH_NJETS_3_htt125",
      "WH_NJETS_GE4_htt125",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) WH_STXS = {
      "WH_MJJ_0_150_htt125",
      "WH_MJJ_150_300_htt125",
      "WH_MJJ_300_450_htt125",
      "WH_MJJ_450_600_htt125",
      "WH_MJJ_600_1000_htt125",
      "WH_MJJ_1000_1400_htt125",
      "WH_MJJ_1400_1800_htt125",
      "WH_MJJ_GE1800_htt125",
    };
  else if (Input.OptionExists("-dljpt")) WH_STXS = {
      "WH_LJPT_30_60_htt125",
      "WH_LJPT_60_120_htt125",
      "WH_LJPT_120_200_htt125",
      "WH_LJPT_200_350_htt125",
      "WH_LJPT_GT350_htt125",
    };
  else WH_STXS = {
      "WH_lep_htt125",
      "WH_0J_htt125",
      "WH_1J_htt125",
      "WH_GE2J_MJJ_0_60_htt125",
      "WH_GE2J_MJJ_60_120_htt125",
      "WH_GE2J_MJJ_120_350_htt125",
      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
      "WH_GE2J_MJJ_GE350_PTH_GE200_htt125",
    };

  vector<string> ZH_STXS;
  if (Input.OptionExists("-q")) ZH_STXS = {"ZH_lep_htt125","ZH_had_htt125"};
  else if (Input.OptionExists("-dp")) ZH_STXS = {
      "ZH_PTH_0_45_htt125",
      "ZH_PTH_45_80_htt125",
      "ZH_PTH_80_120_htt125",
      "ZH_PTH_120_200_htt125",
      "ZH_PTH_200_350_htt125",
      "ZH_PTH_350_450_htt125",
      "ZH_PTH_GT450_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) ZH_STXS = {
      "ZH_NJETS_0_htt125",
      "ZH_NJETS_1_htt125",
      "ZH_NJETS_2_htt125",
      "ZH_NJETS_3_htt125",
      "ZH_NJETS_GE4_htt125",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) ZH_STXS = {
      "ZH_MJJ_0_150_htt125",
      "ZH_MJJ_150_300_htt125",
      "ZH_MJJ_300_450_htt125",
      "ZH_MJJ_450_600_htt125",
      "ZH_MJJ_600_1000_htt125",
      "ZH_MJJ_1000_1400_htt125",
      "ZH_MJJ_1400_1800_htt125",
      "ZH_MJJ_GE1800_htt125",
    };
  else if (Input.OptionExists("-dljpt")) ZH_STXS = {
      "ZH_LJPT_30_60_htt125",
      "ZH_LJPT_60_120_htt125",
      "ZH_LJPT_120_200_htt125",
      "ZH_LJPT_200_350_htt125",
      "ZH_LJPT_GT350_htt125",
    };
  else ZH_STXS = {
      "ZH_lep_htt125",
      "ZH_0J_htt125",
      "ZH_1J_htt125",
      "ZH_GE2J_MJJ_0_60_htt125",
      "ZH_GE2J_MJJ_60_120_htt125",
      "ZH_GE2J_MJJ_120_350_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
      "ZH_GE2J_MJJ_GE350_PTH_GE200_htt125",
    };

  
  vector<string> ggZH_STXS;
  if (Input.OptionExists("-g")) ggZH_STXS = {"ggZH_lep_htt125","ggZH_had_htt125"};
  else if (Input.OptionExists("-dm")||Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dljpt")) ggZH_STXS = {};
  else ggZH_STXS = {
      "ggZH_lep_htt125",
      "ggZH_PTH_0_200_0J_PTH_10_200_htt125",
      "ggZH_PTH_0_200_0J_PTH_0_10_htt125",
      "ggZH_PTH_0_200_1J_PTH_0_60_htt125",
      "ggZH_PTH_0_200_1J_PTH_60_120_htt125",
      "ggZH_PTH_0_200_1J_PTH_120_200_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125",
      "ggZH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125",
      "ggZH_PTH_200_300_htt125",
      "ggZH_PTH_300_450_htt125",
      "ggZH_PTH_450_650_htt125",
      "ggZH_PTH_GE650_htt125"
    };

  vector<string> sig_procs = ch::JoinStr({ggH_STXS,
	qqH_STXS,
	WH_STXS,
	ZH_STXS,
	ggZH_STXS});
  
  cb.AddProcesses(masses, {"smh2016"}, {"13TeV"}, {"mt"}, sig_procs, cats, true);    

  //! [part4]

  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;
  using ch::JoinStr;

  //start with lnN errors
  //********************************************************************************************************************************
  
  //Theory uncerts
  if (not(Input.OptionExists("-x0")||Input.OptionExists("-x1")))
    {
      cb.cp().process(sig_procs).AddSyst(cb, "BR_htt_PU_alphas", "lnN", SystMap<>::init(1.0062));
      cb.cp().process(sig_procs).AddSyst(cb, "BR_htt_PU_mq", "lnN", SystMap<>::init(1.0099));
      cb.cp().process(sig_procs).AddSyst(cb, "BR_htt_THU", "lnN", SystMap<>::init(1.017));
      cb.cp().process(JoinStr({WH_STXS,{"WH_hww125","WH_htt_nonfid125"}})).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.008));
      cb.cp().process(JoinStr({ZH_STXS,{"ZH_hww125","ZH_htt_nonfid125"}})).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.009));
      cb.cp().process(JoinStr({WH_STXS,{"WH_hww125","WH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.018));
      cb.cp().process(JoinStr({ZH_STXS,{"ZH_hww125","ZH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.013));
      cb.cp().process(JoinStr({ggH_STXS,{"ggH_hww125","ggH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_gg", "lnN", SystMap<>::init(1.032));
      cb.cp().process(JoinStr({qqH_STXS,{"qqH_hww125","qqH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_qq", "lnN", SystMap<>::init(1.021));
    }
  cb.cp().process({"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125"}).AddSyst(cb, "BR_hww_PU_alphas", "lnN", ch::syst::SystMapAsymm<>::init(1.0066,1.0063));
  cb.cp().process({"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125"}).AddSyst(cb, "BR_hww_PU_mq", "lnN", ch::syst::SystMapAsymm<>::init(1.0099,1.0098));
  cb.cp().process({"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125"}).AddSyst(cb, "BR_hww_THU", "lnN", SystMap<>::init(1.0099));  
  //cb.cp().process(JoinStr({qqH_STXS,{"qqH_hww125"}})).AddSyst(cb, "QCDScale_qqH", "lnN", SystMap<>::init(1.005));  

  cb.cp().process({"ggH_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.002,0.998));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.006,0.997));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.000,0.998));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.954,1.026));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.069,0.956));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"qqH_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.027,0.986));
  cb.cp().process({"qqH_0J_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.022,0.991));
  cb.cp().process({"qqH_1J_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.041,0.974));
  cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"ggH_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.971,1.024));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.983,1.004));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.952,1.037));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.980,1.025));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.952,1.036));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.122,0.929));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.026,0.977));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.043,0.957));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.925,1.075));
  cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.920,1.087));
  cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.084,0.955));
  cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"qqH_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.011,0.994));
  cb.cp().process({"qqH_0J_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.980,1.011));
  cb.cp().process({"qqH_1J_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.011,0.995));
  cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.909,1.012));
  cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.000,0.952));
  cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.009,0.986));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.169,0.926));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.120,0.934));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"mt_boosted_1J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"ggH_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.017,0.977));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.038,0.984));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.013,0.996));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.033,0.979));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.041,0.955));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.980,0.983));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.012,0.979));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.027,0.972));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.082,0.907));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.047,0.909));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.038,0.975));
  cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.978,1.001));
  cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.054,0.941));
  cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"qqH_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.985,1.013));
  cb.cp().process({"qqH_0J_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_1J_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.053,0.964));
  cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.943,1.033));
  cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.969,1.020));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.898,1.081));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.031,0.997));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"ggH_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.061,0.957));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.037,0.973));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.023,0.977));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.205,0.885));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.039,0.974));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.088,0.924));
  cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.073,0.944));
  cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"qqH_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.986,1.009));
  cb.cp().process({"qqH_0J_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_1J_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.026,0.995));
  cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.957,1.031));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.960,1.023));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.976,1.004));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.984,1.010));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.009,0.994));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.004,0.994));
  cb.cp().process({"ggH_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.099,0.961));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_10_200_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.006,0.997));
  cb.cp().process({"ggH_PTH_0_200_0J_PTH_0_10_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_0_60_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_60_120_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_1J_PTH_120_200_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"ggH_PTH_200_300_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.099,0.961));
  cb.cp().process({"ggH_PTH_300_450_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.081,0.972));
  cb.cp().process({"ggH_PTH_450_650_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.177,0.911));
  cb.cp().process({"ggH_PTH_GE650_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.00));
  cb.cp().process({"qqH_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.978,1.008));
  cb.cp().process({"qqH_0J_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_1J_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_0_60_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_60_120_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_120_350_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(1.038,0.980));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",SystMap<>::init(1.0));
  cb.cp().process({"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"CMS_pythia_scale","lnN",ch::syst::SystMapAsymm<>::init(0.980,1.005));

  //pdf acceptance uncertainties
  cb.cp().process({ggH_STXS}).bin({"mt_0jet"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.008));
  cb.cp().process({ggH_STXS}).bin({"mt_boosted_1J"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.008));
  cb.cp().process({ggH_STXS}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.005));
  cb.cp().process({ggH_STXS}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.011));
  cb.cp().process({ggH_STXS}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"pdf_Higgs_gg_ACCEPT","lnN",SystMap<>::init(1.011));

  cb.cp().process({qqH_STXS}).bin({"mt_0jet"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.006));
  cb.cp().process({qqH_STXS}).bin({"mt_boosted_1J"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.005));
  cb.cp().process({qqH_STXS}).bin({"mt_boosted_GE2J"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.004));
  cb.cp().process({qqH_STXS}).bin({"mt_vbf_PTH_0_200"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.008));
  cb.cp().process({qqH_STXS}).bin({"mt_vbf_PTH_GE_200"}).AddSyst(cb,"pdf_Higgs_qq_ACCEPT","lnN",SystMap<>::init(1.008));


  //Muon ID efficiency: Decorollated in 18-032 datacards.  
  cb.cp().process(JoinStr({{"ZT","TTT","VVT","STT","ZL","TTL","VVL","STL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_eff_m_2016","lnN",SystMap<>::init(1.02));

  // Against ele and against mu for real taus
  //cb.cp().process(JoinStr({{"ZT","TTT","VVT","STT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_eff_t_againstemu_mt_2016","lnN",SystMap<>::init(1.01));

    // b-tagging efficiency
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_eta","lnN",SystMap<>::init(1.005));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_btag_eta","lnN",SystMap<>::init(1.001));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_hf","lnN",SystMap<>::init(0.993));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_btag_hf","lnN",SystMap<>::init(1.002));
  
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_hfstats1_2016","lnN",SystMap<>::init(1.03));
  //cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_btag_hfstats1_2016","lnN",SystMap<>::init(1.0000));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_hfstats2_2016","lnN",SystMap<>::init(1.015));
  //cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_hfstats2_2016","lnN",SystMap<>::init(1.000));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_jes","lnN",SystMap<>::init(0.98));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_btag_jes","lnN",SystMap<>::init(1.003));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_lf","lnN",SystMap<>::init(0.90));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_btag_lf","lnN",SystMap<>::init(0.999));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_lfstats1_2016","lnN",SystMap<>::init(0.995));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_btag_lfstats1_2016","lnN",SystMap<>::init(0.999));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_lfstats2_2016","lnN",SystMap<>::init(0.995));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"},sig_procs})).AddSyst(cb,"CMS_btag_lfstats2_2016","lnN",SystMap<>::init(1.001));

  // XSection Uncertainties
  cb.cp().process({"TTT","TTL"}).AddSyst(cb,"CMS_htt_tjXsec", "lnN", SystMap<>::init(1.042));
  cb.cp().process({"VVT","VVL"}).AddSyst(cb,"CMS_htt_vvXsec", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"STT","STL"}).AddSyst(cb,"CMS_htt_stXsec", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"ZT","ZL"}).AddSyst(cb,"CMS_htt_zjXsec", "lnN", SystMap<>::init(1.02));

  //Muon Fake Rate Uncertainty
  //cb.cp().process({"ZL","TTL","VVL"}).AddSyst(cb, "CMS_mFakeTau_2016 ", "lnN",SystMap<>::init(1.20));    
  
  //Luminosity Uncertainty
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_Run2016", "lnN", SystMap<>::init(1.022));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_XYfactorization", "lnN", SystMap<>::init(1.009));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_beamBeamDeflection", "lnN", SystMap<>::init(1.004));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_dynamicBeta", "lnN", SystMap<>::init(1.005));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_ghostsAndSatellites", "lnN", SystMap<>::init(1.004));

  cb.cp().process({"jetFakes"}).bin({"mt_0jet"}).AddSyst(cb,"CMS_jetFakesNorm_0jet_mt_2016","lnN",SystMap<>::init(1.05));
  //cb.cp().process({"jetFakes"}).bin({"mt_0jet_PTH_GE10"}).AddSyst(cb,"CMS_jetFakesNorm_0jethigh_mt_2016","lnN",SystMap<>::init(1.05));

  //*******************************************************
  //shape uncertainties
  //*******************************************************
  if(not Input.OptionExists("-s"))
    {
      std::cout<<"Adding Shapes..."<<std::endl;
      //uses custom defined utility function that only adds the shape if at least one shape inside is not empty.

      vector<string> METUESVector;
      vector<string> RecoilVector;
      vector<string> ZPTVector;
      vector<string> TopVector;
      vector<string> TESVector;
      vector<string> JESVector;
      vector<string> MuESVector;
      vector<string> PrefiringVector;
      vector<string> TauIDVector;
      vector<string> TriggerVector;
      METUESVector = {"TTT","TTL","VVT","VVL","STT","STL"};
      TopVector = {"TTL","TTT"};
      if(Input.OptionExists("-e"))
	{	  
	  RecoilVector = JoinStr({ggH_STXS,qqH_STXS,{"ZT","ZL","ggH_hww125","qqH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125"}});
	  ZPTVector = {"ZT","ZL"};	  
	  TESVector = JoinStr({sig_procs,{"VVT","STT","ZT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  JESVector = JoinStr({sig_procs,{"ZT","VVT","STT","TTT","VVL","STL","ZL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  MuESVector = JoinStr({sig_procs,{"ZT","VVT","STT","TTT","ZL","VVL","STL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  PrefiringVector = JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  TauIDVector = JoinStr({sig_procs,{"VVT","STT","ZT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  TriggerVector = JoinStr({sig_procs,{"VVT","STT","ZT","TTT","VVL","STL","TTL","ZL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	}
      else
	{	  
	  RecoilVector = JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125"}});
	  ZPTVector = {"ZL"};	  
	  TESVector = JoinStr({sig_procs,{"VVT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  JESVector = JoinStr({sig_procs,{"VVT","TTT","VVL","ZL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  MuESVector = JoinStr({sig_procs,{"ZL","VVT","TTT","VVL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  PrefiringVector = JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  TauIDVector = JoinStr({sig_procs,{"VVT","STT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	  TriggerVector = JoinStr({sig_procs,{"VVT","STT","TTT","VVL","STL","TTL","ZL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}});
	}
      AddShapesIfNotEmpty({"CMS_m_FakeTau_etalt0p4_2016","CMS_m_FakeTau_eta0p4to0p8_2016",
	    "CMS_m_FakeTau_eta0p8to1p2_2016","CMS_m_FakeTau_eta1p2to1p7_2016",
	    "CMS_m_FakeTau_etagt1p7_2016","CMS_m_FakeTau_taupt30to40_2016",
	    "CMS_m_FakeTau_taupt40to50_2016","CMS_m_FakeTau_tauptgt50_2016"},
	{"ZL"},
	&cb,
	1.00,
	TheFile,CategoryArgs);
      
      AddShapesIfNotEmpty({"CMS_etauFR_vloose_barrel_2016","CMS_etauFR_vloose_endcap_2016"},
        {"STL","TTL","VVL"},
        &cb,
        1.00,
        TheFile,CategoryArgs);

      // Prefiring
      std::cout<<"Prefiring..."<<std::endl;
      AddShapesIfNotEmpty({"CMS_prefiring"},
                          PrefiringVector,
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);

      // Tau ID eff in pt bins
      std::cout<<"Tau ID eff"<<std::endl;
      AddShapesIfNotEmpty({"CMS_tauideff_pt30to35_2016","CMS_tauideff_pt35to40_2016","CMS_tauideff_ptgt40_2016"},
                          TauIDVector,
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);

      //Mu to tau fake energy fake scale            
      std::cout<<"ZLShapes"<<std::endl;
      if (Input.OptionExists("-dp")|Input.OptionExists("-dn")|Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
	{
	  AddShapesIfNotEmpty({"CMS_ZLShape_mt_1prong_2016","CMS_ZLShape_mt_1prong1pizero_2016"},
			      {"ZL"},
			      &cb,
			      1.00,
			      TheFile,CategoryArgs);
	}
      else
	{
	  AddShapesIfNotEmpty({"CMS_ZLShape_mt_1prong_2016","CMS_ZLShape_mt_1prong1pizero_2016"},
			      {"ZL"},
			      &cb,
			      1.00,
			      TheFile,{"mt_0jet","mt_boosted_1J","mt_boosted_GE2J"});
	}

      // Trg eff. It is a shape because the 2 triggers affect the ele pT spectrum differently
      std::cout<<"Trigger eff"<<std::endl;
      AddShapesIfNotEmpty({"CMS_singlemutrg_2016","CMS_mutautrg_2016"},
                          TriggerVector,
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);
      
      
      //Fake factors      
      if (Input.OptionExists("-c"))
	{
	  AddShapesIfNotEmpty({
	      "CMS_rawFF_mt_qcd_0jet_unc1_2016",
		"CMS_rawFF_mt_qcd_0jet_unc2_2016",
		"CMS_rawFF_mt_w_0jet_unc1_2016",
		"CMS_rawFF_mt_w_0jet_unc2_2016",
		"CMS_rawFF_mt_qcd_1jet_unc1_2016",
		"CMS_rawFF_mt_qcd_1jet_unc2_2016",
		"CMS_rawFF_mt_w_1jet_unc1_2016",
		"CMS_rawFF_mt_w_1jet_unc2_2016",
		"CMS_rawFF_mt_qcd_2jet_unc1_2016",
		"CMS_rawFF_mt_qcd_2jet_unc2_2016",
		"CMS_rawFF_mt_w_2jet_unc1_2016",
		"CMS_rawFF_mt_w_2jet_unc2_2016",
		"CMS_rawFF_mt_tt_unc1_2016",
		"CMS_rawFF_mt_tt_unc2_2016",
		//"CMS_FF_closure_mvis_mt_qcd_0jet",
		//"CMS_FF_closure_mvis_mt_w_0jet",
		//"CMS_FF_closure_mvis_mt_qcd_1jet",
		//"CMS_FF_closure_mvis_mt_w_1jet",
		//"CMS_FF_closure_mvis_mt_qcd_2jet",
		//"CMS_FF_closure_mvis_mt_w_2jet",	    
		//"CMS_FF_closure_mvis_mt_tt",       
		"CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		"CMS_FF_closure_lpt_xtrg_mt_w_2016",
		"CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		"CMS_FF_closure_lpt_mt_qcd",
		"CMS_FF_closure_lpt_mt_w",
		"CMS_FF_closure_lpt_mt_tt",
		"CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		"CMS_FF_closure_mt_mt_w_unc1_2016",
		"CMS_FF_closure_mt_mt_w_unc2_2016",
		},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      else if(Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
	{
	  AddShapesIfNotEmpty({
	      "CMS_rawFF_mt_qcd_0jet_unc1_2016",
		"CMS_rawFF_mt_qcd_0jet_unc2_2016",
		"CMS_rawFF_mt_w_0jet_unc1_2016",
		"CMS_rawFF_mt_w_0jet_unc2_2016",
		"CMS_rawFF_mt_qcd_1jet_unc1_2016",
		"CMS_rawFF_mt_qcd_1jet_unc2_2016",
		"CMS_rawFF_mt_w_1jet_unc1_2016",
		"CMS_rawFF_mt_w_1jet_unc2_2016",
		"CMS_rawFF_mt_qcd_2jet_unc1_2016",
		"CMS_rawFF_mt_qcd_2jet_unc2_2016",
		"CMS_rawFF_mt_w_2jet_unc1_2016",
		"CMS_rawFF_mt_w_2jet_unc2_2016",
		"CMS_rawFF_mt_tt_unc1_2016",
		"CMS_rawFF_mt_tt_unc2_2016",
		//"CMS_FF_closure_mvis_mt_qcd_0jet",
		//"CMS_FF_closure_mvis_mt_w_0jet",
		//"CMS_FF_closure_mvis_mt_qcd_1jet",
		//"CMS_FF_closure_mvis_mt_w_1jet",
		//"CMS_FF_closure_mvis_mt_qcd_2jet",
		//"CMS_FF_closure_mvis_mt_w_2jet",	    
		//"CMS_FF_closure_mvis_mt_tt",       
		"CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		"CMS_FF_closure_lpt_xtrg_mt_w_2016",
		"CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		"CMS_FF_closure_lpt_mt_qcd",
		"CMS_FF_closure_lpt_mt_w",
		"CMS_FF_closure_lpt_mt_tt",
		"CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		"CMS_FF_closure_mt_mt_w_unc1_2016",
		"CMS_FF_closure_mt_mt_w_unc2_2016",
		},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	  /*
	  AddShapesIfNotEmpty({
	      "CMS_rawFF_mt_qcd_0jet_unc1_2016",
		"CMS_rawFF_mt_qcd_0jet_unc2_2016",
		"CMS_rawFF_mt_w_0jet_unc1_2016",
		"CMS_rawFF_mt_w_0jet_unc2_2016",
		"CMS_rawFF_mt_tt_unc1_2016",
		"CMS_rawFF_mt_tt_unc2_2016",
		//"CMS_FF_closure_mvis_mt_qcd_0jet",
		//"CMS_FF_closure_mvis_mt_w_0jet",
		//"CMS_FF_closure_mvis_mt_tt",            
		"CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		"CMS_FF_closure_lpt_xtrg_mt_w_2016",
		"CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		"CMS_FF_closure_lpt_mt_qcd",
		"CMS_FF_closure_lpt_mt_w",
		"CMS_FF_closure_lpt_mt_tt",
		"CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		"CMS_FF_closure_mt_mt_w_unc1_2016",
		"CMS_FF_closure_mt_mt_w_unc2_2016"},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"mt_0jet"});

	  AddShapesIfNotEmpty({
	      "CMS_rawFF_mt_qcd_1jet_unc1_2016",
		"CMS_rawFF_mt_qcd_1jet_unc2_2016",
		"CMS_rawFF_mt_w_1jet_unc1_2016",
		"CMS_rawFF_mt_w_1jet_unc2_2016",
		"CMS_rawFF_mt_tt_unc1_2016",
		"CMS_rawFF_mt_tt_unc2_2016",
		//"CMS_FF_closure_mvis_mt_qcd_1jet",
		//"CMS_FF_closure_mvis_mt_w_1jet",
		//"CMS_FF_closure_mvis_mt_tt",  
		"CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		"CMS_FF_closure_lpt_xtrg_mt_w_2016",
		"CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		"CMS_FF_closure_lpt_mt_qcd",
		"CMS_FF_closure_lpt_mt_w",
		"CMS_FF_closure_lpt_mt_tt",
		"CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		"CMS_FF_closure_mt_mt_w_unc1_2016",
		"CMS_FF_closure_mt_mt_w_unc2_2016"},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"mt_1jet"});

	  if(Input.OptionExists("-dm"))
	    {
	      	      AddShapesIfNotEmpty({
		  "CMS_rawFF_mt_qcd_2jet_unc1_2016",
		    "CMS_rawFF_mt_qcd_2jet_unc2_2016",
		    "CMS_rawFF_mt_w_2jet_unc1_2016",
		    "CMS_rawFF_mt_w_2jet_unc2_2016",
		    "CMS_rawFF_mt_tt_unc1_2016",
		    "CMS_rawFF_mt_tt_unc2_2016",
		    //"CMS_FF_closure_mvis_mt_qcd_2jet",
		    //"CMS_FF_closure_mvis_mt_w_2jet",	    
		    //"CMS_FF_closure_mvis_mt_tt",  
		    "CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		    "CMS_FF_closure_lpt_xtrg_mt_w_2016",
		    "CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		    "CMS_FF_closure_lpt_mt_qcd",
		    "CMS_FF_closure_lpt_mt_w",
		    "CMS_FF_closure_lpt_mt_tt",
		    "CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		    "CMS_FF_closure_mt_mt_w_unc1_2016",
		    "CMS_FF_closure_mt_mt_w_unc2_2016"
		    },
		{"jetFakes"},
		&cb,
		1.00,
		TheFile,
		{"mt_2jet"});
	    }
	  else
	    {
	      AddShapesIfNotEmpty({
		  "CMS_rawFF_mt_qcd_2jet_unc1_2016",
		    "CMS_rawFF_mt_qcd_2jet_unc2_2016",
		    "CMS_rawFF_mt_w_2jet_unc1_2016",
		    "CMS_rawFF_mt_w_2jet_unc2_2016",
		    "CMS_rawFF_mt_tt_unc1_2016",
		    "CMS_rawFF_mt_tt_unc2_2016",
		    //"CMS_FF_closure_mvis_mt_qcd_2jet",
		    //"CMS_FF_closure_mvis_mt_w_2jet",	    
		    //"CMS_FF_closure_mvis_mt_tt",  
		    "CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		    "CMS_FF_closure_lpt_xtrg_mt_w_2016",
		    "CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		    "CMS_FF_closure_lpt_mt_qcd",
		    "CMS_FF_closure_lpt_mt_w",
		    "CMS_FF_closure_lpt_mt_tt",
		    "CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		    "CMS_FF_closure_mt_mt_w_unc1_2016",
		    "CMS_FF_closure_mt_mt_w_unc2_2016"
		    },
		{"jetFakes"},
		&cb,
		1.00,
		TheFile,
		{"mt_2jetlow","mt_2jethigh","mt_3jetlow","mt_3jethigh"});
	    }
	  */
	}
      else
	{
	  AddShapesIfNotEmpty({
	      "CMS_rawFF_mt_qcd_0jet_unc1_2016",
		"CMS_rawFF_mt_qcd_0jet_unc2_2016",
		"CMS_rawFF_mt_w_0jet_unc1_2016",
		"CMS_rawFF_mt_w_0jet_unc2_2016",
		"CMS_rawFF_mt_tt_unc1_2016",
		"CMS_rawFF_mt_tt_unc2_2016",
		//"CMS_FF_closure_mvis_mt_qcd_0jet",
		//"CMS_FF_closure_mvis_mt_w_0jet",
		//"CMS_FF_closure_mvis_mt_tt",            
		"CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		"CMS_FF_closure_lpt_xtrg_mt_w_2016",
		"CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		"CMS_FF_closure_lpt_mt_qcd",
		"CMS_FF_closure_lpt_mt_w",
		"CMS_FF_closure_lpt_mt_tt",
		"CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		"CMS_FF_closure_mt_mt_w_unc1_2016",
		"CMS_FF_closure_mt_mt_w_unc2_2016"},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"mt_0jet"});

	  AddShapesIfNotEmpty({
	      "CMS_rawFF_mt_qcd_1jet_unc1_2016",
		"CMS_rawFF_mt_qcd_1jet_unc2_2016",
		"CMS_rawFF_mt_w_1jet_unc1_2016",
		"CMS_rawFF_mt_w_1jet_unc2_2016",
		"CMS_rawFF_mt_tt_unc1_2016",
		"CMS_rawFF_mt_tt_unc2_2016",
		//"CMS_FF_closure_mvis_mt_qcd_1jet",
		//"CMS_FF_closure_mvis_mt_w_1jet",
		//"CMS_FF_closure_mvis_mt_tt",  
		"CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		"CMS_FF_closure_lpt_xtrg_mt_w_2016",
		"CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		"CMS_FF_closure_lpt_mt_qcd",
		"CMS_FF_closure_lpt_mt_w",
		"CMS_FF_closure_lpt_mt_tt",
		"CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		"CMS_FF_closure_mt_mt_w_unc1_2016",
		"CMS_FF_closure_mt_mt_w_unc2_2016"},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"mt_boosted_1J"});
	  
	  AddShapesIfNotEmpty({
	      "CMS_rawFF_mt_qcd_2jet_unc1_2016",
		"CMS_rawFF_mt_qcd_2jet_unc2_2016",
		"CMS_rawFF_mt_w_2jet_unc1_2016",
		"CMS_rawFF_mt_w_2jet_unc2_2016",
		"CMS_rawFF_mt_tt_unc1_2016",
		"CMS_rawFF_mt_tt_unc2_2016",
		//"CMS_FF_closure_mvis_mt_qcd_2jet",
		//"CMS_FF_closure_mvis_mt_w_2jet",	    
		//"CMS_FF_closure_mvis_mt_tt",  
		"CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		"CMS_FF_closure_lpt_xtrg_mt_w_2016",
		"CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		"CMS_FF_closure_lpt_mt_qcd",
		"CMS_FF_closure_lpt_mt_w",
		"CMS_FF_closure_lpt_mt_tt",
		"CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		"CMS_FF_closure_mt_mt_w_unc1_2016",
		"CMS_FF_closure_mt_mt_w_unc2_2016"
		},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"mt_boosted_GE2J","mt_vbf_PTH_0_200","mt_vbf_PTH_GE_200"});
	}

      //MET Unclustered Energy Scale      
      std::cout<<"MET UES"<<std::endl;
      AddShapesIfNotEmpty({"CMS_scale_met_unclustered_2016"},
			  METUESVector,
			  &cb,
			  1.00,
			  TheFile,CategoryArgs);
      
      //Recoil Shapes:                        
      std::cout<<"Recoil shapes"<<std::endl;
      if (Input.OptionExists("-c"))
	{
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2016","CMS_htt_boson_scale_met_0jet_2016",
		"CMS_htt_boson_reso_met_1jet_2016","CMS_htt_boson_scale_met_1jet_2016",
		"CMS_htt_boson_reso_met_2jet_2016","CMS_htt_boson_scale_met_2jet_2016"},
	    RecoilVector,
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      else if(Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
	{
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2016","CMS_htt_boson_scale_met_0jet_2016",
		"CMS_htt_boson_reso_met_1jet_2016","CMS_htt_boson_scale_met_1jet_2016",
		"CMS_htt_boson_reso_met_2jet_2016","CMS_htt_boson_scale_met_2jet_2016"},
	    RecoilVector,
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	  /*
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2016","CMS_htt_boson_scale_met_0jet_2016"},
			      RecoilVector,
			      &cb,
			      1.00,
			      TheFile,
			      {"mt_0jet"});
      
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_1jet_2016","CMS_htt_boson_scale_met_1jet_2016"},
			      RecoilVector,
			      &cb,
			      1.00,
			      TheFile,
			      {"mt_1jet"});

	  if(Input.OptionExists("-dm"))
	    {
	      AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_2jet_2016","CMS_htt_boson_scale_met_2jet_2016"},
				  RecoilVector,
				  &cb,
				  1.00,
				  TheFile,
				  {"mt_2jet"});
	    }
	  else
	    {
	      AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_2jet_2016","CMS_htt_boson_scale_met_2jet_2016"},
				  RecoilVector,
				  &cb,
				  1.00,
				  TheFile,
				  {"mt_2jetlow","mt_2jethigh","mt_3jetlow","mt_3jethigh"});
	    }
	  */
	}
      else
	{
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2016","CMS_htt_boson_scale_met_0jet_2016"},
			      RecoilVector,
			      &cb,
			      1.00,
			      TheFile,
			      {"mt_0jet"});
      
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_1jet_2016","CMS_htt_boson_scale_met_1jet_2016"},
			      RecoilVector,
			      &cb,
			      1.00,
			      TheFile,
			      {"mt_boosted_1J"});

	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_2jet_2016","CMS_htt_boson_scale_met_2jet_2016"},
			      RecoilVector,
			      &cb,
			      1.00,
			      TheFile,
			      {"mt_boosted_GE2J","mt_vbf_PTH_0_200","mt_vbf_PTH_GE_200"});
	}      

      //ZPT Reweighting Shapes:      
      std::cout<<"ZPT Reweighting"<<std::endl;
      AddShapesIfNotEmpty({"CMS_htt_dyShape_2016"},
			  ZPTVector,
			  &cb,
			  1.00,
			  TheFile,CategoryArgs);

      //Top Pt Reweighting      
      std::cout<<"ttbar shape"<<std::endl;
      AddShapesIfNotEmpty({"CMS_htt_ttbarShape"},
			  TopVector,
			  &cb,
			  1.00,
			  TheFile,CategoryArgs);
  
      //TES Uncertainty                  
      std::cout<<"TES"<<std::endl;
      AddShapesIfNotEmpty({"CMS_scale_t_1prong_2016","CMS_scale_t_3prong_2016","CMS_scale_t_1prong1pizero_2016","CMS_scale_t_3prong1pizero_2016"},
			  TESVector,
			  &cb,
			  1.00,
			  TheFile,CategoryArgs);

      // JES
      std::cout<<"JES"<<std::endl;
      AddShapesIfNotEmpty({"CMS_JetAbsolute","CMS_JetAbsolute_2016","CMS_JetBBEC1","CMS_JetBBEC1_2016","CMS_JetEC2","CMS_JetEC2_2016",
	    "CMS_JetFlavorQCD","CMS_JetHF","CMS_JetHF_2016","CMS_JetRelativeBal"},
	JESVector,
	&cb,
	1.000,
	TheFile,CategoryArgs);      

      //JER
      std::cout<<"JER"<<std::endl;
      AddShapesIfNotEmpty({"CMS_JER_2016"},
			  JESVector,
			  &cb,
			  1.000,
			  TheFile,CategoryArgs);

      if (Input.OptionExists("-x0"))
	{
	  std::cout<<"Scaled ggH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_ggH_Mu_norm","THU_ggH_Res_norm","THU_ggH_Mig01_norm","THU_ggH_Mig12_norm","THU_ggH_VBF2j_norm",
		"THU_ggH_VBF3j_norm","THU_ggH_qmtop_norm","THU_ggH_PT60_norm","THU_ggH_PT120_norm"},
	    JoinStr({ggH_STXS,{"ggH_hww125","ggH_htt_nonfid125"}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);            

	  //qqH theory uncertainties
	  std::cout<<"Scaled qqH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_qqH_yield_norm","THU_qqH_PTH200_norm","THU_qqH_Mjj60_norm","THU_qqH_Mjj120_norm","THU_qqH_Mjj350_norm","THU_qqH_Mjj700_norm",
		"THU_qqH_Mjj1000_norm","THU_qqH_Mjj1500_norm","THU_qqH_PTH25_norm","THU_qqH_JET01_norm"},
	    JoinStr({qqH_STXS,{"qqH_hww125","qqH_htt_nonfid125",}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      //unscaled for either mu measurement
      else if(not(Input.OptionExists("-x0")||Input.OptionExists("-x1")))
	{
	  std::cout<<"ggH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_ggH_Mu","THU_ggH_Res","THU_ggH_Mig01","THU_ggH_Mig12","THU_ggH_VBF2j",
		"THU_ggH_VBF3j","THU_ggH_qmtop","THU_ggH_PT60","THU_ggH_PT120"},
	    JoinStr({ggH_STXS,{"ggH_hww125","ggH_htt_nonfid125"}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);            

	  //qqH theory uncertainties
	  std::cout<<"qqH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_qqH_yield","THU_qqH_PTH200","THU_qqH_Mjj60","THU_qqH_Mjj120","THU_qqH_Mjj350","THU_qqH_Mjj700",
		"THU_qqH_Mjj1000","THU_qqH_Mjj1500","THU_qqH_PTH25","THU_qqH_JET01"},
	    JoinStr({qqH_STXS,{"qqH_hww125","qqH_htt_nonfid125",}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}

      //Muon Energy scale uncertainties
      AddShapesIfNotEmpty({"CMS_scale_m_etalt1p2","CMS_scale_m_eta1p2to2p1"},
	MuESVector,
	&cb,
	1.00,
	TheFile,CategoryArgs);
            
      //new theory shapes
      //inclusive shapes
      /*
      AddShapesIfNotEmpty({"ggH_scale"},
			  {"ggH_htt125",
			      "ggZH_had_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"vbf_scale"},
			  {"qqH_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      */
      AddShapesIfNotEmpty({"VH_scale"},
			  {"WH_had_htt125",
			      "ZH_had_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"WHlep_scale"},
			  {"WH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ZHlep_scale"},
			  {"ZH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );

      if(not(Input.OptionExists("-x0")||Input.OptionExists("-x1")))
	{
      //individual STXS bin shapes
	  AddShapesIfNotEmpty({"ggH_scale_0jet"},
			      {"ggH_PTH_0_200_0J_PTH_10_200_htt125",
				  "ggH_PTH_0_200_0J_PTH_0_10_htt125",
				  "ggZH_PTH_0_200_0J_PTH_10_200_htt125",
				  "ggZH_PTH_0_200_0J_PTH_0_10_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"ggH_scale_1jet_lowpt"},
			      {"ggH_PTH_0_200_1J_PTH_0_60_htt125",
				  "ggH_PTH_0_200_1J_PTH_60_120_htt125",
				  "ggH_PTH_0_200_1J_PTH_120_200_htt125",
				  "ggZH_PTH_0_200_1J_PTH_0_60_htt125",
				  "ggZH_PTH_0_200_1J_PTH_60_120_htt125",
				  "ggZH_PTH_0_200_1J_PTH_120_200_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"ggH_scale_2jet_lowpt"},
			      {"ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125",		   
				  "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125",		   
				  "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"
				  "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125",		   
				  "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125",		   
				  "ggZH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"ggH_scale_vbf"},
			      {"ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125",
				  "ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125",
				  "ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125",
				  "ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125",
				  "ggZH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125",
				  "ggZH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125",
				  "ggZH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125",
				  "ggZH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"ggH_scale_highpt"},
			      {"ggH_PTH_200_300_htt125",
				  "ggH_PTH_300_450_htt125",
				  "ggZH_PTH_200_300_htt125",
				  "ggZH_PTH_300_450_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"ggH_scale_very_highpt"},
			      {"ggH_PTH_450_650_htt125",
				  "ggH_PTH_GE650_htt125",
				  "ggZH_PTH_450_650_htt125",
				  "ggZH_PTH_GE650_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );

	  AddShapesIfNotEmpty({"vbf_scale_0jet"},
			      {"qqH_0J_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"vbf_scale_1jet"},
			      {"qqH_1J_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"vbf_scale_lowmjj"},
			      {"qqH_GE2J_MJJ_0_60_htt125",
				  "qqH_GE2J_MJJ_60_120_htt125",
				  "qqH_GE2J_MJJ_120_350_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );

	  AddShapesIfNotEmpty({"vbf_scale_highmjj_lowpt"},
			      {"qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
				  "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
				  "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
				  "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );

	  AddShapesIfNotEmpty({"vbf_scale_highmjj_highpt"},
			      {"qqH_GE2J_MJJ_GE350_PTH_GE200_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );

	  AddShapesIfNotEmpty({"VH_scale_0jet"},
			      {"WH_0J_htt125",
				  "ZH_0J_htt125,"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"VH_scale_1jet"},
			      {"WH_1J_htt125",
				  "ZH_1J_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"VH_scale_lowmjj"},
			      {"WH_GE2J_MJJ_0_60_htt125",
				  "WH_GE2J_MJJ_60_120_htt125",
				  "WH_GE2J_MJJ_120_350_htt125",
				  "ZH_GE2J_MJJ_0_60_htt125",
				  "ZH_GE2J_MJJ_60_120_htt125",
				  "ZH_GE2J_MJJ_120_350_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );

	  AddShapesIfNotEmpty({"VH_scale_highmjj_lowpt"},
			      {"WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
				  "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
				  "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
				  "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );

	  AddShapesIfNotEmpty({"VH_scale_highmjj_highpt"},
			      {"WH_GE2J_MJJ_GE350_PTH_GE200_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_GE200_htt125"},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
      
	  AddShapesIfNotEmpty({"VH_scale_0jet"},
			      {"WH_0J_htt125",
				  "ZH_0J_htt125",},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
      
	  AddShapesIfNotEmpty({"VH_scale_1jet"},
			      {"WH_1J_htt125",
				  "ZH_1J_htt125",},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"VH_scale_lowmjj"},
			      {"WH_GE2J_MJJ_0_60_htt125",
				  "WH_GE2J_MJJ_60_120_htt125",
				  "WH_GE2J_MJJ_120_350_htt125",
				  "ZH_GE2J_MJJ_0_60_htt125",
				  "ZH_GE2J_MJJ_60_120_htt125",
				  "ZH_GE2J_MJJ_120_350_htt125",},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"VH_scale_highmjj_lowpt"},
			      {"WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
				  "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
				  "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
				  "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	  AddShapesIfNotEmpty({"VH_scale_highmjj_highpt"},
			      {"WH_GE2J_MJJ_GE350_PTH_GE200_htt125",
				  "ZH_GE2J_MJJ_GE350_PTH_GE200_htt125",},
			      &cb,
			      1.00,
			      TheFile,
			      CategoryArgs
			      );
	}
      //FIX ME: shapes are valid on split VH_lep, but we do not use seperated VH_lep at the moment.
      /*
      AddShapesIfNotEmpty({"WH_scale_lowpt"},
			  {"WH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ZH_scale_lowpt"},
			  {"ZH_lep_htt125",
			      "ggZH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      
      
      AddShapesIfNotEmpty({"WH_scale_highpt"},
			  {"WH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ZH_scale_highpt"},
			  {"ZH_lep_htt125",
			      "ggZH_lep_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      */
    }
  //**************************************
  //embedded uncertainties. 
  //**************************************
  if(not Input.OptionExists("-e"))
    {
      //50% correlation with ID unc in MC
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_m_2018","lnN",SystMap<>::init(1.010));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_m_embedded_2018","lnN",SystMap<>::init(1.01732));

      //Tracking Uncertainty
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_prong_emb_2016","shape",SystMap<>::init(1.00));
      //cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_prong_2016","shape",SystMap<>::init(1.00));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_1prong1pizero_emb_2016","shape",SystMap<>::init(1.00));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_3prong1pizero_emb_2016","shape",SystMap<>::init(1.00));

      //Tau ID eff
      //cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_t_embedded_2016", "lnN", SystMap<>::init(1.020));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_t_embedded_pt30to35_2016", "shape", SystMap<>::init(1.00));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_t_embedded_pt35to40_2016", "shape", SystMap<>::init(1.00));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_t_embedded_ptgt40_2016", "shape", SystMap<>::init(1.00));
      

      //cb.cp().process({"embedded"}).AddSyst(cb,"CMS_1ProngPi0Eff","lnN",ch::syst::SystMapAsymm<>::init(0.9934,1.011));
      //cb.cp().process({"embedded"}).AddSyst(cb,"CMS_3ProngEff","lnN",ch::syst::SystMapAsymm<>::init(0.969,1.005));

      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_htt_doublemutrg_2016", "lnN", SystMap<>::init(1.04));

      // TTBar Contamination
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_htt_emb_ttbar_2016", "shape", SystMap<>::init(1.00));

      //TES uncertainty
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_t_1prong_2016", "shape", SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_t_1prong1pizero_2016", "shape", SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_t_3prong_2016", "shape", SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_t_3prong1pizero_2016", "shape", SystMap<>::init(0.866));

      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_t_1prong_2016", "shape", SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_t_1prong1pizero_2016", "shape", SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_t_3prong_2016", "shape", SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_t_3prong1pizero_2016", "shape", SystMap<>::init(0.500));

      //Trigger uncertainty
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_singlemutrg_emb_2016","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_mutautrg_emb_2016","shape",SystMap<>::init(0.866));
      
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_singlemutrg_2016","shape",SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_mutautrg_2016","shape",SystMap<>::init(0.500));
      
      //MuonES uncertaity
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_m_etalt1p2","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_m_eta1p2to2p1","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_m_etalt1p2","shape",SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_m_eta1p2to2p1","shape",SystMap<>::init(0.500));
    }
  //********************************************************************************************************************************                          

  if (Input.OptionExists("-c"))
    {
      cb.cp().backgrounds().ExtractShapes(
      aux_shapes + "mt_controls_2016.root",
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
      aux_shapes + "mt_controls_2016.root",
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  else if(Input.OptionExists("-gf"))
    {
      cb.cp().backgrounds().ExtractShapes(
      aux_shapes + "smh2016mt_GOF.root",
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
      aux_shapes + "smh2016mt_GOF.root",
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  else if(Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
    {
      cb.cp().backgrounds().ExtractShapes(
      aux_shapes + "smh2016mt_Differential.root",
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
      aux_shapes + "smh2016mt_Differential.root",
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  else
    {
      cb.cp().backgrounds().ExtractShapes(
      aux_shapes + "smh2016mt.root",
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
      aux_shapes + "smh2016mt.root",
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  
  //! [part7]

  //! [part8]
  
  if (not Input.OptionExists("-b"))
    {
      auto bbb = ch::BinByBinFactory()
	.SetAddThreshold(0.05)
	.SetMergeThreshold(0.5)
	.SetFixNorm(false);
      bbb.MergeBinErrors(cb.cp().backgrounds());
      bbb.AddBinByBin(cb.cp().backgrounds(), cb);
      bbb.AddBinByBin(cb.cp().signals(), cb);
    }  

  /*auto bbb = ch::BinByBinFactory()
    .SetAddThreshold(0.0)
    .SetFixNorm(false);

  //bbb.AddBinByBin(cb.cp().backgrounds(), cb);
  bbb.AddBinByBin(cb.cp().signals(), cb);
  bbb.AddBinByBin(cb.cp().process({"TT"}), cb);
  bbb.AddBinByBin(cb.cp().process({"QCD"}), cb);
  bbb.AddBinByBin(cb.cp().process({"W"}), cb);
  bbb.AddBinByBin(cb.cp().process({"VV"}), cb);
  bbb.AddBinByBin(cb.cp().process({"ZTT"}), cb);
  bbb.AddBinByBin(cb.cp().process({"ZLL"}), cb);
*/
  // This function modifies every entry to have a standardised bin name of
  // the form: {analysis}_{channel}_{bin_id}_{era}
  // which is commonly used in the htt analyses
  ch::SetStandardBinNames(cb);
  //! [part8]

  //! [part9]
  // First we generate a set of bin names:
  set<string> bins = cb.bin_set();
  // This method will produce a set of unique bin names by considering all
  // Observation, Process and Systematic entries in the CombineHarvester
  // instance.

  // We create the output root file that will contain all the shapes.
  TFile output((Input.ReturnToken(0)+"/"+"smh2016_mt.input.root").c_str(), "RECREATE");

  // Finally we iterate through each bin,mass combination and write a
  // datacard.
  for (auto b : bins) {
    for (auto m : masses) {
      cout << ">> Writing datacard for bin: " << b << " and mass: " << m
           << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must remember to include the "*" mass entry to get
      // all the data and backgrounds.
      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(Input.ReturnToken(0)+"/"+b + "_" + m + ".txt", output);
    }
  }
  //! [part9]

}
