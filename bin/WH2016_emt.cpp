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

  // Create an emtpty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  // Here we will just define two categories for an 8TeV analysis. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.
  ch::Categories cats = {
      {1, "emt_high"}
    };
  vector<string> masses = {"125"};//ch::MassesFromRange("120");//120-135:5");
  //! [part3]
  cb.AddObservations({"*"}, {"wh"}, {"2016"}, {"emt"}, cats);

  vector<string> bkg_procs = {"TriBoson","allFakes","WZ","ZZ","WH_lep_hww125","ZH_lep_hww125"};//VV
  cb.AddProcesses({"*"}, {"wh"}, {"2016"}, {"emt"}, bkg_procs, cats, false);

  vector<string> sig_procs = {"WH_lep_htt","ZH_lep_htt","ggZH_lep_htt"};
  //vector<string> sig_procs = {"WH_lep_PTV_0_75_htt","WH_lep_PTV_75_150_htt","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","ZH_lep_htt","ggZH_lep_htt"};
  cb.AddProcesses(masses, {"wh"}, {"2016"}, {"emt"}, sig_procs, cats, true);
  //! [part4]


  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;

  cb.cp().process({"DY"}).AddSyst(cb, "CMH_htt_zXsec_13TeV", "lnN", SystMap<>::init(1.02));
  cb.cp().process({"TT"}).AddSyst(cb, "CMS_htt_tjXsec_13TeV", "lnN", SystMap<>::init(1.042));
  cb.cp().process({"allFakes"}).AddSyst(cb, "reducible_norm_emt", "lnN", SystMap<>::init(1.15));
  cb.cp().process({"WZ"}).AddSyst(cb, "CMS_htt_zzXsec_13TeV", "lnN", SystMap<>::init(1.032));
  cb.cp().process({"ZZ"}).AddSyst(cb, "CMS_htt_wzXsec_13TeV", "lnN", SystMap<>::init(1.032));
  cb.cp().process({"ttZ"}).AddSyst(cb, "CMS_htt_ttzXsec_13TeV", "lnN", SystMap<>::init(1.25));
  cb.cp().process({"TriBoson"}).AddSyst(cb, "CMS_htt_triboson_13TeV", "lnN", SystMap<>::init(1.25));

  cb.cp().process({"WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt"}).AddSyst(cb, "BR_htt_THU", "lnN", SystMap<>::init(1.017));
  cb.cp().process({"WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt"}).AddSyst(cb, "BR_htt_PU_mq", "lnN", SystMap<>::init(1.0099));
  cb.cp().process({"WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt"}).AddSyst(cb, "BR_htt_PU_alphas", "lnN", SystMap<>::init(1.0062));
  cb.cp().process({"WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","WH_lep_htt"}).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.007));
  cb.cp().process({"ZH_lep_htt","ggZH_lep_htt","ZH_lep_hww125"}).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.038));
  cb.cp().process({"WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","WH_lep_htt"}).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.019));
  cb.cp().process({"ZH_lep_htt","ggZH_lep_htt","ZH_lep_hww125"}).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.016));

  cb.cp().process({"WH_lep_htt"}).AddSyst(cb,"WHlep_scale", "shape", SystMap<>::init(1.00));
  cb.cp().process({"ZH_lep_htt"}).AddSyst(cb,"ZHlep_scale", "shape", SystMap<>::init(1.00));
  cb.cp().process({"WH_lep_PTV_0_75_htt","WH_lep_PTV_75_150_htt","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt"}).AddSyst(cb,"WH_scale_lowpt", "shape", SystMap<>::init(1.00));
  cb.cp().process({"WH_lep_PTV_GT250_htt"}).AddSyst(cb,"WH_scale_highpt", "shape", SystMap<>::init(1.00));

  cb.cp().process({"TT","DY"}).AddSyst(cb, "CMH_htt_jetFakeLep_2016", "lnN", SystMap<>::init(1.20));

  cb.cp().process({"TriBoson","TT","DY","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb, "lumi_Run2016", "lnN", SystMap<>::init(1.022));
  cb.cp().process({"TriBoson","TT","DY","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb, "lumi_XYfactorization", "lnN", SystMap<>::init(1.009));
  cb.cp().process({"TriBoson","TT","DY","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb, "lumi_beamBeamDeflection", "lnN", SystMap<>::init(1.004));
  cb.cp().process({"TriBoson","TT","DY","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb, "lumi_dynamicBeta", "lnN", SystMap<>::init(1.005));
  cb.cp().process({"TriBoson","TT","DY","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb, "lumi_ghostsAndSatellites", "lnN", SystMap<>::init(1.004));

  cb.cp().process({"TriBoson","TT","DY","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb, "CMS_singleeletrg_2016", "lnN", SystMap<>::init(1.01));
  cb.cp().process({"TriBoson","TT","DY","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb, "CMS_singlemutrg_2016", "lnN", SystMap<>::init(1.01));
  cb.cp().process({"TriBoson","TT","DY","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb, "CMS_eff_e_2016", "lnN", SystMap<>::init(1.02));
  cb.cp().process({"TriBoson","TT","DY","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb, "CMS_eff_m_2016", "lnN", SystMap<>::init(1.02));

  // Against ele and against mu for real taus
  cb.cp().process({"TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_eff_t_againstemu_emt_2016","lnN",SystMap<>::init(1.03));

  // b-tagging efficiency
  cb.cp().process({"TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_btag_eta","lnN",SystMap<>::init(1.001));
  cb.cp().process({"TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_btag_hf","lnN",SystMap<>::init(1.002));
  cb.cp().process({"TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_btag_hfstats1_2018","lnN",SystMap<>::init(1.0000));
  cb.cp().process({"TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_hfstats2_2018","lnN",SystMap<>::init(1.000));
  cb.cp().process({"TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_btag_jes","lnN",SystMap<>::init(1.003));
  cb.cp().process({"TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_btag_lf","lnN",SystMap<>::init(0.999));
  cb.cp().process({"TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_btag_lfstats1_2018","lnN",SystMap<>::init(0.999));
  cb.cp().process({"TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_btag_lfstats2_2018","lnN",SystMap<>::init(1.001));

  // TES
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_scale_t_1prong_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_scale_t_1prong1pizero_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_scale_t_3prong_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_scale_t_3prong1pizero_2016", "shape", SystMap<>::init(1.00));

  // Tau ID
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_tauideff_pt20to25_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_tauideff_pt25to30_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_tauideff_pt30to35_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_tauideff_pt35to40_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_tauideff_ptgt40_2016", "shape", SystMap<>::init(1.00));

  //Scale met
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_scale_met_unclustered_2016", "shape", SystMap<>::init(1.00));

  //Scale mu, ele
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_scale_e_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_scale_m_etalt1p2_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_scale_m_eta1p2to2p1_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_scale_m_etagt2p1_2016", "shape", SystMap<>::init(1.00));

  //Prefiring
  cb.cp().process({"allFakes","TriBoson","WZ","ZZ","WH_had_htt","WH_lep_PTH_0_75","WH_lep_PTV_75_150","WH_lep_PTV_150_250_0J_htt","WH_lep_PTV_150_250_GE1J_htt","WH_lep_PTV_GT250_htt","WH_lep_hww125","ZH_lep_hww125","WH_lep_hww125","ZH_lep_hww125","WH_lep_htt","ZH_lep_htt","ggZH_lep_htt","ttZ"}).AddSyst(cb,"CMS_prefiring", "shape", SystMap<>::init(1.00));


  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeEle_pt10to15_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeEle_pt15to20_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeEle_pt20to30_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeEle_pt30to40_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeEle_ptgt40_2016", "shape", SystMap<>::init(1.00));

  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeMu_pt10to15_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeMu_pt15to20_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeMu_pt20to30_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeMu_pt30to40_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeMu_ptgt40_2016", "shape", SystMap<>::init(1.00));

  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm0_pt20to25_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm0_pt25to30_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm0_pt30to35_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm0_pt35to40_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm0_pt40to50_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm0_pt50to60_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm0_ptgt60_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm1_pt20to25_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm1_pt25to30_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm1_pt30to35_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm1_pt35to40_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm1_pt40to50_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm1_pt50to60_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm1_ptgt60_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm10_pt20to25_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm10_pt25to30_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm10_pt30to35_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm10_pt35to40_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm10_pt40to50_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm10_pt50to60_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeTauVT_dm10_ptgt60_2016", "shape", SystMap<>::init(1.00));

  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeEle_promptSubtraction_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"allFakes"}).AddSyst(cb,"CMS_fakeMu_promptSubtraction_2016", "shape", SystMap<>::init(1.00));

  cb.cp().backgrounds().ExtractShapes(
      aux_shapes + "whemt2016.root",
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
  cb.cp().signals().ExtractShapes(
      aux_shapes + "whemt2016.root",
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
  //! [part7]

  //! [part8]

  /*auto bbb = ch::BinByBinFactory()
  .SetAddThreshold(0.05)
  .SetMergeThreshold(0.5)
  .SetFixNorm(false);
  bbb.MergeBinErrors(cb.cp().backgrounds());
  bbb.AddBinByBin(cb.cp().backgrounds(), cb);*/

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
  // Observation, Process and Systemtatic entries in the CombineHarvester
  // instance.

  // We create the output root file that will contain all the shapes.
  TFile output((Input.ReturnToken(0)+"/"+"wh2016_emt.input.root").c_str(), "RECREATE");

  // Finally we iterate through each bin,mass combination and write a
  // datacard.
  for (auto b : bins) {
    for (auto m : masses) {
      cout << ">> Writing datacard for bin: " << b << " and mass: " << m
           << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must remtemtber to include the "*" mass entry to get
      // all the data and backgrounds.
      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(Input.ReturnToken(0)+"/"+b + "_" + m + ".txt", 
						    output);
    }
  }
  //! [part9]

}
