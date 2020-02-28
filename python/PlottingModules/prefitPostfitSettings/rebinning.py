import ROOT
import CombineHarvester.Run2HTT_Combine.CategoryConfigurations as catConfig
from array import array

standardSliceSetup = [0,1,2,3,4,5,6,8,10,12]

def GetNSlices(category,year):
    if (category == catConfig.tt_boosted_1J_category
        or category == catConfig.tt_boosted_GE2J_category               
        ):
            nSlices = 6
    elif (category == catConfig.tt_vbf_low_category):
        if year == "2016" or year == "Run2":
            nSlices = 5
        else:
            nSlices = 6
    elif (category == catConfig.tt_vbf_high_category):
        if year == "2016" or year == "Run2":
            nSlices = 3
        else:
            nSlices = 4
    elif (category == catConfig.et_boosted_1J_category
          or category == catConfig.et_boosted_GE2J_category):        
        nSlices = 5
    elif (category == catConfig.et_vbf_low_category):
        if year == "2016" or year == "Run2":
            nSlices = 4
        else:
            nSlices = 5
    elif (category == catConfig.mt_vbf_low_category
          or category == catConfig.em_vbf_low_category                    
          or category == catConfig.mt_boosted_1J_category
          or category == catConfig.mt_boosted_GE2J_category
          or category == catConfig.em_boosted_1J_category
          or category == catConfig.em_boosted_GE2J_category
          ):        
        nSlices = 5
    #elif (
    #      ):
    #    nSlices = 4
    elif (category == catConfig.mt_vbf_high_category
          or category == catConfig.et_vbf_high_category
          or category == catConfig.em_vbf_high_category          
          or category == catConfig.mt_0jet_high_category
          or category == catConfig.mt_0jet_low_category
          or category == catConfig.et_0jet_high_category
          or category == catConfig.et_0jet_low_category
          or category == catConfig.em_0jet_high_category
          or category == catConfig.em_0jet_low_category
          ):
        nSlices = 3    
    #elif ():
    #    nSlices = 2
    elif (category == catConfig.tt_0jet_category):
        nSlices = 0
    return nSlices

def CreateStandardSliceBinBoundaryArray(nSlices):
    binBoundaryArray = standardSliceSetup[:]    
    for i in range(nSlices):
        binBoundariesToAdd = standardSliceSetup[:]
        for j in range(len(standardSliceSetup)):
            binBoundariesToAdd[j] += standardSliceSetup[len(standardSliceSetup)-1]*(i+1)
        binBoundariesToAdd.pop(0)
        binBoundaryArray  += binBoundariesToAdd                
    return binBoundaryArray
    

def RebinDictionary(dictionary,channel,category,year):
    binBoundaries = []
    nSlices = GetNSlices(category,year)
    binBoundaries = array('f',CreateStandardSliceBinBoundaryArray(nSlices))
    #print(binBoundaries)    
    
    #print binBoundaries
    print channel
    print category    
    for histogram in dictionary:
        #print histogram
        #print(binBoundaries)
        #print(dictionary[histogram].GetNbinsX())
        newHistogram  = ROOT.TH1F(dictionary[histogram].GetName(),
                                  dictionary[histogram].GetName(),
                                  dictionary[histogram].GetNbinsX(),
                                  binBoundaries)
        #print("New One Made")
        for i in range(1,dictionary[histogram].GetNbinsX()+1):
            #print(i)
            newHistogram.SetBinContent(i,dictionary[histogram].GetBinContent(i))
            newHistogram.SetBinError(i,dictionary[histogram].GetBinError(i))            
        #print("replacing")
        dictionary[histogram] = newHistogram

    
            
def RebinCollection(collection):
    for channel in collection:
        for year in collection[channel]:
            for category in collection[channel][year]:
                for prefitOrPostfit in ['prefit','postfit']:
                    for dictType in collection[channel][year][category][prefitOrPostfit]:
                        RebinDictionary(collection[channel][year][category][prefitOrPostfit][dictType],channel,category,year)
