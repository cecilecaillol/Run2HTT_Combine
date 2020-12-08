#!/usr/bin/env python
import ROOT
import argparse
import re
import CombineHarvester.Run2HTT_Combine.PlottingModules.Utilities as utils
import CombineHarvester.Run2HTT_Combine.PlottingModules.prefitPostfitSettings.ratioPlot as RP
import os
from array import array


#accumulated width
#0-1: 50-70
#1-2: 70-90
#2-3: 90-110
#3-4: 110-130
#4-5: 130-150
#5-6: 150-170
#6-8: 170-210
#8-10: 210-250
#10-12: 250-290

#Okay, we need a way to rebin entire histogram sets quickly on the fly.
def rebinHistogram(histograms, nBins, binsWithNewWidths={}):
    accumulatedBinWidth = 0
    theBin=1
    rebinningScheme = []
    while accumulatedBinWidth<=nBins*12:
        rebinningScheme.append(accumulatedBinWidth)
        if theBin in binsWithNewWidths:
            accumulatedBinWidth+=binsWithNewWidths[theBin]
            
        else:
            if accumulatedBinWidth%12 in [0,1,2,3,4,5]:
                accumulatedBinWidth+=1
            else:
                accumulatedBinWidth+=2
        theBin+=1
    
    for histogramKey in histograms.keys():
        if histogramKey != 'data':
            #we have a standard TH1, so, here's what we do
            #we make a copy of the histogram
            tempHisto = histograms[histogramKey].Clone()
            binningArray = array('d', rebinningScheme)
            #set the binning to a new binning
            tempHisto.SetBins(len(rebinningScheme)-1, binningArray)
            #we have the same number of bins,
            #so we loop on the old histogram, to duplicate it into the new one.
            for i in range (1,histograms[histogramKey].GetNbinsX()+1):
                tempHisto.SetBinContent(i, histograms[histogramKey].GetBinContent(i))
                tempHisto.SetBinError(i, histograms[histogramKey].GetBinError(i))
            #we now have a duplicate histogram, set our new histogram to be that
            histograms[histogramKey] = tempHisto
        else: #we have to rebin data. This is a TGraphAsymmErrors, so this is a little interesting
            tempData = ROOT.TGraphAsymmErrors(len(rebinningScheme)-1)                        
            pointContent = histograms[histogramKey].GetY()
            for i in range(0,tempData.GetN()):
                x = (float(rebinningScheme[i+1])+float(rebinningScheme[i]))/2
                y = pointContent[i]
                err_Up = histograms[histogramKey].GetErrorYhigh(i)
                err_Down = histograms[histogramKey].GetErrorYlow(i)
                tempData.SetPoint(i,x,y)
                tempData.SetPointEYhigh(i,err_Up)
                tempData.SetPointEYlow(i,err_Down)
                tempData.SetPointEXlow(i,x-rebinningScheme[i])
                tempData.SetPointEXhigh(i,rebinningScheme[i+1]-x)
            histograms[histogramKey] = tempData
    return rebinningScheme
            
def setMassBinAxisTitles(theHistogram,rebinScheme):
    for i in range(1,len(rebinScheme)):
        #let's figure out the string we need
        #50+20*x
        lowBinStr = str(50+20*(rebinScheme[i-1]%12))
        highBinStr = str(50+20*(rebinScheme[i]%12))
        if rebinScheme[i]%12 == 0:
            highBinStr = '290'
        theHistogram.GetXaxis().SetBinLabel(i,lowBinStr+"-"+highBinStr)

def AddIfExists(directory,originalObject,histogramName):
    if directory.GetListOfKeys().Contains(histogramName):
        originalObject.Add(directory.Get(histogramName))

def CompileSignals(directory,signalName,listOfSignalBins):
    #okay, let's find the first thing in the list of these elements that is not empty.
    #directory.ls()
    #print listOfSignalBins
    for signalBin in listOfSignalBins:
        fullHistoName = signalName+'_'+signalBin
        if directory.GetListOfKeys().Contains(fullHistoName):
            initialSignalHisto = directory.Get(fullHistoName)
            listOfSignalBins.remove(signalBin)
            break
        else:
            listOfSignalBins.remove(signalBin)
    #okay, we should now have an initial signal bin histo.
    #now we just add the other elemenrs to it, and then return it
    for signalBin in listOfSignalBins:
        fullHistoName = signalName+'_'+signalBin
        AddIfExists(directory,initialSignalHisto,fullHistoName)
    return initialSignalHisto

def BlindDataHistogram(dataHistogram):
    for i in range (0,dataHistogram.GetNbinsX()+1):
        if i % 9 ==5 or i % 9 == 6 or i % 9 == 7:
            dataHistogram.SetBinContent(i,-1)
    

#okay, we're trying to make a (Data-bkg)/BackgdError plot
#I thnk the MC errors should essentially just be what we do for the ratio plot.
def MakeSignalStrengthHisto(histograms,ratioErrors):       
    #okay, first things first, let's make a histogram that is just the 
    #background so we can do the subtraction
    backgroundHistogram = histograms['embedded'].Clone()
    backgroundHistogram.Add(histograms['jetFakes'])
    backgroundHistogram.Add(histograms['ttbar'])
    backgroundHistogram.Add(histograms['diboson'])
    if 'singleTop' in histograms.keys():
        backgroundHistogram.Add(histograms['singleTop'])
    backgroundHistogram.Add(histograms['ZL'])

    """signalStrengthHisto = histograms['dataHist'].Clone()
    signalStrengthHisto.Add(backgroundHistogram,-1.0)
    #do the division
    signalStrengthHisto.Divide(ratioErrors)"""

    signalStrengthHisto = histograms['data'].Clone()
    p_x = signalStrengthHisto.GetX()
    p_y = signalStrengthHisto.GetY()
    for i in range(0,signalStrengthHisto.GetN()):
        signalStrengthHisto.SetPoint(i,p_x[i],(p_y[i]-backgroundHistogram.GetBinContent(i+1))/(ratioErrors.GetBinContent(i+1)))
        signalStrengthHisto.SetPointEYhigh(i,signalStrengthHisto.GetErrorYhigh(i)/(ratioErrors.GetBinContent(i+1)+0.0001))
        signalStrengthHisto.SetPointEYlow(i,signalStrengthHisto.GetErrorYlow(i)/(ratioErrors.GetBinContent(i+1)+0.0001))
    
    #okay, now we do the same thing for signal and we can get the heck out of here.
    higgsHisto = histograms['signal'].Clone()
    higgsHisto.Divide(ratioErrors)
    higgsHisto.SetLineColor(ROOT.kRed)
    higgsHisto.SetFillStyle(4000)
    higgsHisto.SetLineWidth(2)

    unitRatioError = ratioErrors.Clone()
    for i in range(1,unitRatioError.GetNbinsX()+1):
        unitRatioError.SetBinContent(i,0)
        unitRatioError.SetBinError(i,1)
        #unitRatioError.Divide(ratioErrors)

    return signalStrengthHisto,higgsHisto,unitRatioError

parser = argparse.ArgumentParser(description="Quick script for duping prefit plots from differential analysis")
parser.add_argument('theFile',nargs='?',help='The file to retrieve plots and dump from')
parser.add_argument('--measurementType',nargs='?',choices=['pth','njets','ljpt'],help='Type of higgs measurements to retrieve',default='pth')
parser.add_argument('--prefitOrPostfit',nargs='?',choices=['prefit','postfit'],help='make prefit or postfit plots',default='prefit')
parser.add_argument('--pause',action='store_true',help='pause after every histogram made')
parser.add_argument('--unblind',action='store_true',help='unblind the datapoints in each histogram')
parser.add_argument('--lowerPad',nargs='?',choices=['ratio','signal'],help='Choose ratio or (obs-bkg)/err plot for the lower pad',default='ratio')

args = parser.parse_args()
theFile = ROOT.TFile(args.theFile)

#theFile.cd('shapes_prefit')

#shapeDirectory = theFile.Get('shapes_prefit')

if args.prefitOrPostfit == 'prefit':
    categoryDirectory = theFile.Get("shapes_prefit")
else:
    categoryDirectory = theFile.Get("shapes_fit_s")

for directory in categoryDirectory.GetListOfKeys():
    theDirectory = categoryDirectory.Get(directory.GetName())
    theDirectory.cd()    

    print(theDirectory.GetName())
    
    #let's just go get anything that isn't a shape uncertainty
    #let's check if we have an em channel directory or not
    
    isEMChannel = False    
    if 'em_' in theDirectory.GetName():
        isEMChannel = True

    #okay, the toys don't carry the real data histogram along, so we need to do it ourselves.
    #first things first, let's go find the file and histogram
    #okay, let's figure out a channel and year
    if 'mt_' in theDirectory.GetName():
        channel = 'mt'
    elif 'et_' in theDirectory.GetName():
        channel = 'et'
    elif 'tt_' in theDirectory.GetName():
        channel = 'tt'
    elif 'em_' in theDirectory.GetName():
        channel = 'em'
    
    if '2016' in theDirectory.GetName():
        year = '2016'
    elif '2017' in theDirectory.GetName():
        year = '2017'
    elif '2018' in theDirectory.GetName():
        year = '2018'

    #let's get all the histograms required.
    print('Getting embedded and jet fakes...')
    histograms = {}
    histograms['embedded'] = theDirectory.Get('embedded')
    if isEMChannel:
        histograms['jetFakes'] = theDirectory.Get('QCD')
        #histograms['jetFakes'].Add(theDirectory.Get('W'))
        #histograms['jetFakes'] = theDirectory.Get('W')
    else:
        histograms['jetFakes'] = theDirectory.Get('jetFakes')
        
    #let's collect up signal samples
    print('Getting signal...')
    #theDirectory.ls()
    if args.measurementType == 'pth':                
        ggHHistogram = CompileSignals(theDirectory,
                                      'ggH',
                                      ['PTH_0_45',
                                       'PTH_45_80',
                                       'PTH_80_120',
                                       'PTH_120_200',
                                       'PTH_200_350',
                                       'PTH_350_450',
                                       'PTH_GT_450'])
        xHHistogram = CompileSignals(theDirectory,
                                      'xH',
                                      ['PTH_0_45',
                                       'PTH_45_80',
                                       'PTH_80_120',
                                       'PTH_120_200',
                                       'PTH_200_350',
                                       'PTH_350_450',
                                       'PTH_GT_450,'])
    elif args.measurementType == 'njets':                
        ggHHistogram = CompileSignals(theDirectory,
                                      'ggH',
                                      ['NJ_0',
                                       'NJ_1',
                                       'NJ_2',
                                       'NJ_3',
                                       'NJ_GE4',])
        xHHistogram = CompileSignals(theDirectory,
                                      'xH',
                                      ['NJ_0',
                                       'NJ_1',
                                       'NJ_2',
                                       'NJ_3',
                                       'NJ_GE4',])
    elif args.measurementType == 'ljpt':
        ggHHistogram = CompileSignals(theDirectory,
                                      'ggH',
                                      ['J1PT_30_60',
                                       'J1PT_60_120',
                                       'J1PT_120_200',
                                       'J1PT_200_350',
                                       'J1PT_GT350',
                                       'NJ_0'])
        xHHistogram = CompileSignals(theDirectory,
                                      'xH',
                                      ['J1PT_30_60',
                                       'J1PT_60_120',
                                       'J1PT_120_200',
                                       'J1PT_200_350',
                                       'J1PT_GT350',
                                       'NJ_0'])
    histograms['ggH'] = ggHHistogram
    histograms['xH'] = xHHistogram
    print('Making final signal...')
    signalHistogram = ggHHistogram.Clone()
    signalHistogram.Add(xHHistogram)
    #signalHistogram.Scale(20)
    
    histograms['signal'] = signalHistogram

    #let's go grab other background stuff
    print('Getting remaining backgrounds...')
    histograms['ZL'] = theDirectory.Get('ZL')
    
    print('VVL..')
    dibosonHistogram = theDirectory.Get('VVL').Clone()
    print('VVT...')
    if not isEMChannel:
        AddIfExists(theDirectory,dibosonHistogram,'VVT')        
    histograms['diboson'] = dibosonHistogram
    
    if not (channel == 'et' and year=='2018'):
        print('STL...')
        singleTopHistogram = theDirectory.Get('STL').Clone()
        print('STT...')
        if not isEMChannel:
            AddIfExists(theDirectory,singleTopHistogram,'STT')        
        histograms['singleTop'] = singleTopHistogram

    print('TTL...')
    ttbarHistogram = theDirectory.Get('TTL').Clone()
    print('TTT...')
    if not isEMChannel:
        AddIfExists(theDirectory,ttbarHistogram,'TTT')
    histograms['ttbar']=ttbarHistogram
    
    print('OutsideAcceptance...')
    outsideAcceptanceHistogram = theDirectory.Get('OutsideAcceptance').Clone()

    print('Making others histogram...')
    othersHistogram= dibosonHistogram.Clone()
    if not (channel=='et' and year == '2018'):
        print('single top...')
        othersHistogram.Add(singleTopHistogram)
    othersHistogram.Add(outsideAcceptanceHistogram)
    #print('signal...')
    #othersHistogram.Add(signalHistogram)
    histograms['other'] = othersHistogram    

    print('Getting the data...')
    # 
    print(channel)
    category=''
    if channel == 'mt' or channel == 'et' or channel == 'tt':
        category = re.search('[a-z,A-Z]*$',theDirectory.GetName()).group(0)
    print(category)

    #we use this for proper Poisson Error Drawing
    dataGraph = theDirectory.Get("data")    
    """
    dataHist = ROOT.TH1F("Data","Data",dataGraph.GetN(),0,dataGraph.GetN())
    x = ROOT.Double()
    y = ROOT.Double()
    for i in range(0,dataGraph.GetN()):
        dataGraph.GetPoint(i, x, y)
        dataHist.Fill(x,y)
    """
        
    histograms['data'] = dataGraph
    #histograms['dataHist'] = dataHist

    histograms['total'] = theDirectory.Get('total')

    #REBINNING HERE
    rebinScheme = []
    if args.measurementType == 'pth':
        if channel == 'et':
            if category == 'LowTauPt':                
                rebinScheme = rebinHistogram(histograms,6)
            elif category == 'IntermediateTauPt':
                    rebinScheme = rebinHistogram(histograms,6,{1:2})
            elif category == 'HighTauPt':
                if year == '2016' or year == '2017':
                    rebinScheme = rebinHistogram(histograms,7,{1:3,9:2})
                else:
                    rebinScheme = rebinHistogram(histograms,7,{1:3})
        elif channel == 'mt':
            if category == 'LowTauPt' or category == 'IntermediateTauPt':
                rebinScheme = rebinHistogram(histograms,6)
            elif category == 'HighTauPt':
                rebinScheme = rebinHistogram(histograms,7,{1:2})
        elif channel == 'tt':
            if category == 'LowTauPt':
                if year == '2016': 
                    rebinScheme = rebinHistogram(histograms,7,{61:2})
                elif year == '2017':
                    rebinScheme = rebinHistogram(histograms,7,{60:2})
                else:
                    rebinScheme = rebinHistogram(histograms,7)
            elif category == 'IntermediateTauPt':
                rebinScheme = rebinHistogram(histograms,7)
            elif category == 'HighTauPt':
                rebinScheme = rebinHistogram(histograms,6)
        else:
            rebinScheme = rebinHistogram(histograms,7)
                
    elif args.measurementType == 'njets':
        if channel == 'et':
            if category == 'LowTauPt':
                if year == '2017' or year == '2018':
                    rebinScheme = rebinHistogram(histograms,5,{1:2})
                else:
                    rebinScheme = rebinHistogram(histograms,5)
            elif category == 'IntermediateTauPt':
                if year == '2016' or year == '2017':
                    rebinScheme = rebinHistogram(histograms,5,{1:3})
                elif year == '2018':
                    rebinScheme = rebinHistogram(histograms,5,{1:2})
            elif category == 'HighTauPt':
                if year == '2016' or year == '2017':
                    rebinScheme = rebinHistogram(histograms,5,{1:4})
                elif year == '2018':
                    rebinScheme = rebinHistogram(histograms,5,{1:3})
        elif channel == 'mt':
            if category == 'LowTauPt':
                rebinScheme = rebinHistogram(histograms,5)
            elif category == 'IntermediateTauPt':
                rebinScheme = rebinHistogram(histograms,5,{1:2})
            elif category == 'HighTauPt':
                rebinScheme = rebinHistogram(histograms,5,{1:3})
        elif channel == 'tt':
            if category == 'LowTauPt' or  category == 'IntermediateTauPt':
                rebinScheme = rebinHistogram(histograms,5)
            elif category == 'HighTauPt':
                if year == '2016' or year == '2017':
                    rebinScheme = rebinHistogram(histograms,5,{5:2})
                elif year == '2018':
                    rebinScheme = rebinHistogram(histograms,5,{1:2,4:3})
        else:
            rebinScheme = rebinHistogram(histograms,5)
    elif args.measurementType == 'ljpt':        
        if channel == 'et':
            if category == 'HighTauPt':
                if(year == '2016' or year == '2017'):
                    rebinScheme = rebinHistogram(histograms,6,{1:4})            
                else:
                    rebinScheme = rebinHistogram(histograms,6,{1:3})
            elif category == 'IntermediateTauPt':
                if (year == '2016' or year == '2017'):
                    rebinScheme = rebinHistogram(histograms,6,{1:3})
                else:
                    rebinScheme = rebinHistogram(histograms,6,{1:2})
            elif category == 'LowTauPt':
                if year == '2017' or year == '2018':
                    rebinScheme = rebinHistogram(histograms,6,{1:2})
                else:
                    rebinScheme = rebinHistogram(histograms,6)
        elif channel == 'mt':
            if category == 'HighTauPt':
                rebinScheme = rebinHistogram(histograms,6,{1:3})
            elif category == 'IntermediateTauPt':
                rebinScheme = rebinHistogram(histograms,6,{1:2})
            else:
                rebinScheme = rebinHistogram(histograms,6)
        elif channel == 'tt':
            if category == 'HighTauPt':
                if year == '2016':
                    rebinScheme = rebinHistogram(histograms,6,{1:2,46:2})
                elif year == '2017':
                    rebinScheme = rebinHistogram(histograms,6,{46:2})
                elif year == '2018':
                    rebinScheme = rebinHistogram(histograms,6,{46:2,49:3})
            else:
                rebinScheme = rebinHistogram(histograms,6)
        else:
            rebinScheme = rebinHistogram(histograms,6)
    rebinArray = array('f', rebinScheme)

    #let's assemble the stuff and do the things.
    #lets assemble some colors
    histograms['jetFakes'].SetFillColor(ROOT.TColor.GetColor('#ffccff'))
    histograms['other'].SetFillColor(ROOT.TColor.GetColor('#12cadd'))
    histograms['signal'].SetFillColor(ROOT.kRed)
    histograms['ttbar'].SetFillColor(ROOT.TColor.GetColor('#9999cc'))
    histograms['ZL'].SetFillColor(ROOT.TColor.GetColor('#4496c8'))
    histograms['embedded'].SetFillColor(ROOT.TColor.GetColor('#ffcc66'))

    histograms['data'].SetMarkerStyle(20)
    #histograms['signal'].SetLineColor(ROOT.kRed)
    #histograms['signal'].SetLineWidth(2)    

    #let's assemble the stack
    print('making the stack')
    theBackgroundStack = ROOT.THStack('backgroundStack_'+channel+'_'+category,'backgroundStack_'+channel+'_'+category)
    print('other...')
    theBackgroundStack.Add(histograms['other'],'HIST')
    print('tt...')
    theBackgroundStack.Add(histograms['ttbar'],'HIST')
    print('ZL...')
    theBackgroundStack.Add(histograms['ZL'],'HIST')
    print('fakes...')
    theBackgroundStack.Add(histograms['jetFakes'],'HIST')    
    print('embedded...')
    theBackgroundStack.Add(histograms['embedded'],'HIST')
    print('signal')
    theBackgroundStack.Add(histograms['signal'],'HIST')
    
    print('making stack errors...')
    #theStackErrors = utils.MakeStackErrors(theBackgroundStack)
    theStackErrors = histograms['total']
    theStackErrors.SetLineColor(0)
    theStackErrors.SetLineWidth(0)
    theStackErrors.SetMarkerStyle(0)
    theStackErrors.SetFillStyle(3001)
    theStackErrors.SetFillColor(15)

    print('making ratio...')
    ratioPlot,ratioPlotErrors = RP.MakeRatioPlot(theBackgroundStack,theStackErrors,histograms['data'])

    print('making signal strength...')
    dataSignalStrength,predictedSignalStrength,unitRatioError = MakeSignalStrengthHisto(histograms,ratioPlotErrors)

    #okay, let's set up the canvas and pads and stuff
    print('Drawing...')
    theCanvas = ROOT.TCanvas("prefitCanvas"+'_'+channel+'_'+category,"prefitCanvas"+'_'+channel+'_'+category,0,0,2000,600)
    theCanvas.Divide(1,2)#theCanvas.Divide(1,3)
    plotPad = theCanvas.GetPrimitive(theCanvas.GetName()+"_1")
    #signalStrengthPad = theCanvas.GetPrimitive(theCanvas.GetName()+"_2")
    #ratioPad = theCanvas.GetPrimitive(theCanvas.GetName()+"_3")
    ratioPad = theCanvas.GetPrimitive(theCanvas.GetName()+"_2")
    plotPad.SetPad(0.0,0.3,1.,1.)#plotPad.SetPad(0.0,0.6,1.,1.)
    #signalStrengthPad.SetPad(0.0,0.3,1.0,0.6)
    ratioPad.SetPad(0.0,0.0,1.0,0.3)

    plotPad.SetLogy()
    plotPad.SetTickx()
    plotPad.SetTicky()

    """
    signalStrengthPad.SetTickx()
    signalStrengthPad.SetTicky()
    signalStrengthPad.SetTopMargin(0.04)
    #signalStrengthPad.SetBottomMargin(0.5)
    #signalStrengthPad.SetLeftMargin(1)
    """

    ratioPad.SetTickx()
    ratioPad.SetTicky()
    ratioPad.SetTopMargin(0.0)
    ratioPad.SetBottomMargin(0.35)
    plotPad.SetBottomMargin(0.02)

    #let's come up with a full title
    fullTitle=year+' '
    if channel == 'tt':
        fullTitle+='#tau#tau '
    elif channel == 'mt':
        fullTitle += '#mu#tau '
    elif channel == 'et':
        fullTitle += 'e#tau '
    else:
        fullTitle += 'e#mu '
    
    if args.measurementType == 'pth':
        fullTitle+='p_{t}^{H} '
    elif args.measurementType == 'njets':
        fullTitle+='N_{jets} '
    else:
        fullTitle+='Leading Jet p_{t} '
        
    if channel != 'em':
        if category == 'LowTauPt':
            fullTitle+='Low p_{t}^{#tau} Category'
        elif category == 'IntermediateTauPt':
            fullTitle+='Intermediate p_{t}^{#tau} Category'
        elif category == 'HighTauPt':
            fullTitle+='High p_{t}^{#tau} Category'

    print('main pad...')
    plotPad.cd()
    #plotPad.SetGridx()    
    #we need to have a resonable maximum and minimum
    theBackgroundStack.SetMaximum(max(theBackgroundStack.GetMaximum(),histograms['data'].GetMaximum())*100)
    theBackgroundStack.SetMinimum(max(theBackgroundStack.GetMinimum()*0.9,0.1))
    theBackgroundStack.Draw()    
    theBackgroundStack.GetXaxis().SetLabelSize(0)
    #theBackgroundStack.GetXaxis().SetNdivisions(-900-(histograms['embedded'].GetNbinsX()/9))
    theBackgroundStack.SetTitle(fullTitle)
    theBackgroundStack.GetYaxis().SetTitle('Events')
    theStackErrors.Draw("SAME e2")    
    #histograms['signal'].Draw('SAME HIST')
    histograms['data'].Draw('E0P')

    cmsLatex = ROOT.TLatex()
    cmsLatex.SetTextSize(0.06)
    cmsLatex.SetNDC(True)
    cmsLatex.SetTextFont(61)
    cmsLatex.SetTextAlign(11)
    cmsLatex.DrawLatex(0.1,0.92,"CMS")
    cmsLatex.SetTextFont(52)
    cmsLatex.DrawLatex(0.1+0.065,0.92,"Preliminary")

    cmsLatex.SetTextAlign(31)
    cmsLatex.SetTextFont(42)
    if year == '2016':
        luminosity = '35.9'
    if year == '2017':
        luminosity = '41.5'
    if year == '2018':
        luminosity = '59.7'
    cmsLatex.DrawLatex(0.9,0.92,luminosity+" fb^{-1} (13 TeV)")

    #okay, let's make some labels
    latex = ROOT.TLatex()
    latex.SetTextSize(0.06)
    latex.SetTextAlign(21)
    latex.SetTextFont(52)
    verticalLocation = max(theBackgroundStack.GetMaximum(),histograms['data'].GetMaximum())*5
    if args.measurementType == 'pth':
        if channel == 'tt' and category == 'HighTauPt':
            latex.DrawLatex(5,verticalLocation,'0<p_{t}^{H}<80')
            latex.DrawLatex(5+12,verticalLocation,'80<p_{t}^{H}<120')
            latex.DrawLatex(5+24,verticalLocation,'120<p_{t}^{H}<200')
            latex.DrawLatex(5+36,verticalLocation,'200<p_{t}^{H}<350')
            latex.DrawLatex(5+48,verticalLocation,'350<p_{t}^{H}<450')
            latex.DrawLatex(5+60,verticalLocation,'450<p_{t}^{H}')
        elif (channel == 'mt' or channel == 'et') and (category == 'LowTauPt' or category=='IntermediateTauPt'):
            latex.DrawLatex(5,verticalLocation,'0<p_{t}^{H}<45')
            latex.DrawLatex(5+12,verticalLocation,'45<p_{t}^{H}<80')
            latex.DrawLatex(5+24,verticalLocation,'80<p_{t}^{H}<120')
            latex.DrawLatex(5+36,verticalLocation,'120<p_{t}^{H}<200')
            latex.DrawLatex(5+48,verticalLocation,'200<p_{t}^{H}<350')
            latex.DrawLatex(5+60,verticalLocation,'350<p_{t}^{H}')
        else:
            latex.DrawLatex(5,verticalLocation,'0<p_{t}^{H}<45')
            latex.DrawLatex(5+12,verticalLocation,'45<p_{t}^{H}<80')
            latex.DrawLatex(5+24,verticalLocation,'80<p_{t}^{H}<120')
            latex.DrawLatex(5+36,verticalLocation,'120<p_{t}^{H}<200')
            latex.DrawLatex(5+48,verticalLocation,'200<p_{t}^{H}<350')
            latex.DrawLatex(5+60,verticalLocation,'350<p_{t}^{H}<450')
            latex.DrawLatex(5+72,verticalLocation,'450<p_{t}^{H}')
    if args.measurementType == 'njets':        
        latex.DrawLatex(5,verticalLocation,'N_{Jets}=0')
        latex.DrawLatex(5+12,verticalLocation,'N_{Jets}=1')
        latex.DrawLatex(5+24,verticalLocation,'N_{Jets}=2')
        latex.DrawLatex(5+36,verticalLocation,'N_{Jets}=3')
        latex.DrawLatex(5+48,verticalLocation,'N_{Jets}=4')
    if args.measurementType == 'ljpt':
        if channel == 'tt':
            latex.DrawLatex(5,verticalLocation,'30<p_{t}^{Jet}<60')
            latex.DrawLatex(5+12,verticalLocation,'60<p_{t}^{Jet}<120')
            latex.DrawLatex(5+24,verticalLocation,'120<p_{t}^{Jet}<200')
            latex.DrawLatex(5+36,verticalLocation,'200<p_{t}^{Jet}<350')
            latex.DrawLatex(5+48,verticalLocation,'350<p_{t}^{Jet}')            
            latex.DrawLatex(5+60,verticalLocation,'0 Jet')            
        else:
            latex.DrawLatex(5,verticalLocation,'0 Jet')
            latex.DrawLatex(5+12,verticalLocation,'30<p_{t}^{Jet}<60')
            latex.DrawLatex(5+24,verticalLocation,'60<p_{t}^{Jet}<120')
            latex.DrawLatex(5+36,verticalLocation,'120<p_{t}^{Jet}<200')
            latex.DrawLatex(5+48,verticalLocation,'200<p_{t}^{Jet}<350')
            latex.DrawLatex(5+60,verticalLocation,'350<p_{t}^{Jet}')

    #okay, let's force a grid on top
    plotGridPad = ROOT.TPad('slices','slices',0,0,1,1)    
    plotGridPad.Draw()
    plotGridPad.cd()
    plotGridPad.SetGridx()
    plotGridPad.SetFrameFillColor(0)
    plotGridPad.SetFrameFillStyle(0)
    plotGridPad.SetFrameBorderMode(0)
    plotGridPad.SetFillStyle(4000)
    plotGridPad.SetBottomMargin(plotPad.GetBottomMargin())

    #plotGridHisto = histograms['dataHist'].Clone()
    nSlices = 0
    if args.measurementType == 'ljpt':
        nSlices = 6
    if args.measurementType == 'njets':
        nSlices = 5
    if args.measurementType == 'pth':
        if channel == 'tt' and category == 'HighTauPt':
            nSlices = 6
        elif (channel == 'mt' or channel == 'et') and (category == 'LowTauPt' or category=='IntermediateTauPt'):
            nSlices = 6
        else:
            nSlices = 7
    plotGridHisto = ROOT.TH1F('grid','grid',nSlices*12,0,nSlices*12)
    plotGridHisto.Reset()    
    plotGridHisto.SetTitle('')
    plotGridHisto.GetXaxis().SetLabelSize(0.0)
    plotGridHisto.GetYaxis().SetLabelSize(0.0)
    plotGridHisto.GetXaxis().SetTickLength(0.0)
    plotGridHisto.GetYaxis().SetTickSize(0.0)
    plotGridHisto.GetYaxis().SetTitleSize(0.0)
    plotGridHisto.Draw()    
    #plotGridHisto.GetXaxis().SetNdivisions(-900-(histograms['dataHist'].GetNbinsX()/9))
    plotGridHisto.GetXaxis().SetNdivisions(-1200-nSlices)
    plotGridHisto.SetFillStyle(4000)

    #we also want a legend on this pad, so let's make that
    theLegend = ROOT.TLegend(0.12,0.78,0.88,0.88)
    theLegend.AddEntry(histograms['data'],'Obs.','P')
    theLegend.AddEntry(histograms['embedded'],'#tau#tau bkg.','F')
    if channel != 'em':
        theLegend.AddEntry(histograms['jetFakes'],'Jet#rightarrow#tau_{h} mis-ID','F')
    else:
        theLegend.AddEntry(histograms['jetFakes'],'Jet#rightarrow e/#mu mis-ID','F')
    theLegend.AddEntry(histograms['ZL'],'Z #rightarrow ee/#mu#mu','F')
    theLegend.AddEntry(histograms['ttbar'],'t#bar{t} + Jets','F')
    theLegend.AddEntry(histograms['other'],'Others','F')
    theLegend.AddEntry(theStackErrors,'Bkg. uncertainty','F')
    theLegend.AddEntry(histograms['signal'],'Higgs #rightarrow #tau#tau','F')    

    theLegend.SetNColumns(4)
    theLegend.SetBorderSize(0)
    theLegend.SetFillStyle(0)
    theLegend.SetTextSize(0.06)
    theLegend.Draw()
    
    
    
    print('ratio pad...')
    ratioPad.cd()
    ROOT.gStyle.SetOptStat(0)    
    if args.lowerPad == 'ratio':
        ratioPad.SetGridy()
        ratioPlotErrors.Draw('e2')
        ratioPlot.Draw('E0P')
        ratioPlotErrors.SetTitle('')    
        ratioPlotErrors.GetXaxis().SetLabelSize(0.1)
        setMassBinAxisTitles(ratioPlotErrors,rebinScheme)
        ratioPlotErrors.GetXaxis().SetTitle('m_{#tau#tau}')
        ratioPlotErrors.GetXaxis().SetTitleSize(0.1)
        ratioPlotErrors.GetXaxis().SetTitleOffset(1.2)
    elif args.lowerPad == 'signal':
        unitRatioError.Draw('e2')
        unitRatioError.GetYaxis().SetTitle('#frac{Obs.-bkg.}{Bkg. unc.}')
        unitRatioError.GetYaxis().SetRangeUser(-2,
                                               max(dataSignalStrength.GetMaximum(),
                                                   predictedSignalStrength.GetMaximum())*2)
        unitRatioError.GetYaxis().SetTitleOffset(0.5)
        predictedSignalStrength.Draw('HIST SAME')
        dataSignalStrength.Draw('E0P')
        unitRatioError.SetTitle('')
        unitRatioError.GetXaxis().SetLabelSize(0.1)
        setMassBinAxisTitles(unitRatioError,rebinScheme)
        unitRatioError.GetXaxis().SetTitle('m_{#tau#tau}')
        unitRatioError.GetXaxis().SetTitleSize(0.1)
        unitRatioError.GetXaxis().SetTitleOffset(1.8)
        
        signalPlotLegend = ROOT.TLegend(0.12,0.78,0.88,0.88)
        signalPlotLegend.AddEntry(dataSignalStrength,'(Obs.-bkg.)/Bkg. unc.','LP')
        signalPlotLegend.AddEntry(predictedSignalStrength, 'H#rightarrow#tau#tau/Bkg. unc.','L')
        signalPlotLegend.AddEntry(unitRatioError, 'Bkg. unc.','F')
        signalPlotLegend.SetNColumns(3)
        signalPlotLegend.SetBorderSize(0)
        signalPlotLegend.SetFillStyle(0)
        signalPlotLegend.Draw()
        
    
    ratioGridPad = ROOT.TPad('slices','slices',0,0,1,1)    
    ratioGridPad.Draw()
    ratioGridPad.cd()
    ratioGridPad.SetGridx()
    ratioGridPad.SetFrameFillColor(0)
    ratioGridPad.SetFrameFillStyle(0)
    ratioGridPad.SetFrameBorderMode(0)
    ratioGridPad.SetFillStyle(4000)
    ratioGridPad.SetTopMargin(ratioPad.GetTopMargin())
    ratioGridPad.SetBottomMargin(ratioPad.GetBottomMargin())
    
    ratioGridHisto = plotGridHisto.Clone()
    ratioGridHisto.Draw()
    ratioGridHisto.GetXaxis().SetNdivisions(-1200-nSlices)
    ratioGridHisto.SetFillStyle(4000)        

    if args.pause:
        raw_input('Press Enter To Continue...')
    #okay, let's figure out the final name 
    finalName=''
    if channel == 'em':
        finalName += channel+'_'
    else:
        finalName += channel+'_'+category+'_'
    if args.measurementType == 'pth':
        finalName +='HiggsPt'
    elif args.measurementType == 'njets':
        if channel == 'et' or channel == 'em':
            finalName += 'njets'
        elif channel == 'mt' or channel == 'tt':
            finalName += 'NJets'
    elif args.measurementType == 'ljpt':
        if channel == 'et' or channel == 'em':
            finalName += 'j1pt'
        elif channel == 'mt':
            finalName += 'LeadingJetPt'
        elif channel == 'tt':
            finalName += 'LJPT'
    finalName += year
    theCanvas.SaveAs(finalName+'.png')
    theCanvas.SaveAs(finalName+'.pdf')
    del theCanvas #if you don't do this, future changes to the variable cause a virtual method exception
    
