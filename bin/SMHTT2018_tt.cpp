//Andrew Loeliger
//Input options: 
// -s disables shape uncertainties
// -e disables embedded
// -b disables bin-by-bin uncertainties
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
  if (Input.OptionExists("-c")) TheFile = new TFile((aux_shapes+"tt_controls_2018.root").c_str());
  else if (Input.OptionExists("-gf")) TheFile = new TFile((aux_shapes+"smh2018tt_GOF.root").c_str());
  else if (Input.OptionExists("-dp") or Input.OptionExists("-dn") or Input.OptionExists("-dm")) TheFile = new TFile((aux_shapes+"smh2018tt_Differential.root").c_str());
  else TheFile = new TFile((aux_shapes+"smh2018tt.root").c_str());

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

  vector<string> masses = {""};;
  //! [part3]
  cb.AddObservations({"*"}, {"smh2018"}, {"13TeV"}, {"tt"}, cats);

  
  vector<string> bkg_procs = {"embedded","jetFakes","VVL","STL","TTL","ZL"};
  if(Input.OptionExists("-e")) { // disable embed
    bkg_procs.erase(std::remove(bkg_procs.begin(), bkg_procs.end(), "embedded"), bkg_procs.end());
    bkg_procs.push_back("ZT");
    bkg_procs.push_back("VVT");
    bkg_procs.push_back("TTT");
    bkg_procs.push_back("STT");
  }
  /*if (Input.OptionExists("-dp") || Input.OptionExists("-dn") || Input.OptionExists("-dm"))
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
    }*/

  cb.AddProcesses({"*"}, {"smh2018"}, {"13TeV"}, {"tt"}, bkg_procs, cats, false);

  vector<string> ggH_STXS;
  if (Input.OptionExists("-g")) ggH_STXS = {"ggH_htt125"};
  //PTH differential option
  else if (Input.OptionExists("-dp")) ggH_STXS = {
      "ggH_PTH_0_20_htt125",
      "ggH_PTH_20_45_htt125",
      "ggH_PTH_45_80_htt125",
      "ggH_PTH_80_120_htt125",
      "ggH_PTH_120_200_htt125",
      "ggH_PTH_200_350_htt125",
      "ggH_PTH_350_600_htt125",
      "ggH_PTH_GE600_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) ggH_STXS = {
      "ggH_NJETS_0_htt125",
      "ggH_NJETS_1_htt125",
      "ggH_NJETS_2_htt125",
      "ggH_NJETS_GE3_htt125",
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
      "qqH_PTH_0_20_htt125",
      "qqH_PTH_20_45_htt125",
      "qqH_PTH_45_80_htt125",
      "qqH_PTH_80_120_htt125",
      "qqH_PTH_120_200_htt125",
      "qqH_PTH_200_350_htt125",
      "qqH_PTH_350_600_htt125",
      "qqH_PTH_GE600_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) qqH_STXS = {
      "qqH_NJETS_0_htt125",
      "qqH_NJETS_1_htt125",
      "qqH_NJETS_2_htt125",
      "qqH_NJETS_GE3_htt125",
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
  //if (Input.OptionExists("-q")) WH_STXS = {"WH_lep_htt125","WH_had_htt125"};
  if (Input.OptionExists("-q")) WH_STXS = {"WH_htt125"};
  else if (Input.OptionExists("-dp")) WH_STXS = {
      "WH_PTH_0_20_htt125",
      "WH_PTH_20_45_htt125",
      "WH_PTH_45_80_htt125",
      "WH_PTH_80_120_htt125",
      "WH_PTH_120_200_htt125",
      "WH_PTH_200_350_htt125",
      "WH_PTH_350_600_htt125",
      "WH_PTH_GE600_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) WH_STXS = {
      "WH_NJETS_0_htt125",
      "WH_NJETS_1_htt125",
      "WH_NJETS_2_htt125",
      "WH_NJETS_GE3_htt125",
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
  /*else WH_STXS = {
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
    };*/
  else WH_STXS = {"WH_htt125"};

  vector<string> ZH_STXS;
  //if (Input.OptionExists("-q")) ZH_STXS = {"ZH_lep_htt125","ZH_had_htt125"};
  if (Input.OptionExists("-q")) ZH_STXS = {"ZH_htt125"};
  else if (Input.OptionExists("-dp")) ZH_STXS = {
      "ZH_PTH_0_20_htt125",
      "ZH_PTH_20_45_htt125",
      "ZH_PTH_45_80_htt125",
      "ZH_PTH_80_120_htt125",
      "ZH_PTH_120_200_htt125",
      "ZH_PTH_200_350_htt125",
      "ZH_PTH_350_600_htt125",
      "ZH_PTH_GE600_htt125",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) ZH_STXS = {
      "ZH_NJETS_0_htt125",
      "ZH_NJETS_1_htt125",
      "ZH_NJETS_2_htt125",
      "ZH_NJETS_GE3_htt125",
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
  /*else ZH_STXS = {
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
    };*/
  else ZH_STXS = {"ZH_htt125"};
  
  vector<string> ggZH_STXS;
  if (Input.OptionExists("-g")) ggZH_STXS = {"ggZH_lep_htt125","ggZH_had_htt125"};
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
  cb.AddProcesses(masses, {"smh2018"}, {"13TeV"}, {"tt"}, sig_procs, cats, true);

  //! [part4]

  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;
  using ch::JoinStr;
  //**************************************************
  //start with lnN errors
  //**************************************************

  //Theory uncertainties
  cb.cp().process(sig_procs).AddSyst(cb, "BR_htt_PU_alphas", "lnN", SystMap<>::init(1.0062));
  cb.cp().process(sig_procs).AddSyst(cb, "BR_htt_PU_mq", "lnN", SystMap<>::init(1.0099));
  cb.cp().process(sig_procs).AddSyst(cb, "BR_htt_THU", "lnN", SystMap<>::init(1.017));  
  cb.cp().process(JoinStr({WH_STXS,{"WH_hww125","WH_htt_nonfid125"}})).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.008));
  cb.cp().process(JoinStr({ZH_STXS,{"ZH_hww125","ZH_htt_nonfid125"}})).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.009));
  //cb.cp().process(JoinStr({qqH_STXS,{"qqH_hww125"}})).AddSyst(cb, "QCDScale_qqH", "lnN", SystMap<>::init(1.005));
  cb.cp().process(JoinStr({WH_STXS,{"WH_hww125","WH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.018));
  cb.cp().process(JoinStr({ZH_STXS,{"ZH_hww125","ZH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.013));
  cb.cp().process(JoinStr({ggH_STXS,{"ggH_hww125","ggH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_gg", "lnN", SystMap<>::init(1.032));
  cb.cp().process(JoinStr({qqH_STXS,{"qqH_hww125","qqH_htt_nonfid125"}})).AddSyst(cb, "pdf_Higgs_qq", "lnN", SystMap<>::init(1.021));


  // XSection Uncertainties
  cb.cp().process({"TTT","TTL"}).AddSyst(cb,"CMS_htt_tjXsec", "lnN", SystMap<>::init(1.042));
  cb.cp().process({"VVT","VVL"}).AddSyst(cb,"CMS_htt_vvXsec", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"STT","STL"}).AddSyst(cb,"CMS_htt_stXsec", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"ZT","ZL"}).AddSyst(cb,"CMS_htt_zjXsec", "lnN", SystMap<>::init(1.02));
  
  //Luminosity Uncertainty
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_Run2018", "lnN", SystMap<>::init(1.020));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_XYfactorization", "lnN", SystMap<>::init(1.008));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_lengthScale", "lnN", SystMap<>::init(1.003));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_beamBeamDeflection", "lnN", SystMap<>::init(1.004));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_dynamicBeta", "lnN", SystMap<>::init(1.005));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_beamCurrentCalibration", "lnN", SystMap<>::init(1.003));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}})).AddSyst(cb, "lumi_ghostsAndSatellites", "lnN", SystMap<>::init(1.001));

  cb.cp().process({"VVL","STL","ZL","TTL"}).AddSyst(cb, "CMS_efaketau_tt_2018", "lnN", SystMap<>::init(1.50));

  cb.cp().process({"jetFakes"}).bin({"tt_0jet"}).AddSyst(cb,"CMS_jetFakesNorm_0jet_tt_2018","lnN",SystMap<>::init(1.05));
  cb.cp().process({"jetFakes"}).bin({"tt_boosted_onejet"}).AddSyst(cb,"CMS_jetFakesNorm_1jet_tt_2018","lnN",SystMap<>::init(1.05));
  cb.cp().process({"jetFakes"}).bin({"tt_boosted_multijet"}).AddSyst(cb,"CMS_jetFakesNorm_2jet_tt_2018","lnN",SystMap<>::init(1.05));
  cb.cp().process({"jetFakes"}).bin({"tt_vbf_highHpT"}).AddSyst(cb,"CMS_jetFakesNorm_2jet_tt_2018","lnN",SystMap<>::init(1.05));
  cb.cp().process({"jetFakes"}).bin({"tt_vbf_lowHpT"}).AddSyst(cb,"CMS_jetFakesNorm_2jet_tt_2018","lnN",SystMap<>::init(1.05));


  //***********************************************************************
  //shape uncertainties
  //***********************************************************************
  if(not Input.OptionExists("-s"))
    {
      std::cout<<"Adding Shapes..."<<std::endl;
      //uses custom defined utility function that only adds the shape if at least one shape inside is not empty.

      // Tau ID eff in DM bins
      std::cout<<"Tau ID eff"<<std::endl;
      AddShapesIfNotEmpty({"CMS_tauideff_dm0_2018","CMS_tauideff_dm1_2018","CMS_tauideff_dm10_2018","CMS_tauideff_dm11_2018"},
                          JoinStr({sig_procs,{"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);

      // Trg eff. 

      std::cout<<"Trigger eff"<<std::endl;
      AddShapesIfNotEmpty({"CMS_doubletautrg_dm0_2018","CMS_doubletautrg_dm1_2018","CMS_doubletautrg_dm10_2018","CMS_doubletautrg_dm11_2018"},
                          JoinStr({sig_procs,{"VVL","STL","TTL","ZL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);


      //Fake factors
      std::cout<<"Fake factors"<<std::endl;
      if (Input.OptionExists("-c"))
	{
	  AddShapesIfNotEmpty({"CMS_rawFF_tt_qcd_0jet_2018",
		"CMS_rawFF_tt_qcd_1jet_2018",
		"CMS_rawFF_tt_qcd_2jet_2018",
		"CMS_FF_closure_tau2pt_tt_qcd_0jet",
		"CMS_FF_closure_tau2pt_tt_qcd_1jet",
		"CMS_FF_closure_tau2pt_tt_qcd_2jet",
		"CMS_FF_closure_tt_qcd_osss_2018",},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      else if (Input.OptionExists("-dp") || Input.OptionExists("-dn") || Input.OptionExists("-dm"))
	{      
	  AddShapesIfNotEmpty({"CMS_rawFF_tt_qcd_0jet_2018",	    
		"CMS_FF_closure_tau2pt_tt_qcd_0jet",
		"CMS_FF_closure_tt_qcd_osss_2018",},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"tt_0jet"});

	  AddShapesIfNotEmpty({"CMS_rawFF_tt_qcd_1jet_2018",	    
		"CMS_FF_closure_tau2pt_tt_qcd_1jet",
		"CMS_FF_closure_tt_qcd_osss_2018",},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"tt_1jet"});
	  if(Input.OptionExists("-dm"))
	    {
	      AddShapesIfNotEmpty({"CMS_rawFF_tt_qcd_2jet_2018",	    
		    "CMS_FF_closure_tau2pt_tt_qcd_2jet",
		    "CMS_FF_closure_tt_qcd_osss_2018",},
		{"jetFakes"},
		&cb,
		1.00,
		TheFile,
		{"tt_2jet"});
	    }
	  else
	    {
	      AddShapesIfNotEmpty({"CMS_rawFF_tt_qcd_2jet_2018",	    
		    "CMS_FF_closure_tau2pt_tt_qcd_2jet",
		    "CMS_FF_closure_tt_qcd_osss_2018",},
		{"jetFakes"},
		&cb,
		1.00,
		TheFile,
		{"tt_2jetlow","tt_2jethigh","tt_3jetlow","tt_3jethigh"});
	    }
	}
      else
	{
	  AddShapesIfNotEmpty({"CMS_rawFF_tt_qcd_0jet_2018",	    
		"CMS_FF_closure_tau2pt_tt_qcd_0jet",
		"CMS_FF_closure_tt_qcd_osss_2018",},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"tt_0jet"});

	  AddShapesIfNotEmpty({"CMS_rawFF_tt_qcd_1jet_2018",	    
		"CMS_FF_closure_tau2pt_tt_qcd_1jet",
		"CMS_FF_closure_tt_qcd_osss_2018",},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"tt_boosted_onejet"});

	  AddShapesIfNotEmpty({"CMS_rawFF_tt_qcd_2jet_2018",	    
		"CMS_FF_closure_tau2pt_tt_qcd_2jet",
		"CMS_FF_closure_tt_qcd_osss_2018",},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,
	    {"tt_boosted_multijet","tt_vbf_highHpT","tt_vbf_lowHpT"});
	}

      //MET Unclustered Energy Scale      
      std::cout<<"MET UES"<<std::endl;
      AddShapesIfNotEmpty({"CMS_scale_met_unclustered_2018"},
                          {"TTL","VVL","STL"},
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);

      //Recoil Shapes:                  
      std::cout<<"Recoil shapes"<<std::endl;
      if (Input.OptionExists("-c"))
	{
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2018","CMS_htt_boson_scale_met_0jet_2018",
		"CMS_htt_boson_reso_met_1jet_2018","CMS_htt_boson_scale_met_1jet_2018",
		"CMS_htt_boson_reso_met_2jet_2018","CMS_htt_boson_scale_met_2jet_2018"},
	    JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125"}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      else if (Input.OptionExists("-dp") or Input.OptionExists("-dn") or Input.OptionExists("-dm"))
	{
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2018","CMS_htt_boson_scale_met_0jet_2018"},
			      JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125"}}),
			      &cb,
			      1.00,
			      TheFile,
			      {"tt_0jet"});
      
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_1jet_2018","CMS_htt_boson_scale_met_1jet_2018"},
			      JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125"}}),
			      &cb,
			      1.00,
			      TheFile,
			      {"tt_1jet"});

	  if(Input.OptionExists("-dm"))
	    {
	      AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_2jet_2018","CMS_htt_boson_scale_met_2jet_2018"},
				  JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125"}}),
				  &cb,
				  1.00,
				  TheFile,
				  {"tt_2jet"});
	    }
	  else
	    {
	      AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_2jet_2018","CMS_htt_boson_scale_met_2jet_2018"},
				  JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125"}}),
				  &cb,
				  1.00,
				  TheFile,
				  {"tt_2jetlow","tt_2jethigh","tt_3jetlow","tt_3jethigh"});
	    }
	}
      else
	{
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_0jet_2018","CMS_htt_boson_scale_met_0jet_2018"},
			      JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125"}}),
			      &cb,
			      1.00,
			      TheFile,
			      {"tt_0jet"});
      
	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_1jet_2018","CMS_htt_boson_scale_met_1jet_2018"},
			      JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125"}}),
			      &cb,
			      1.00,
			      TheFile,
			      {"tt_boosted_onejet"});

	  AddShapesIfNotEmpty({"CMS_htt_boson_reso_met_2jet_2018","CMS_htt_boson_scale_met_2jet_2018"},
			      JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125"}}),
			      &cb,
			      1.00,
			      TheFile,
			      {"tt_boosted_multijet","tt_vbf_highHpT","tt_vbf_lowHpT"});
	}

      //ZPT Reweighting Shapes:      
      std::cout<<"ZPT Reweighting"<<std::endl;
      AddShapesIfNotEmpty({"CMS_htt_dyShape"},
                          {"ZL"},
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);

      //Top Pt Reweighting      
      std::cout<<"ttbar shape"<<std::endl;
      AddShapesIfNotEmpty({"CMS_htt_ttbarShape"},
                          {"TTL"},
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);

      //TES Uncertainty                  
      std::cout<<"TES"<<std::endl;
      AddShapesIfNotEmpty({"CMS_scale_t_1prong_2018","CMS_scale_t_3prong_2018","CMS_scale_t_1prong1pizero_2018","CMS_scale_t_3prong1pizero_2018"},
                          JoinStr({sig_procs,{"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
                          &cb,
                          1.00,
                          TheFile,CategoryArgs);

      std::cout<<"JES"<<std::endl;
      AddShapesIfNotEmpty({"CMS_JetAbsolute","CMS_JetAbsolute_2018","CMS_JetBBEC1","CMS_JetBBEC1_2018","CMS_JetEC2","CMS_JetEC2_2018",
	    "CMS_JetFlavorQCD","CMS_JetHF","CMS_JetHF_2018","CMS_JetRelativeSample_2018","CMS_JetRelativeBal"},
	JoinStr({sig_procs,{"VVL","STL","ZL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
	&cb,
	1.000,
	TheFile,CategoryArgs);

      //JER      
      AddShapesIfNotEmpty({"CMS_JER_2018"},
			  JoinStr({sig_procs,{"VVL","STL","ZL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","ggH_htt_nonfid125","qqH_htt_nonfid125","WH_htt_nonfid125","ZH_htt_nonfid125"}}),
			  &cb,
			  1.000,
			  TheFile,CategoryArgs);
      //ggH Theory Uncertainties
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
	JoinStr({qqH_STXS,{"qqH_hww125","qqH_htt_nonfid125"}}),
	&cb,
	1.00,
	TheFile,CategoryArgs);
      
      AddShapesIfNotEmpty({"ggH_scale_0jet"},
			  {"ggH_htt125",
			      "ggZH_had_htt125",
			      "ggH_PTH_0_200_0J_PTH_10_200_htt125",
			      "ggH_PTH_0_200_0J_PTH_0_10_htt125",
			      "ggZH_PTH_0_200_0J_PTH_10_200_htt125",
			      "ggZH_PTH_0_200_0J_PTH_0_10_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ggH_scale_1jet_lowpt"},
			  {"ggH_htt125",
			      "ggZH_had_htt125",
			      "ggH_PTH_0_200_1J_PTH_0_60_htt125",
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
			  {"ggH_htt125",
			      "ggZH_had_htt125",
			      "ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125",		   
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
			  {"ggH_htt125",
			      "ggZH_had_htt125",
			      "ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125",
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
			  {"ggH_htt125",
			      "ggZH_had_htt125",
			      "ggH_PTH_200_300_htt125",
			      "ggH_PTH_300_450_htt125",
			      "ggZH_PTH_200_300_htt125",
			      "ggZH_PTH_300_450_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"ggH_scale_very_highpt"},
			  {"ggH_htt125",
			      "ggZH_had_htt125",
			      "ggH_PTH_450_650_htt125",
			      "ggH_PTH_GE650_htt125",
			      "ggZH_PTH_450_650_htt125",
			      "ggZH_PTH_GE650_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );

      AddShapesIfNotEmpty({"vbf_scale_0jet"},
			  {"qqH_htt125",
			      "WH_had_htt125",
			      "ZH_had_htt125"
			      "qqH_0J_htt125",
			      "WH_0J_htt125",
			      "ZH_0J_htt125,"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"vbf_scale_1jet"},
			  {"qqH_htt125",
			      "WH_had_htt125",
			      "ZH_had_htt125",
			      "qqH_1J_htt125",
			      "WH_1J_htt125",
			      "ZH_1J_htt125"},
			  &cb,
			  1.00,
			  TheFile,
			  CategoryArgs
			  );
      AddShapesIfNotEmpty({"vbf_scale_lowmjj"},
			  {"qqH_htt125",
			      "WH_had_htt125",
			      "ZH_had_htt125",
			      "qqH_GE2J_MJJ_0_60_htt125",
			      "qqH_GE2J_MJJ_60_120_htt125",
			      "qqH_GE2J_MJJ_120_350_htt125",
			      "WH_GE2J_MJJ_0_60_htt125",
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

      AddShapesIfNotEmpty({"vbf_scale_highmjj_lowpt"},
			  {"qqH_htt125",
			      "WH_had_htt125",
			      "ZH_had_htt125",
			      "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
			      "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125",
			      "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125",
			      "qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125",
			      "WH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125",
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

      AddShapesIfNotEmpty({"vbf_scale_highmjj_highpt"},
			  {"qqH_htt125",
			      "WH_had_htt125",
			      "ZH_had_htt125",
			      "qqH_GE2J_MJJ_GE350_PTH_GE200_htt125",
			      "WH_GE2J_MJJ_GE350_PTH_GE200_htt125",
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
    }


  //*****************************************************************
  //embedded uncertainties. 
  //*****************************************************************
  if(not Input.OptionExists("-e"))
    {      
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_htt_doublemutrg_2018", "lnN", SystMap<>::init(1.04));

      // Tau ID eff
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_t_embedded_dm0_2018", "shape", SystMap<>::init(1.000));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_t_embedded_dm1_2018", "shape", SystMap<>::init(1.000));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_t_embedded_dm10_2018", "shape", SystMap<>::init(1.000));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_eff_t_embedded_dm11_2018", "shape", SystMap<>::init(1.000));


      // TTBar Contamination
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_htt_emb_ttbar_2018", "shape", SystMap<>::init(1.00));

      //TES uncertainty
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_t_1prong_2018", "shape", SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_t_1prong1pizero_2018", "shape", SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_t_3prong_2018", "shape", SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_emb_t_3prong1pizero_2018", "shape", SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_t_1prong_2018", "shape", SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_t_1prong1pizero_2018", "shape", SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_t_3prong_2018", "shape", SystMap<>::init(0.500));     
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_scale_t_3prong1pizero_2018", "shape", SystMap<>::init(0.500));

      //Trigger Uncertainty
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_doubletautrg_emb_dm0_2018","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_doubletautrg_dm0_2018","shape",SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_doubletautrg_emb_dm1_2018","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_doubletautrg_dm1_2018","shape",SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_doubletautrg_emb_dm10_2018","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_doubletautrg_dm10_2018","shape",SystMap<>::init(0.500));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_doubletautrg_emb_dm11_2018","shape",SystMap<>::init(0.866));
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_doubletautrg_dm11_2018","shape",SystMap<>::init(0.500));

    }

  //********************************************************************************************************************************                          

  if (Input.OptionExists("-c"))
    {
      cb.cp().backgrounds().ExtractShapes(
					  aux_shapes + "tt_controls_2018.root",
					  "$BIN/$PROCESS",
					  "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
				      aux_shapes + "tt_controls_2018.root",
				      "$BIN/$PROCESS$MASS",
				      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  else if(Input.OptionExists("-gf"))
    {
      cb.cp().backgrounds().ExtractShapes(
                      aux_shapes + "smh2018tt_GOF.root",
                      "$BIN/$PROCESS",
                      "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
                      aux_shapes + "smh2018tt_GOF.root",
                      "$BIN/$PROCESS$MASS",
                      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  else if(Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm"))
    {
      cb.cp().backgrounds().ExtractShapes(
      aux_shapes + "smh2018tt_Differential.root",
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
      aux_shapes + "smh2018tt_Differential.root",
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  else
    {
      cb.cp().backgrounds().ExtractShapes(
					  aux_shapes + "smh2018tt.root",
					  "$BIN/$PROCESS",
					  "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().signals().ExtractShapes(
				      aux_shapes + "smh2018tt.root",
				      "$BIN/$PROCESS$MASS",
				      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
  //! [part7]

  //! [part8]
  
  //TODO: Ongoing effort to move to autoMCstats instead of combineharvester bin-by-bin.
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
  //TFile output("smh2018_tt.input.root", "RECREATE");
  TFile output((Input.ReturnToken(0)+"/"+"smh2018_tt.input.root").c_str(), "RECREATE");

  // Finally we iterate through each bin,mass combination and write a
  // datacard.
  for (auto b : bins) {
    for (auto m : masses) {
      cout << ">> Writing datacard for bin: " << b << " and mass: " << m
           << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must remember to include the "*" mass entry to get
      // all the data and backgrounds.
      //cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(
      //    b + "_" + m + ".txt", output);
      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(Input.ReturnToken(0)+"/"+b + "_" + m + ".txt", output);
    }
  }
  //! [part9]

}
