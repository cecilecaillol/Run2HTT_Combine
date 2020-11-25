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
#include "CombineHarvester/CombineTools/interface/AutoRebin.h"

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
  if (Input.OptionExists("-dp") || Input.OptionExists("-dn") || Input.OptionExists("-dm")||Input.OptionExists("-dljpt")) bkg_procs.push_back("OutsideAcceptance");      
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
  else if (Input.OptionExists("-dp")) ggH_STXS = {
      "ggH_PTH_0_45",
      "ggH_PTH_45_80",
      "ggH_PTH_80_120",
      "ggH_PTH_120_200",
      "ggH_PTH_200_350",
      "ggH_PTH_350_450",
      "ggH_PTH_GT450",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) ggH_STXS = {
      "ggH_NJ_0",
      "ggH_NJ_1",
      "ggH_NJ_2",
      "ggH_NJ_3",
      "ggH_NJ_GE4",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) ggH_STXS = {
      "ggH_MJJ_0_150",
      "ggH_MJJ_150_300",
      "ggH_MJJ_300_450",
      "ggH_MJJ_450_600",
      "ggH_MJJ_600_1000",
      "ggH_MJJ_1000_1400",
      "ggH_MJJ_1400_1800",
      "ggH_MJJ_GE1800",
    };
  else if (Input.OptionExists("-dljpt")) ggH_STXS = {
      "ggH_NJ_0",
      "ggH_J1PT_30_60",
      "ggH_J1PT_60_120",
      "ggH_J1PT_120_200",
      "ggH_J1PT_200_350",
      "ggH_J1PT_GT350",
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
      "xH_PTH_0_45",
      "xH_PTH_45_80",
      "xH_PTH_80_120",
      "xH_PTH_120_200",
      "xH_PTH_200_350",
      "xH_PTH_350_450",
      "xH_PTH_GT450",
    };
  //NJets differential Option
  else if (Input.OptionExists("-dn")) qqH_STXS = {
      "xH_NJ_0",
      "xH_NJ_1",
      "xH_NJ_2",
      "xH_NJ_3",
      "xH_NJ_GE4",
    };
  //mjj differential option
  else if (Input.OptionExists("-dm")) qqH_STXS = {
      "xH_MJJ_0_150",
      "xH_MJJ_150_300",
      "xH_MJJ_300_450",
      "xH_MJJ_450_600",
      "xH_MJJ_600_1000",
      "xH_MJJ_1000_1400",
      "xH_MJJ_1400_1800",
      "xH_MJJ_GE1800",
    };
  else if (Input.OptionExists("-dljpt")) qqH_STXS = {
      "xH_NJ_0",
      "xH_J1PT_30_60",
      "xH_J1PT_60_120",
      "xH_J1PT_120_200",
      "xH_J1PT_200_350",
      "xH_J1PT_GT350",
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
  else if (Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt")) WH_STXS = {};  
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
  else if (Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt")) ZH_STXS = {};  
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
      //cb.cp().process(JoinStr({WH_STXS,{"WH_hww125","WH_htt_nonfid125"}})).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.008));
      //cb.cp().process(JoinStr({ZH_STXS,{"ZH_hww125","ZH_htt_nonfid125"}})).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.009));
      cb.cp().process(JoinStr({ggH_STXS,{"ggH_hww125"}})).AddSyst(cb, "pdf_Higgs_gg", "lnN", SystMap<>::init(1.032));
      if(not(Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt")))
	{
	  cb.cp().process(JoinStr({qqH_STXS,{"qqH_hww125"}})).AddSyst(cb, "pdf_Higgs_qq", "lnN", SystMap<>::init(1.021));
	  cb.cp().process(JoinStr({WH_STXS,{"WH_hww125"}})).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.018));
	  cb.cp().process(JoinStr({ZH_STXS,{"ZH_hww125"}})).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.013));      
	}
    }
  cb.cp().process({"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125"}).AddSyst(cb, "BR_hww_PU_alphas", "lnN", ch::syst::SystMapAsymm<>::init(1.0066,1.0063));
  cb.cp().process({"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125"}).AddSyst(cb, "BR_hww_PU_mq", "lnN", ch::syst::SystMapAsymm<>::init(1.0099,1.0098));
  cb.cp().process({"ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125"}).AddSyst(cb, "BR_hww_THU", "lnN", SystMap<>::init(1.0099));  
  //cb.cp().process(JoinStr({qqH_STXS,{"qqH_hww125"}})).AddSyst(cb, "QCDScale_qqH", "lnN", SystMap<>::init(1.005));  

  cb.cp().process({"WH_had_htt125","WH_lep_htt125","WH_hww125","WH_lep_PTV_0_75_htt125","WH_lep_PTV_75_150_htt125","WH_lep_PTV_150_250_0J_htt125","WH_lep_PTV_PTV_150_250_GE1J_htt125","WH_lep_PTV_GT250_htt125"}).AddSyst(cb, "THU_WH_inc", "lnN", ch::syst::SystMapAsymm<>::init(0.993,1.005));
  cb.cp().process({"WH_lep_PTV_0_75_htt125"}).AddSyst(cb, "THU_WH_mig75", "lnN", SystMap<>::init(0.965));
  cb.cp().process({"WH_lep_PTV_75_150_htt125","WH_lep_PTV_150_250_0J_htt125","WH_lep_PTV_150_250_GE1J_htt125","WH_lep_PTV_GT250_htt125"}).AddSyst(cb, "THU_WH_mig75", "lnN", SystMap<>::init(1.039));
  cb.cp().process({"WH_lep_PTV_75_150_htt125"}).AddSyst(cb,"THU_WH_mig150","lnN", SystMap<>::init(0.995));
  cb.cp().process({"WH_lep_PTV_150_250_0J_htt125","WH_lep_PTV_150_250_GE1J_htt125","WH_lep_PTV_GT250_htt125"}).AddSyst(cb, "THU_WH_mig150", "lnN", SystMap<>::init(1.013));
  cb.cp().process({"WH_lep_PTV_150_250_0J","WH_lep_PTV_150_250_GE1J"}).AddSyst(cb, "THU_WH_mig250", "lnN", SystMap<>::init(0.9958));
  cb.cp().process({"WH_lep_PTV_GT250"}).AddSyst(cb, "THU_WH_mig250", "lnN", SystMap<>::init(1.014));
  cb.cp().process({"WH_lep_PTV_150_250_0J"}).AddSyst(cb, "THU_WH_mig01", "lnN", SystMap<>::init(0.961));
  cb.cp().process({"WH_lep_PTV_150_250_GE1J"}).AddSyst(cb, "THU_WH_mig01", "lnN", SystMap<>::init(1.053));

  cb.cp().process({"ZH_had_htt125","ZH_lep_htt125","ZH_hww125","ZH_lep_PTV_0_75_htt125","ZH_lep_PTV_75_150_htt125","ZH_lep_PTV_150_250_0J_htt125","ZH_lep_PTV_PTV_150_250_GE1J_htt125","ZH_lep_PTV_GT250_htt125"}).AddSyst(cb, "THU_ZH_inc", "lnN", ch::syst::SystMapAsymm<>::init(0.994,1.005));
  cb.cp().process({"ZH_lep_PTV_0_75_htt125"}).AddSyst(cb, "THU_ZH_mig75", "lnN", SystMap<>::init(0.963));
  cb.cp().process({"ZH_lep_PTV_75_150_htt125","ZH_lep_PTV_150_250_0J_htt125","ZH_lep_PTV_150_250_GE1J_htt125","ZH_lep_PTV_GT250_htt125"}).AddSyst(cb, "THU_ZH_mig75", "lnN", SystMap<>::init(1.04));
  cb.cp().process({"ZH_lep_PTV_75_150_htt125"}).AddSyst(cb,"THU_ZH_mig150","lnN", SystMap<>::init(0.995));
  cb.cp().process({"ZH_lep_PTV_150_250_0J_htt125","ZH_lep_PTV_150_250_GE1J_htt125","ZH_lep_PTV_GT250_htt125"}).AddSyst(cb, "THU_ZH_mig150", "lnN", SystMap<>::init(1.013));
  cb.cp().process({"ZH_lep_PTV_150_250_0J","ZH_lep_PTV_150_250_GE1J"}).AddSyst(cb, "THU_ZH_mig250", "lnN", SystMap<>::init(0.9958));
  cb.cp().process({"ZH_lep_PTV_GT250"}).AddSyst(cb, "THU_ZH_mig250", "lnN", SystMap<>::init(1.014));
  cb.cp().process({"ZH_lep_PTV_150_250_0J"}).AddSyst(cb, "THU_ZH_mig01", "lnN", SystMap<>::init(0.956));
  cb.cp().process({"ZH_lep_PTV_150_250_GE1J"}).AddSyst(cb, "THU_ZH_mig01", "lnN", SystMap<>::init(1.053));

  cb.cp().process({"ggZH_had_htt125","ggZH_lep_htt125","ggZH_lep_PTV_0_75_htt125","ggZH_lep_PTV_75_150_htt125","ggZH_lep_PTV_150_250_0J_htt125","ggZH_lep_PTV_PTV_150_250_GE1J_htt125","ggZH_lep_PTV_GT250_htt125"}).AddSyst(cb, "THU_ggZH_inc", "lnN", ch::syst::SystMapAsymm<>::init(0.811,1.251));
  cb.cp().process({"ggZH_lep_PTV_0_75_htt125"}).AddSyst(cb, "THU_ggZH_mig75", "lnN", ch::syst::SystMapAsymm<>::init(1.9,0.1));
  cb.cp().process({"ggZH_lep_PTV_75_150_htt125","ggZH_lep_PTV_150_250_0J_htt125","ggZH_lep_PTV_150_250_GE1J_htt125","ggZH_lep_PTV_GT250_htt125"}).AddSyst(cb, "THU_ggZH_mig75", "lnN", SystMap<>::init(1.27));
  cb.cp().process({"ggZH_lep_PTV_75_150_htt125"}).AddSyst(cb,"THU_ggZH_mig150","lnN", SystMap<>::init(0.882));
  cb.cp().process({"ggZH_lep_PTV_150_250_0J_htt125","ggZH_lep_PTV_150_250_GE1J_htt125","ggZH_lep_PTV_GT250_htt125"}).AddSyst(cb, "THU_ggZH_mig150", "lnN", SystMap<>::init(1.142));
  cb.cp().process({"ggZH_lep_PTV_150_250_0J","ggZH_lep_PTV_150_250_GE1J"}).AddSyst(cb, "THU_ggZH_mig250", "lnN", SystMap<>::init(0.963));
  cb.cp().process({"ggZH_lep_PTV_GT250"}).AddSyst(cb, "THU_ggZH_mig250", "lnN", SystMap<>::init(1.154));
  cb.cp().process({"ggZH_lep_PTV_150_250_0J"}).AddSyst(cb, "THU_ggZH_mig01", "lnN", ch::syst::SystMapAsymm<>::init(1.6,0.393));
  cb.cp().process({"ggZH_lep_PTV_150_250_GE1J"}).AddSyst(cb, "THU_ggZH_mig01", "lnN", SystMap<>::init(1.277));

  if(not (Input.OptionExists("-x1")))
    {
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
    }

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
  cb.cp().process(JoinStr({{"ZT","TTT","VVT","STT","ZL","TTL","VVL","STL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_eff_m_2016","lnN",SystMap<>::init(1.02));

  // Against ele and against mu for real taus
  //cb.cp().process(JoinStr({{"ZT","TTT","VVT","STT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_eff_t_againstemu_mt_2016","lnN",SystMap<>::init(1.01));

    // b-tagging efficiency
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_eta","lnN",SystMap<>::init(1.005));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_btag_eta","lnN",SystMap<>::init(1.001));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_hf","lnN",SystMap<>::init(0.993));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_btag_hf","lnN",SystMap<>::init(1.002));
  
  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_hfstats1_2016","lnN",SystMap<>::init(1.03));
  //cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_btag_hfstats1_2016","lnN",SystMap<>::init(1.0000));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_hfstats2_2016","lnN",SystMap<>::init(1.015));
  //cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_hfstats2_2016","lnN",SystMap<>::init(1.000));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_jes","lnN",SystMap<>::init(0.98));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_btag_jes","lnN",SystMap<>::init(1.003));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_lf","lnN",SystMap<>::init(0.90));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_btag_lf","lnN",SystMap<>::init(0.999));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_lfstats1_2016","lnN",SystMap<>::init(0.995));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_btag_lfstats1_2016","lnN",SystMap<>::init(0.999));

  cb.cp().process({"STT","STL","TTT","TTL"}).AddSyst(cb,"CMS_btag_lfstats2_2016","lnN",SystMap<>::init(0.995));
  cb.cp().process(JoinStr({{"W","ZT","VVT","ZL","VVL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"},sig_procs})).AddSyst(cb,"CMS_btag_lfstats2_2016","lnN",SystMap<>::init(1.001));

  // XSection Uncertainties
  cb.cp().process({"TTT","TTL"}).AddSyst(cb,"CMS_htt_tjXsec", "lnN", SystMap<>::init(1.042));
  cb.cp().process({"VVT","VVL"}).AddSyst(cb,"CMS_htt_vvXsec", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"STT","STL"}).AddSyst(cb,"CMS_htt_stXsec", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"ZT","ZL"}).AddSyst(cb,"CMS_htt_zjXsec", "lnN", SystMap<>::init(1.02));

  //Muon Fake Rate Uncertainty
  //cb.cp().process({"ZL","TTL","VVL"}).AddSyst(cb, "CMS_mFakeTau_2016 ", "lnN",SystMap<>::init(1.20));    
  
  //Luminosity Uncertainty
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}})).AddSyst(cb, "lumi_13TeV_2016", "lnN", SystMap<>::init(1.022));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}})).AddSyst(cb, "lumi_13TeV_XY", "lnN", SystMap<>::init(1.009));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}})).AddSyst(cb, "lumi_13TeV_BBD", "lnN", SystMap<>::init(1.004));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}})).AddSyst(cb, "lumi_13TeV_DB", "lnN", SystMap<>::init(1.005));
  cb.cp().process(JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}})).AddSyst(cb, "lumi_13TeV_GS", "lnN", SystMap<>::init(1.004));

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
	  RecoilVector = JoinStr({ggH_STXS,qqH_STXS,{"ZT","ZL","ggH_hww125","qqH_hww125","OutsideAcceptance"}});
	  ZPTVector = {"ZT","ZL"};	  
	  TESVector = JoinStr({sig_procs,{"VVT","STT","ZT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  JESVector = JoinStr({sig_procs,{"ZT","VVT","STT","TTT","VVL","STL","ZL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  MuESVector = JoinStr({sig_procs,{"ZT","VVT","STT","TTT","ZL","VVL","STL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  PrefiringVector = JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","ZT","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  TauIDVector = JoinStr({sig_procs,{"VVT","STT","ZT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  TriggerVector = JoinStr({sig_procs,{"VVT","STT","ZT","TTT","VVL","STL","TTL","ZL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	}
      else
	{	  
	  RecoilVector = JoinStr({ggH_STXS,qqH_STXS,{"ZL","ggH_hww125","qqH_hww125","OutsideAcceptance"}});
	  ZPTVector = {"ZL"};	  
	  TESVector = JoinStr({sig_procs,{"VVT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  JESVector = JoinStr({sig_procs,{"VVT","TTT","VVL","ZL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  MuESVector = JoinStr({sig_procs,{"ZL","VVT","TTT","VVL","TTL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  PrefiringVector = JoinStr({sig_procs,{"VVL","VVT","STT","STL","ZL","TTL","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  TauIDVector = JoinStr({sig_procs,{"VVT","STT","TTT","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	  TriggerVector = JoinStr({sig_procs,{"VVT","STT","TTT","VVL","STL","TTL","ZL","ggH_hww125","qqH_hww125","WH_hww125","ZH_hww125","OutsideAcceptance"}});
	}

      if(Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
	{
	  AddShapesIfNotEmpty({"CMS_m_FakeTau_etalt0p4_2016","CMS_m_FakeTau_eta0p4to0p8_2016",
		"CMS_m_FakeTau_eta0p8to1p2_2016","CMS_m_FakeTau_eta1p2to1p7_2016",
		"CMS_m_FakeTau_etagt1p7_2016","CMS_m_FakeTau_taupt30to50_2016",
		"CMS_m_FakeTau_taupt50to70_2016","CMS_m_FakeTau_tauptgt70_2016"},
	    {"ZL"},
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      else
	{
	  AddShapesIfNotEmpty({"CMS_m_FakeTau_etalt0p4_2016","CMS_m_FakeTau_eta0p4to0p8_2016",
		"CMS_m_FakeTau_eta0p8to1p2_2016","CMS_m_FakeTau_eta1p2to1p7_2016",
		"CMS_m_FakeTau_etagt1p7_2016","CMS_m_FakeTau_taupt30to40_2016",
		"CMS_m_FakeTau_taupt40to50_2016","CMS_m_FakeTau_tauptgt50_2016"},
	    {"ZL"},
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      
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
		"CMS_FF_closure_lpt_xtrg_mt_qcd_2016",
		"CMS_FF_closure_lpt_xtrg_mt_w_2016",
		"CMS_FF_closure_lpt_xtrg_mt_tt_2016",
		"CMS_FF_closure_lpt_mt_qcd",
		"CMS_FF_closure_lpt_mt_w",
		"CMS_FF_closure_lpt_mt_tt",
		"CMS_FF_closure_OSSS_mvis_mt_qcd_2016",            
		"CMS_FF_closure_pth_mt_w_2016",
		"CMS_FF_norm_mt_0jet_2016",
		"CMS_FF_norm_mt_1jet_2016",
		"CMS_FF_norm_mt_2jet_2016",
		"CMS_FF_norm_mt_3jet_2016",
		"CMS_FF_norm_mt_4jet_2016",
		},
	    {"jetFakes"},
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
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
      AddShapesIfNotEmpty({"CMS_scale_j_Absolute","CMS_scale_j_Absolute_2016","CMS_scale_j_BBEC1","CMS_scale_j_BBEC1_2016","CMS_scale_j_EC2","CMS_scale_j_EC2_2016",
	    "CMS_scale_j_FlavorQCD","CMS_scale_j_HF","CMS_scale_j_HF_2016","CMS_scale_j_RelativeBal"},
	JESVector,
	&cb,
	1.000,
	TheFile,CategoryArgs);      

      //JER
      std::cout<<"JER"<<std::endl;
      AddShapesIfNotEmpty({"CMS_res_j_2016"},
			  JESVector,
			  &cb,
			  1.000,
			  TheFile,CategoryArgs);

      if (Input.OptionExists("-x0"))
	{
	  std::cout<<"Scaled ggH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_ggH_Mu_norm","THU_ggH_Res_norm","THU_ggH_Mig01_norm","THU_ggH_Mig12_norm","THU_ggH_VBF2j_norm",
		"THU_ggH_VBF3j_norm","THU_ggH_qmtop_norm","THU_ggH_PT60_norm","THU_ggH_PT120_norm"},
	    JoinStr({ggH_STXS,{"ggH_hww125"}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);            

	  //qqH theory uncertainties
	  //NOTE, We explicitly removed THU_qqH_yield from this.
	  std::cout<<"qqH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_qqH_PTH200","THU_qqH_Mjj60","THU_qqH_Mjj120","THU_qqH_Mjj350","THU_qqH_Mjj700",
		"THU_qqH_Mjj1000","THU_qqH_Mjj1500","THU_qqH_PTH25","THU_qqH_JET01"},
	    JoinStr({qqH_STXS,{"qqH_hww125"}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);
	}
      else if (Input.OptionExists("-dp")||Input.OptionExists("-dn")||Input.OptionExists("-dm")||Input.OptionExists("-dljpt"))
	{
	  std::cout<<"ggH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_ggH_Mu","THU_ggH_Res","THU_ggH_Mig01","THU_ggH_Mig12","THU_ggH_VBF2j",
		"THU_ggH_VBF3j","THU_ggH_qmtop","THU_ggH_PT60","THU_ggH_PT120"},
	    JoinStr({ggH_STXS,{"ggH_hww125"}}),
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
	    JoinStr({ggH_STXS,{"ggH_hww125"}}),
	    &cb,
	    1.00,
	    TheFile,CategoryArgs);            

	  //qqH theory uncertainties
	  std::cout<<"qqH Theory"<<std::endl;
	  AddShapesIfNotEmpty({"THU_qqH_yield","THU_qqH_PTH200","THU_qqH_Mjj60","THU_qqH_Mjj120","THU_qqH_Mjj350","THU_qqH_Mjj700",
		"THU_qqH_Mjj1000","THU_qqH_Mjj1500","THU_qqH_PTH25","THU_qqH_JET01"},
	    JoinStr({qqH_STXS,{"qqH_hww125"}}),
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

    }
  //**************************************
  //embedded uncertainties. 
  //**************************************
  if(not Input.OptionExists("-e"))
    {
      //test embedded zero jet normalization uncertainty
      cb.cp().process({"embedded"}).AddSyst(cb,"CMS_EmbeddedZeroJet_2016", "shape", SystMap<>::init(1.00));

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
  //auto rebinning of low background bins
  auto rebin = ch::AutoRebin()
    .SetBinThreshold(0.01);
  rebin.Rebin(cb.cp().channel({"mt"}), cb);
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
