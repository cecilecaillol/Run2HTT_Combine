#!/usr/bin/env python
import ROOT
import argparse
import re

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create a copy of the specified data card with duplicated shape histograms for correllating/decorrelating uncertainties")
    parser.add_argument('--year',nargs="?",choices={"2016","2017","2018"},help="data card year.")
    parser.add_argument('--DataCard',help="Specify the data card")
    parser.add_argument('--OutputFileName',nargs="?",help="Name of the result data card.")
    parser.add_argument('--TrimYears',action="store_true",help="Instead of adding years to histogram names, trim them off instead")

    args = parser.parse_args()        

    DataCardFile = ROOT.TFile(args.DataCard)

    if args.OutputFileName:
        NewDataCardName = args.OutputFileName
    else:
        NewDataCardName = args.DataCard.rsplit(".root")[0]+"_DC.root"

    NewDataCardFile = ROOT.TFile(NewDataCardName,"RECREATE")

    for Directory in DataCardFile.GetListOfKeys():
        TheDirectory = DataCardFile.Get(Directory.GetName())
        NewDirectory = NewDataCardFile.mkdir(Directory.GetName())
        NewDirectory.cd()
        for Histogram in TheDirectory.GetListOfKeys():
            TheDirectory.Get(Histogram.GetName()).Write()
	    ModifiedTrimYears=args.TrimYears
	    if re.search("looser_lep_wp_201",Histogram.GetName()) or re.search("closure_jet1pt_tt_qcd_201",Histogram.GetName()) or re.search("closure_tau2pt_tt_qcd_201",Histogram.GetName()):
		ModifiedTrimYears= (not args.TrimYears)
            #if a shape, add it and a copy to the new file
            if re.search("(Up|Down)",Histogram.GetName()):                
                #JES shapes now are decorrelated from the get-go.
                #don't handle deal with those.
                if re.search('CMS_Jet',Histogram.GetName()):
                    continue
                #if we're trimming years, but this histogram doesn't even have a year
                #then we don't really need to do anything.
                if ModifiedTrimYears and not re.search(args.year+"(Up|Down)$",Histogram.GetName()):
                    continue
                CopyHisto = TheDirectory.Get(Histogram.GetName()).Clone()
                #we need to add a way to add in the year before the "up/down"
                if re.search("Up",CopyHisto.GetName()):
                    if ModifiedTrimYears:
                        NewNameTitle = CopyHisto.GetName()[:len(CopyHisto.GetName())-7]+"Up"
                    else:
                        NewNameTitle = CopyHisto.GetName()[:len(CopyHisto.GetName())-2]+"_"+args.year+"Up"
		    # For the embedded sample, duplicate the tau ES for partial correlation
		    if "_emb" in CopyHisto.GetName():
			if ModifiedTrimYears:
			   CopyHisto2 = TheDirectory.Get(Histogram.GetName()).Clone()
			   NewNameTitle2 = CopyHisto.GetName().replace('_emb','')[:len(CopyHisto.GetName())-11]+"Up"
                	   CopyHisto2.SetNameTitle(NewNameTitle2,NewNameTitle2)
			   CopyHisto2.Write()
                           CopyHisto3 = TheDirectory.Get(Histogram.GetName()).Clone()
                           NewNameTitle3 = CopyHisto.GetName().replace('_emb','')
                           CopyHisto3.SetNameTitle(NewNameTitle3,NewNameTitle3)
                           CopyHisto3.Write()                           
			else:
                           #print("Prepping embedded up decorrelations: "+Histogram.GetName())
                           CopyHisto2 = TheDirectory.Get(Histogram.GetName()).Clone()                           
                           #NewNameTitle2 = CopyHisto.GetName().replace('_emb','')[:len(CopyHisto.GetName())-2]+"_"+args.year+"Up"
                           NewNameTitle2 = CopyHisto.GetName().replace('_emb','')
                           NewNameTitle2 = NewNameTitle2[:len(NewNameTitle2)-2]
                           NewNameTitle2+="_"+args.year+"Up"
                           #print(NewNameTitle2)
                           CopyHisto2.SetNameTitle(NewNameTitle2,NewNameTitle2)
                           CopyHisto2.Write()
                           CopyHisto3 = TheDirectory.Get(Histogram.GetName()).Clone()
                           NewNameTitle3 = CopyHisto.GetName().replace('_emb','')
                           CopyHisto3.SetNameTitle(NewNameTitle3,NewNameTitle3)
                           CopyHisto3.Write()
                           #print(NewNameTitle3)                           

                elif re.search("Down",CopyHisto.GetName()):
                    if ModifiedTrimYears:
                        NewNameTitle = CopyHisto.GetName()[:len(CopyHisto.GetName())-9]+"Down"
                    else:
                        NewNameTitle = CopyHisto.GetName()[:len(CopyHisto.GetName())-4]+"_"+args.year+"Down"
                    if "_emb" in CopyHisto.GetName():
                        if ModifiedTrimYears:
                           CopyHisto2 = TheDirectory.Get(Histogram.GetName()).Clone()
                           NewNameTitle2 = CopyHisto.GetName().replace("_emb","")[:len(CopyHisto.GetName())-13]+"Down" 
                           CopyHisto2.SetNameTitle(NewNameTitle2,NewNameTitle2)
                           CopyHisto2.Write()
                           CopyHisto3 = TheDirectory.Get(Histogram.GetName()).Clone()
                           NewNameTitle3 = CopyHisto.GetName().replace("_emb","")
                           CopyHisto3.SetNameTitle(NewNameTitle3,NewNameTitle3)
                           CopyHisto3.Write()                           
                        else:
                           #print("Prepping embedded down decorrelations: "+Histogram.GetName())
                           CopyHisto2 = TheDirectory.Get(Histogram.GetName()).Clone()
                           #NewNameTitle2 = CopyHisto.GetName().replace("_emb","")[:len(CopyHisto.GetName())-4]+"_"+args.year+"Down"
                           NewNameTitle2 = CopyHisto.GetName().replace("_emb","")
                           NewNameTitle2 = NewNameTitle2[:len(NewNameTitle2)-4 ]
                           NewNameTitle2+="_"+args.year+"Down"
                           #print(NewNameTitle2)
                           CopyHisto2.SetNameTitle(NewNameTitle2,NewNameTitle2)
                           CopyHisto2.Write()
                           CopyHisto3 = TheDirectory.Get(Histogram.GetName()).Clone()
                           NewNameTitle3 = CopyHisto.GetName().replace("_emb","")
                           #print(NewNameTitle3)
                           CopyHisto3.SetNameTitle(NewNameTitle3,NewNameTitle3)
                           CopyHisto3.Write()
                           CopyHisto4 = TheDirectory.Get(Histogram.GetName()).Clone()                           

                else:
                    raise RuntimeError("Something fell through the RE")
                CopyHisto.SetNameTitle(NewNameTitle,NewNameTitle)
                CopyHisto.Write()
    NewDataCardFile.Write()
    NewDataCardFile.Close()
    DataCardFile.Close()
    
                
