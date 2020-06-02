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

using namespace std;

int main() {
  //! [part1]
  // First define the location of the "auxiliaries" directory where we can
  // source the input files containing the datacard shapes
  cout<<"test"<<endl;
  string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/auxiliaries/shapes/";

  // Create an mmmtpty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  // Here we will just define two categories for an 8TeV analysis. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.
  ch::Categories cats = {
      {1, "mmmt"}
    };
  vector<string> masses = {"125"};//ch::MassesFromRange("120");//120-135:5");
  //! [part3]
  cb.AddObservations({"*"}, {"zh"}, {"2016"}, {"mmmt"}, cats);

  vector<string> bkg_procs = {"Triboson","Reducible","ZZ"};//VV
  cb.AddProcesses({"*"}, {"zh"}, {"2016"}, {"mmmt"}, bkg_procs, cats, false);

  vector<string> sig_procs = {"ZH_lep_htt"};
  //vector<string> sig_procs = {"ZH_had_htt","ZH_lep_PTV_0_75_htt","ZH_lep_PTV_75_150_htt","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"};
  cb.AddProcesses(masses, {"zh"}, {"2016"}, {"mmmt"}, sig_procs, cats, true);
  //! [part4]


  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;

  cb.cp().process({"Reducible"}).AddSyst(cb, "reducible_norm_mmmt", "lnN", SystMap<>::init(1.20));
  cb.cp().process({"ZZ"}).AddSyst(cb, "CMS_htt_wzXsec_13TeV", "lnN", SystMap<>::init(1.032));
  cb.cp().process({"Triboson"}).AddSyst(cb, "CMS_htt_triboson_13TeV", "lnN", SystMap<>::init(1.25));

  // Theory uncertainties
  cb.cp().process({"ZH_had_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt","ZH_lep_htt"}).AddSyst(cb, "BR_htt_THU", "lnN", SystMap<>::init(1.017));
  cb.cp().process({"ZH_had_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt","ZH_lep_htt"}).AddSyst(cb, "BR_htt_PU_mq", "lnN", SystMap<>::init(1.0099));
  cb.cp().process({"ZH_had_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt","ZH_htt","ZH_htt"}).AddSyst(cb, "BR_htt_PU_alphas", "lnN", SystMap<>::init(1.0062));
  cb.cp().process({"ZH_had_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt","ZH_htt"}).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.007));
  cb.cp().process({"ZH_lep_htt"}).AddSyst(cb, "QCDScale_VH", "lnN", SystMap<>::init(1.038));
  cb.cp().process({"ZH_had_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt","ZH_htt"}).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.019));
  cb.cp().process({"ZH_lep_htt"}).AddSyst(cb, "pdf_Higgs_VH", "lnN", SystMap<>::init(1.016));

  // Lumi uncertainties
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb, "lumi_Run2016", "lnN", SystMap<>::init(1.022));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb, "lumi_XYfactorization", "lnN", SystMap<>::init(1.009));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb, "lumi_beamBeamDeflection", "lnN", SystMap<>::init(1.004));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb, "lumi_dynamicBeta", "lnN", SystMap<>::init(1.005));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb, "lumi_ghostsAndSatellites", "lnN", SystMap<>::init(1.004));

  // Trg and ID uncertainties
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb, "CMS_singlemutrg_2016", "lnN", SystMap<>::init(1.02));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb, "CMS_eff_m_2016", "lnN", SystMap<>::init(1.06));

/*
  // TES
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_scale_t_1prong_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_scale_t_1prong1pizero_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_scale_t_3prong_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_scale_t_3prong1pizero_2016", "shape", SystMap<>::init(1.00));

  // Tau ID
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_tauideff_pt20to25_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_tauideff_pt25to30_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_tauideff_pt30to35_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_tauideff_pt35to40_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_tauideff_ptgt40_2016", "shape", SystMap<>::init(1.00));

  //Scale met
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_scale_met_unclustered_2016", "shape", SystMap<>::init(1.00));

  //Scale mu, ele
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_scale_m_etalt1p2_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_scale_m_eta1p2to2p1_2016", "shape", SystMap<>::init(1.00));
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_scale_m_etagt2p1_2016", "shape", SystMap<>::init(1.00));

  //Prefiring
  cb.cp().process({"Triboson","ZZ","ZH_lep_htt","ZH_lep_PTH_0_75","ZH_lep_PTV_75_150","ZH_lep_PTV_150_250_0J_htt","ZH_lep_PTV_150_250_GE1J_htt","ZH_lep_PTV_GT250_htt"}).AddSyst(cb,"CMS_prefiring", "shape", SystMap<>::init(1.00));
*/

  cb.cp().backgrounds().ExtractShapes(
      aux_shapes + "zh2016.root",
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
  cb.cp().signals().ExtractShapes(
      aux_shapes + "zh2016.root",
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
  // This function modifies every entry to have a standardised bin name of
  // the form: {analysis}_{channel}_{bin_id}_{era}
  // which is commonly used in the htt analyses
  ch::SetStandardBinNames(cb);
  //! [part8]

  //! [part9]
  // First we generate a set of bin names:
  set<string> bins = cb.bin_set();
  // This method will produce a set of unique bin names by considering all
  // Observation, Process and Systmmmtatic entries in the CombineHarvester
  // instance.

  // We create the output root file that will contain all the shapes.
  TFile output("zh2016_mmmt.input.root", "RECREATE");

  // Finally we iterate through each bin,mass combination and write a
  // datacard.
  for (auto b : bins) {
    for (auto m : masses) {
      cout << ">> Writing datacard for bin: " << b << " and mass: " << m
           << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must rmmmtmmmtber to include the "*" mass entry to get
      // all the data and backgrounds.
      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(
          b + "_" + m + ".txt", output);
    }
  }
  //! [part9]

}
