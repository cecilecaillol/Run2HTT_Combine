#!/usr/bin/env python
import ROOT
import argparse

parser =argparse.ArgumentParser(description = 'Update a file to have copies of theory uncertainties, these theory uncertainties normalized to the nominal histogram normalization')

parser.add_argument('datacards',nargs="+",help="Specify the datacard")

args = parser.parse_args()

#let's keep a list of our theory shapes
ggHTheoryShapes = ["THU_ggH_Mu",
                   "THU_ggH_Res",
                   "THU_ggH_Mig01",
                   "THU_ggH_Mig12",
                   "THU_ggH_VBF2j",
                   "THU_ggH_VBF3j",
                   "THU_ggH_qmtop",
                   "THU_ggH_PT60",
                   "THU_ggH_PT120"]
qqHTheoryShapes = [
    "THU_qqH_yield",
    "THU_qqH_PTH200",
    "THU_qqH_Mjj60",
    "THU_qqH_Mjj120",
    "THU_qqH_Mjj350",
    "THU_qqH_Mjj700",
    "THU_qqH_Mjj1000",
    "THU_qqH_Mjj1500",
    "THU_qqH_PTH25",
    "THU_qqH_JET01"
]

for datacard in args.datacards:
    datacardFile = ROOT.TFile(datacard,"UPDATE")
    for directory in datacardFile.GetListOfKeys():
        theDirectory = datacardFile.Get(directory.GetName())
        theDirectory.cd()    
        #let's start grabbing ggH shapes cloning, and shaping them
        for nominal in ['ggH_htt125','ggH_hww125']:
            for ggHShape in ggHTheoryShapes:
                for upOrDown in ['Up','Down']:
                    newShape = theDirectory.Get(nominal+"_"+ggHShape+upOrDown).Clone()
                    newShape.SetNameTitle(nominal+"_"+ggHShape+"_norm"+upOrDown,
                                          nominal+"_"+ggHShape+"_norm"+upOrDown,)
                    try:
                        scale = newShape.Integral()/theDirectory.Get(nominal).Integral()
                    except ZeroDivisionError:
                        print("Nominal: "+nominal+" in directory: "+directory.GetName()+" of file: "+datacard+" has norm 0. Setting theory norms to 0 as well.")
                        scale = 0
                    newShape.Scale(scale)
                    #okay, should be good to write it
                    newShape.Write()
        for nominal in ['qqH_htt125','qqH_hww125']:
            for qqHShape in qqHTheoryShapes:
                for upOrDown in ['Up','Down']:
                    newShape = theDirectory.Get(nominal+"_"+qqHShape+upOrDown).Clone()
                    newShape.SetNameTitle(nominal+"_"+qqHShape+"_norm"+upOrDown,
                                          nominal+"_"+qqHShape+"_norm"+upOrDown,)
                    try:
                        scale = newShape.Integral()/theDirectory.Get(nominal).Integral()
                    except ZeroDivisionError:
                        print("Nominal: "+nominal+" in directory: "+directory.GetName()+" of file: "+datacard+" has norm 0. Setting theory norms to 0 as well.")
                        scale = 0
                    newShape.Scale(scale)
                    newShape.Write()

    datacardFile.Write()
    datacardFile.Close()
