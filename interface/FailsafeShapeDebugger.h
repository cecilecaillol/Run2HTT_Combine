#include "TROOT.h"
#include <string>
#include <vector>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"

class FailsafeShapeDebugger
{
 private:
  ch::CombineHarvester * myCB;
  TFile* myFile;
  ch::CombineHarvester workingCopy;
  
  std::set <std::string> badDirectories;
  std::set <std::pair<std::string,std::string>> badProcs;
  std::set <std::tuple<std::string,std::string,std::string>> badHistograms;
  std::set <std::tuple<std::string,std::string,std::string>> skippableHistograms;
  int fatalErrors;
  int nonFatalErrors;

 public:
  FailsafeShapeDebugger(ch::CombineHarvester* , TFile *);
  FailsafeShapeDebugger& cp();
  FailsafeShapeDebugger& process(std::vector<std::string>);
  FailsafeShapeDebugger& bin(std::vector<std::string>);
  FailsafeShapeDebugger& AddSyst(ch::CombineHarvester&, std::string, std::string, ch::syst::SystMap<>);
  void report();  
  std::vector<std::string> SetToVector(std::set<std::string>);

};
