#!/usr/bin/env python
import ROOT
import argparse

def main():
    parser = argparse.ArgumentParser(description='Create a copy of the pseicfied data card with specific bins blinded')
    parser.add_argument('dataCard',nargs='?',help='')
    parser.add_argument('--lowerBin',nargs='?',help='lowest bin to blind',type=int,required=True)
    parser.add_argument('--upperBin',nargs='?',help='highest bin to blind',type=int,required=True)
    parser.add_argument('--outputCardName',nargs='?',help='name of the result data card',default='blindedCard.root')

    args = parser.parse_args()

    theDataCardFile = ROOT.TFile(args.dataCard)

    newDataCardFile = ROOT.TFile(args.outputCardName,"RECREATE")

    for directoryKey in theDataCardFile.GetListOfKeys():
        theDirectory = theDataCardFile.Get(directoryKey.GetName())
        newDirectory = newDataCardFile.mkdir(directoryKey.GetName())
        newDirectory.cd()
        for histogramKey in theDirectory.GetListOfKeys():
            #okay, let's get the histogram, clone it, blind it, and then put it into the new file
            theHistogram = theDirectory.Get(histogramKey.GetName()).Clone()
            for binNumber in range(args.lowerBin, args.upperBin+1):
                theHistogram.SetBinContent(binNumber,0)
            theHistogram.Write()
    newDataCardFile.Write()
    newDataCardFile.Close()
    theDataCardFile.Close()

if __name__ == "__main__":
    main()
