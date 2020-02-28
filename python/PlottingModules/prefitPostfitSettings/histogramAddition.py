import ROOT
import rebinning
import math
from array import array

def AddYearsTogether(collection,channel,category,prefitOrPostfit):
    for dictType in ['Full','Slimmed','Signals','Data']:        
        collection[channel]['Run2'][category][prefitOrPostfit][dictType] = {}
        for histogram in collection[channel]['2016'][category][prefitOrPostfit][dictType]:            
            #try:
            Run2Histo = collection[channel]['2016'][category][prefitOrPostfit][dictType][histogram].Clone()            
            if ((channel == 'tt' and (category == 'tt_vbf_highHpT' or category == 'tt_vbf_lowHpT')) 
                or (channel == 'et' and category == "et_vbflow" )):
                Run2Histo.Add(collection[channel]['2017-folded'][category][prefitOrPostfit][dictType][histogram])
                Run2Histo.Add(collection[channel]['2018-folded'][category][prefitOrPostfit][dictType][histogram])
            else:
                Run2Histo.Add(collection[channel]['2017'][category][prefitOrPostfit][dictType][histogram])
                Run2Histo.Add(collection[channel]['2018'][category][prefitOrPostfit][dictType][histogram])
            collection[channel]['Run2'][category][prefitOrPostfit][dictType][histogram] = Run2Histo
            #except:
            #    print("Problem creating full run2 histo for "+channel+" "+category+" "+prefitOrPostfit+" "+dictType+" "+histogram)

def PerformAllAdditions(collection):
    #we need a quick way to refold tau tau histograms back together. If they exist
    try:
        collection['tt']['2017-folded']= {}
    except KeyError:
        pass
    else:
        for category in collection['tt']['2017']:
            collection['tt']['2017-folded'][category]= {}
            for prefitOrPostfit in collection['tt']['2017'][category]:
                collection['tt']['2017-folded'][category][prefitOrPostfit]= {}
                for dictType in collection['tt']['2017'][category][prefitOrPostfit]:
                    collection['tt']['2017-folded'][category][prefitOrPostfit][dictType] = CreateRefoldedHistograms(collection['tt']['2017'][category][prefitOrPostfit][dictType])

    try:
        collection['tt']['2018-folded']= {}
    except KeyError:
        pass
    else:
        for category in collection['tt']['2018']:        
            collection['tt']['2018-folded'][category]= {}
            for prefitOrPostfit in collection['tt']['2018'][category]:
                collection['tt']['2018-folded'][category][prefitOrPostfit]= {}
                for dictType in collection['tt']['2018'][category][prefitOrPostfit]:
                    collection['tt']['2018-folded'][category][prefitOrPostfit][dictType] = CreateRefoldedHistograms(collection['tt']['2018'][category][prefitOrPostfit][dictType])

    try:
        collection['et']['2017-folded']= {}
    except KeyError:
        pass
    else:
        for category in collection['et']['2017']:
            collection['et']['2017-folded'][category]= {}
            for prefitOrPostfit in collection['et']['2017'][category]:
                collection['et']['2017-folded'][category][prefitOrPostfit]= {}
                for dictType in collection['et']['2017'][category][prefitOrPostfit]:
                    collection['et']['2017-folded'][category][prefitOrPostfit][dictType] = CreateRefoldedHistograms(collection['et']['2017'][category][prefitOrPostfit][dictType])
    try:
        collection['et']['2018-folded']= {}
    except KeyError:
        pass
    else:
        for category in collection['et']['2018']:
            collection['et']['2018-folded'][category]= {}
            for prefitOrPostfit in collection['et']['2018'][category]:
                collection['et']['2018-folded'][category][prefitOrPostfit]= {}
                for dictType in collection['et']['2018'][category][prefitOrPostfit]:
                    collection['et']['2018-folded'][category][prefitOrPostfit][dictType] = CreateRefoldedHistograms(collection['et']['2018'][category][prefitOrPostfit][dictType])

    #we'll also need a full run 2 dictionary. Let's make that.
    for channel in collection:
        collection[channel]['Run2'] = {}
        for category in collection[channel]['2016']:
            collection[channel]['Run2'][category] = {}
            for prefitOrPostfit in ['prefit','postfit']:
                collection[channel]['Run2'][category][prefitOrPostfit] = {}            
                AddYearsTogether(collection,channel,category,prefitOrPostfit)

#Fold the last slice of a collection of histograms back into itself.
def CreateRefoldedHistograms(histograms):
    #okay how do we do this?
    #for each histogram in the collection
    newDictionary = {}
    for histogramName in histograms:
        #default case, only one slice exists so we can just set our new histogram equal to the old and continue

        # we create a copy of it with one less slice
        newNBins = histograms[histogramName].GetNbinsX()-9
        newHistogramArray = array('f',rebinning.CreateStandardSliceBinBoundaryArray(newNBins/9))
        newHistogram = ROOT.TH1F(histograms[histogramName].GetName(),
                                 histograms[histogramName].GetTitle(),
                                 newNBins,
                                 newHistogramArray)
        #then we loop, filling all but the (new) last slice with the old content and errors
        for i in range(1,newNBins-9+1):
            newHistogram.SetBinContent(i,histograms[histogramName].GetBinContent(i))
            newHistogram.SetBinError(i,histograms[histogramName].GetBinError(i))
        #for the new last slice we loop over each bin, filling it with the addition of contents,
        # and addition in quadrature of the errors
        for i in range(newNBins-9,newNBins+1):
            newBinContent = histograms[histogramName].GetBinContent(i)+histograms[histogramName].GetBinContent(i+9)
            newBinError = math.sqrt(histograms[histogramName].GetBinError(i)*histograms[histogramName].GetBinError(i)+histograms[histogramName].GetBinError(i+9)*histograms[histogramName].GetBinError(i+9))            
            newHistogram.SetBinContent(i,newBinContent)
            newHistogram.SetBinError(i,newBinError)
        #now, we put it in a new dictionary with the same structure
        newDictionary[histogramName] = newHistogram
        #once we have done this for all the histograms, we put them back in a similar looking dictionary, and hand it back
    return newDictionary
