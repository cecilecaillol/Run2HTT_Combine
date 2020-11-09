#include "TROOT.h"
#include <string>
#include "CombineHarvester/Run2HTT_Combine/interface/FailsafeShapeDebugger.h"
#include <iostream>

std::vector<std::string> FailsafeShapeDebugger::SetToVector(std::set<std::string> theSet)
{
  std::vector<std::string> theVector;
  for (auto iter = theSet.begin(); iter!=theSet.end(); ++iter)
    {
      theVector.push_back(*iter);
    }
  return theVector;
}

//this gets us a pointer to the combine harvester instance in use
//ensuring that our additions will be done to the same instance as any other whatever.
FailsafeShapeDebugger::FailsafeShapeDebugger(ch::CombineHarvester* theCB,TFile * theFile)
{
  this->myCB=theCB;
  this->myFile=theFile;
  this->fatalErrors = 0;
  this->nonFatalErrors = 0;
}

//this sets the working copy to be a cp of the original combine harvester
//from here we can manipulate the working copy how we want
FailsafeShapeDebugger& FailsafeShapeDebugger::cp()
{
  this->workingCopy = myCB->cp();
  return *this;
}

//process copy method
//checks are not performed here yet, but could be?
FailsafeShapeDebugger& FailsafeShapeDebugger::process(std::vector<std::string> procs)
{
  this->workingCopy = this->workingCopy.process(procs);
  return *this;
}

//bin copy method
//checks are not performed here yet, but could be?
FailsafeShapeDebugger& FailsafeShapeDebugger::bin(std::vector<std::string> bins)
{
  this->workingCopy = this->workingCopy.bin(bins);
  return *this;
}

//technically, we don't really to reprovide the cb at this point, since we store an unspoiled copy
//but this keeps the notation consistent
//this is also where all the magic and the checks happen.
FailsafeShapeDebugger& FailsafeShapeDebugger::AddSyst(ch::CombineHarvester& cb, std::string syst, std::string type, ch::syst::SystMap<> theMap)
{
  //values to store and keep our the final things we will try to add   
  std::set<std::string> workingCopyBins = this->workingCopy.bin_set();
  std::set<std::string> workingCopyProcs = this->workingCopy.process_set();

  //First things first. let's check the working copy's bins and see if any of them don't exist  
  for(auto directoryIter = workingCopyBins.begin(); directoryIter != workingCopyBins.end(); ++directoryIter)
    {
      TDirectory* theDirectory = (TDirectory*) this->myFile->Get((*directoryIter).c_str());
      if (this->badDirectories.find(*directoryIter) != this->badDirectories.end()) continue; //if we know it's bad, don't worry about it
      else if (theDirectory == NULL) //the directory we're looking at doesn't exist, let's add that'
	{
	  auto result = this->badDirectories.insert((*directoryIter).c_str());
	  if (result.second)this->fatalErrors++;
	}      
      else
	{
	  for (auto procIter = workingCopyProcs.begin(); procIter != workingCopyProcs.end(); ++procIter)
	    {
	      TH1F* nominalHisto = (TH1F*) theDirectory->Get((*procIter).c_str());
	      if (this->badProcs.find(std::pair<std::string,std::string>({*directoryIter, *procIter})) != this->badProcs.end()) continue;
	      else if (nominalHisto == NULL)
		{
		  auto result = this->badProcs.insert({(*directoryIter).c_str(),(*procIter).c_str()});
		  if(result.second)this->fatalErrors++;
		  continue;
		}
	      else
		{		  
		  TH1F* upHisto = (TH1F*) theDirectory->Get((*procIter+"_"+syst+"Up").c_str());
		  TH1F* downHisto = (TH1F*) theDirectory->Get((*procIter+"_"+syst+"Down").c_str());		  
		  if(upHisto == NULL)
		    {
		      auto result = this->badHistograms.insert({(*directoryIter).c_str(),(*procIter).c_str(),syst});
		      if(result.second)this->fatalErrors++;
		    }	  
		  else if (downHisto == NULL)
		    {
		      auto result = this->badHistograms.insert({(*directoryIter).c_str(),(*procIter).c_str(),syst});
		      if(result.second) this->fatalErrors++;
		    }
		  else if (nominalHisto->Integral() == 0.0 && upHisto->Integral() == 0.0 && downHisto->Integral() == 0.0)
		    {
		      auto result = this->skippableHistograms.insert({(*directoryIter).c_str(),(*procIter).c_str(),syst});
		      if(result.second) this->nonFatalErrors++;		      
		    }		  
		  else //this uncertainty is valid, let's add it'
		    {		      
		      this->workingCopy.cp().bin({*directoryIter}).process({*procIter}).AddSyst(cb,syst,type,theMap);
		    }
		}
	    }
	}
    }          

  return *this;
}

void FailsafeShapeDebugger::report()
{
  std::cout<<"##################### Failsafe Shape Debugger Report #####################"<<std::endl;
  std::cout<<"I encountered: "<<this->fatalErrors<<" \033[1;31mFatal Errors\033[0m"<<std::endl;
  for (auto iter=this->badDirectories.begin(); iter != this->badDirectories.end(); ++iter)
    {
      std::cout<<"I could not find the directory: "<<*iter<<std::endl;
    }
  for(auto iter=this->badProcs.begin(); iter != this->badProcs.end(); ++iter)
    {
      std::cout<<"I could not find the nominal processes for: "<<(*iter).first<<"->"<<(*iter).second<<std::endl;
    }
  for (auto iter=this->badHistograms.begin(); iter != this->badHistograms.end(); ++iter)
    {
      std::cout<<"I could not find the variation histograms for: "<<std::get<0>(*iter)<<"->"<<std::get<1>(*iter)<<"->"<<std::get<2>(*iter)<<std::endl;
    }
  std::cout<<std::endl;
  std::cout<<"I encountered: "<<this->nonFatalErrors<<" \033[1;34mNon-Fatal Errors\033[0m"<<std::endl;
  for (auto iter=this->skippableHistograms.begin(); iter != this->skippableHistograms.end(); ++iter)
    {
      std::cout<<"I found empty skippable histograms here: "<<std::get<0>(*iter)<<"->"<<std::get<1>(*iter)<<"->"<<std::get<2>(*iter)<<std::endl;
    }
  std::cout<<"##################### Failsafe Shape Debugger Report #####################"<<std::endl;
}
