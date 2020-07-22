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

ggHScales = {
    "THU_ggH_MuUp":1.01763,
    "THU_ggH_MuDown":0.983136,
    "THU_ggH_ResUp":1.01236,
    "THU_ggH_ResDown":0.990195,
    "THU_ggH_Mig01Up":1.00738,
    "THU_ggH_Mig01Down":0.994629,
    "THU_ggH_Mig12Up":1.00608,
    "THU_ggH_Mig12Down":0.996028,
    "THU_ggH_VBF2jUp":1.01935,
    "THU_ggH_VBF2jDown":0.988537,
    "THU_ggH_VBF3jUp":1.01839,
    "THU_ggH_VBF3jDown":0.988824,
    "THU_ggH_qmtopUp":1.00105,
    "THU_ggH_qmtopDown":0.998741,
    "THU_ggH_PT60Up":1.00346,
    "THU_ggH_PT60Down":0.998505,
    "THU_ggH_PT120Up":1.00368,
    "THU_ggH_PT120Down":0.998689,
}

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
                    newShape.Scale(1.0/ggHScales[ggHShape+upOrDown])
                    #okay, should be good to write it
                    newShape.Write()

    datacardFile.Write()
    datacardFile.Close()
