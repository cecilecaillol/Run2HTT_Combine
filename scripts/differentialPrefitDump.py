#!/usr/env/bin python
import ROOT
import argparse
import re
import CombineHarvester.Run2HTT_Combine.PlottingModules.Utilities as utils
import CombineHarvester.Run2HTT_Combine.PlottingModules.prefitPostfitSettings.ratioPlot as RP
import os

massBins = {
    1:'50-70',
    2:'70-90',
    3:'90-110',
    4:'110-130',
    5:'130-150',
    6:'150-170',
    7:'170-210',
    8:'210-250',
    0:'250-290',
    }

def setMassBinAxisTitles(theHistogram):
    for i in range(1,theHistogram.GetNbinsX()+1):
        theHistogram.GetXaxis().SetBinLabel(i,massBins[(i%9)])

def AddIfExists(directory,originalObject,histogramName):
    if directory.GetListOfKeys().Contains(histogramName):
        originalObject.Add(directory.Get(histogramName))

def CompileSignals(directory,signalName,listOfSignalBins):
    #okay, let's find the first thing in the list of these elements that is not empty.
    for signalBin in listOfSignalBins:
        fullHistoName = signalName+'_'+signalBin+'_htt125'
        if directory.GetListOfKeys().Contains(fullHistoName):
            initialSignalHisto = directory.Get(fullHistoName)
            listOfSignalBins.remove(signalBin)
            break
        else:
            listOfSignalBins.remove(signalBin)
    #okay, we should now have an initial signal bin histo.
    #now we just add the other elemenrs to it, and then return it
    for signalBin in listOfSignalBins:
        fullHistoName = signalName+'_'+signalBin+'_htt125'
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
    backgroundHistogram.Add(histograms['singleTop'])
    backgroundHistogram.Add(histograms['ZL'])

    signalStrengthHisto = histograms['data'].Clone()
    signalStrengthHisto.Add(backgroundHistogram,-1.0)
    #do the division
    signalStrengthHisto.Divide(ratioErrors)
    
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

for directory in theFile.GetListOfKeys():
    theDirectory = theFile.Get(directory.GetName())
    theDirectory.cd()

    if args.prefitOrPostfit == 'postfit':
        if 'prefit' in theDirectory.GetName():
            continue
    else:
        if 'postfit' in theDirectory.GetName():
            continue

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
        if channel == 'tt':
            ggHHistogram = CompileSignals(theDirectory,
                                          'ggH',
                                          ['PTH_0_45',
                                           'PTH_45_80',
                                           'PTH_80_120',
                                           'PTH_120_200',
                                           'PTH_200_350',
                                           'PTH_350_450',
                                           'PTH_GE_450,'])
            qqHHistogram = CompileSignals(theDirectory,
                                          'qqH',
                                          ['PTH_0_45',
                                           'PTH_45_80',
                                           'PTH_80_120',
                                           'PTH_120_200',
                                           'PTH_200_350',
                                           'PTH_350_450',
                                           'PTH_GE_450,'])
            WHHistogram = CompileSignals(theDirectory,
                                         'WH',
                                         ['had_PTH_0_45',
                                          'had_PTH_45_80',
                                          'had_PTH_80_120',
                                          'had_PTH_120_200',
                                          'had_PTH_200_350',
                                          'had_PTH_350_450',
                                          'had_PTH_GE450',
                                          'lep_PTH_0_45',
                                          'lep_PTH_45_80',
                                          'lep_PTH_80_120',
                                          'lep_PTH_120_200',
                                          'lep_PTH_200_350',
                                          'lep_PTH_350_450',
                                          'lep_PTH_GE450',])
            ZHHistogram = CompileSignals(theDirectory,
                                         'ZH',
                                         ['had_PTH_0_45',
                                          'had_PTH_45_80',
                                          'had_PTH_80_120',
                                          'had_PTH_120_200',
                                          'had_PTH_200_350',
                                          'had_PTH_350_450',
                                          'had_PTH_GE450',
                                          'lep_PTH_0_45',
                                          'lep_PTH_45_80',
                                          'lep_PTH_80_120',
                                          'lep_PTH_120_200',
                                          'lep_PTH_200_350',
                                          'lep_PTH_350_450',
                                          'lep_PTH_GE450',])
        else:
            ggHHistogram = CompileSignals(theDirectory,
                                          'ggH',
                                          ['PTH_0_45',
                                           'PTH_45_80',
                                           'PTH_80_120',
                                           'PTH_120_200',
                                           'PTH_200_350',
                                           'PTH_350_450',
                                           'PTH_GT_450'])
            qqHHistogram = CompileSignals(theDirectory,
                                          'qqH',
                                          ['PTH_0_45',
                                           'PTH_45_80',
                                           'PTH_80_120',
                                           'PTH_120_200',
                                           'PTH_200_350',
                                           'PTH_350_450',
                                           'PTH_GT_450,'])
            WHHistogram = CompileSignals(theDirectory,
                                         'WH',
                                         ['PTH_0_45',
                                          'PTH_45_80',
                                          'PTH_80_120',
                                          'PTH_120_200',
                                          'PTH_200_350',
                                          'PTH_350_450',
                                          'PTH_GT450'])
            ZHHistogram = CompileSignals(theDirectory,
                                         'ZH',
                                         ['PTH_0_45',
                                          'PTH_45_80',
                                          'PTH_80_120',
                                          'PTH_120_200',
                                          'PTH_200_350',
                                          'PTH_350_450',
                                          'PTH_GT450'])
                                        

    elif args.measurementType == 'njets':
        if channel == 'tt':
            ggHHistogram = CompileSignals(theDirectory,
                                          'ggH',
                                          ['NJETS_0',
                                           'NJETS_1',
                                           'NJETS_2',
                                           'NJETS_3',
                                           'NJETS_GE4',])
            qqHHistogram = CompileSignals(theDirectory,
                                          'qqH',
                                          ['NJETS_0',
                                           'NJETS_1',
                                           'NJETS_2',
                                           'NJETS_3',
                                           'NJETS_GE4',])
            WHHistogram = CompileSignals(theDirectory,
                                         'WH',
                                         ['had_NJETS_0',
                                          'had_NJETS_1',
                                          'had_NJETS_2',
                                          'had_NJETS_3',
                                          'had_NJETS_GE4',
                                          'lep_NJETS_0',
                                          'lep_NJETS_1',
                                          'lep_NJETS_2',
                                          'lep_NJETS_3',
                                          'lep_NJETS_GE4',])
            ZHHistogram = CompileSignals(theDirectory,
                                         'ZH',
                                         ['had_NJETS_0',
                                          'had_NJETS_1',
                                          'had_NJETS_2',
                                          'had_NJETS_3',
                                          'had_NJETS_GE4',
                                          'lep_NJETS_0',
                                          'lep_NJETS_1',
                                          'lep_NJETS_2',
                                          'lep_NJETS_3',
                                          'lep_NJETS_GE4',])
        else:
            ggHHistogram = CompileSignals(theDirectory,
                                          'ggH',
                                          ['NJETS_0',
                                           'NJETS_1',
                                           'NJETS_2',
                                           'NJETS_3',
                                           'NJETS_GE4',])
            qqHHistogram = CompileSignals(theDirectory,
                                          'qqH',
                                          ['NJETS_0',
                                           'NJETS_1',
                                           'NJETS_2',
                                           'NJETS_3',
                                           'NJETS_GE4',])
            WHHistogram = CompileSignals(theDirectory,
                                         'WH',
                                         ['NJETS_0',
                                           'NJETS_1',
                                           'NJETS_2',
                                           'NJETS_3',
                                           'NJETS_GE4',])
            ZHHistogram = CompileSignals(theDirectory,
                                         'ZH',
                                         ['NJETS_0',
                                          'NJETS_1',
                                          'NJETS_2',
                                          'NJETS_3',
                                          'NJETS_GE4',])
    elif args.measurementType == 'ljpt':
        if channel == 'tt':
            ggHHistogram = CompileSignals(theDirectory,
                                          'ggH',
                                          ['LJPT_30_60',
                                           'LJPT_60_120',
                                           'LJPT_120_200',
                                           'LJPT_200_350',
                                           'LJPT_GE350',
                                           'NJETS_0'])
            qqHHistogram = CompileSignals(theDirectory,
                                          'qqH',
                                          ['LJPT_30_60',
                                           'LJPT_60_120',
                                           'LJPT_120_200',
                                           'LJPT_200_350',
                                           'LJPT_GE350',
                                           'NJETS_0'])
            WHHistogram = CompileSignals(theDirectory,
                                         'WH',
                                         ['had_LJPT_30_60',
                                          'had_LJPT_60_120',
                                          'had_LJPT_120_200',
                                          'had_LJPT_200_350',
                                          'had_LJPT_GE350',
                                          'had_NJETS_0',
                                          'lep_LJPT_30_60',
                                          'lep_LJPT_60_120',
                                          'lep_LJPT_120_200',
                                          'lep_LJPT_200_350',
                                          'lep_LJPT_GE350',
                                          'lep_NJETS_0'])
            ZHHistogram = CompileSignals(theDirectory,
                                         'ZH',
                                         ['had_LJPT_30_60',
                                          'had_LJPT_60_120',
                                          'had_LJPT_120_200',
                                          'had_LJPT_200_350',
                                          'had_LJPT_GE350',
                                          'had_NJETS_0',
                                          'lep_LJPT_30_60',
                                          'lep_LJPT_60_120',
                                          'lep_LJPT_120_200',
                                          'lep_LJPT_200_350',
                                          'lep_LJPT_GE350',
                                          'lep_NJETS_0'])
        else:
            ggHHistogram = CompileSignals(theDirectory,
                                          'ggH',
                                          ['LJPT_30_60',
                                           'LJPT_60_120',
                                           'LJPT_120_200',
                                           'LJPT_200_350',
                                           'LJPT_GT350',
                                           'NJETS_0'])
            qqHHistogram = CompileSignals(theDirectory,
                                          'qqH',
                                          ['LJPT_30_60',
                                           'LJPT_60_120',
                                           'LJPT_120_200',
                                           'LJPT_200_350',
                                           'LJPT_GT350',
                                           'NJETS_0'])
            WHHistogram = CompileSignals(theDirectory,
                                         'WH',
                                         ['LJPT_30_60',
                                          'LJPT_60_120',
                                          'LJPT_120_200',
                                          'LJPT_200_350',
                                          'LJPT_GT350',
                                          'NJETS_0'])
            ZHHistogram = CompileSignals(theDirectory,
                                         'ZH',
                                         ['LJPT_30_60',
                                          'LJPT_60_120',
                                          'LJPT_120_200',
                                          'LJPT_200_350',
                                          'LJPT_GT350',
                                          'NJETS_0'])
        
    histograms['ggH'] = ggHHistogram
    histograms['qqH'] = qqHHistogram
    histograms['WH'] = WHHistogram
    histograms['ZH'] = ZHHistogram
    print('Making final signal...')
    signalHistogram = ggHHistogram.Clone()
    signalHistogram.Add(qqHHistogram)
    signalHistogram.Add(WHHistogram)
    signalHistogram.Add(ZHHistogram)
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

    print('Making others histogram...')
    othersHistogram= dibosonHistogram.Clone()
    print('single top...')
    othersHistogram.Add(singleTopHistogram)
    #print('signal...')
    #othersHistogram.Add(signalHistogram)
    histograms['other'] = othersHistogram    

    print('Getting the data...')
    # 
    print(channel)
    category=''
    if channel == 'mt' or channel == 'et' or channel == 'tt':
        category = re.search('(?<=[0-9]{4}_).*(?=_(pre|post)fit)',theDirectory.GetName()).group(0)
    print(category)

    dataFileName = 'smh'+year+channel+'_Differential.root'
    theDataFile = ROOT.TFile(os.environ['CMSSW_BASE']+'/src/auxiliaries/shapes/'+dataFileName)

    if channel == 'em':
        if args.measurementType == 'pth':
            theDataFileDirectory = theDataFile.Get('em_HiggsPt')
        elif args.measurementType == 'njets':
            theDataFileDirectory = theDataFile.Get('em_njets')
        elif args.measurementType == 'ljpt':
            theDataFileDirectory = theDataFile.Get('em_j1pt')
    else:
        if args.measurementType == 'pth':
            theDataFileDirectory = theDataFile.Get(channel+'_'+category+'_'+'HiggsPt')
        elif args.measurementType == 'njets':
            if channel == 'mt' or channel == 'tt':
                theDataFileDirectory = theDataFile.Get(channel+'_'+category+'_'+'NJets')
            else:
                theDataFileDirectory = theDataFile.Get(channel+'_'+category+'_'+'njets')
        elif args.measurementType == 'ljpt':
            if channel == 'mt':
                theDataFileDirectory = theDataFile.Get(channel+'_'+category+'_'+'LeadingJetPt')
            elif channel == 'et' or channel == 'em':
                theDataFileDirectory = theDataFile.Get(channel+'_'+category+'_'+'j1pt')
            elif channel == 'tt':
                theDataFileDirectory = theDataFile.Get(channel+'_'+category+'_'+'LJPT')

    #dataHistogram = theDirectory.Get('data_obs')
    dataHistogram = theDataFileDirectory.Get('data_obs')
    if not args.unblind:
        BlindDataHistogram(dataHistogram)
    histograms['data'] = dataHistogram

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
    theStackErrors = theDirectory.Get('TotalBkg')
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
    theBackgroundStack.GetXaxis().SetNdivisions(-900-(histograms['data'].GetNbinsX()/9))
    theBackgroundStack.SetTitle(fullTitle)
    theBackgroundStack.GetYaxis().SetTitle('Events')
    theStackErrors.Draw("SAME e2")    
    #histograms['signal'].Draw('SAME HIST')
    histograms['data'].Draw('SAME e1')

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
    latex.SetTextSize(0.04)
    latex.SetTextAlign(21)
    latex.SetTextFont(52)
    verticalLocation = max(theBackgroundStack.GetMaximum(),histograms['data'].GetMaximum())*10
    if args.measurementType == 'pth':
        if channel == 'tt' and category == 'HighTauPt':
            latex.DrawLatex(4.5,verticalLocation,'0<p_{t}^{H}<80')
            latex.DrawLatex(4.5+9,verticalLocation,'80<p_{t}^{H}<120')
            latex.DrawLatex(4.5+18,verticalLocation,'120<p_{t}^{H}<200')
            latex.DrawLatex(4.5+27,verticalLocation,'200<p_{t}^{H}<350')
            latex.DrawLatex(4.5+36,verticalLocation,'350<p_{t}^{H}<450')
            latex.DrawLatex(4.5+45,verticalLocation,'450<p_{t}^{H}')
        elif (channel == 'mt' or channel == 'et') and (category == 'LowTauPt' or category=='IntermediateTauPt'):
            latex.DrawLatex(4.5,verticalLocation,'0<p_{t}^{H}<45')
            latex.DrawLatex(4.5+9,verticalLocation,'45<p_{t}^{H}<80')
            latex.DrawLatex(4.5+18,verticalLocation,'80<p_{t}^{H}<120')
            latex.DrawLatex(4.5+27,verticalLocation,'120<p_{t}^{H}<200')
            latex.DrawLatex(4.5+36,verticalLocation,'200<p_{t}^{H}<350')
            latex.DrawLatex(4.5+45,verticalLocation,'350<p_{t}^{H}')
        else:
            latex.DrawLatex(4.5,verticalLocation,'0<p_{t}^{H}<45')
            latex.DrawLatex(4.5+9,verticalLocation,'45<p_{t}^{H}<80')
            latex.DrawLatex(4.5+18,verticalLocation,'80<p_{t}^{H}<120')
            latex.DrawLatex(4.5+27,verticalLocation,'120<p_{t}^{H}<200')
            latex.DrawLatex(4.5+36,verticalLocation,'200<p_{t}^{H}<350')
            latex.DrawLatex(4.5+45,verticalLocation,'350<p_{t}^{H}<450')
            latex.DrawLatex(4.5+54,verticalLocation,'450<p_{t}^{H}')
    if args.measurementType == 'njets':        
        latex.DrawLatex(4.5,verticalLocation,'N_{Jets}=0')
        latex.DrawLatex(4.5+9,verticalLocation,'N_{Jets}=1')
        latex.DrawLatex(4.5+18,verticalLocation,'N_{Jets}=2')
        latex.DrawLatex(4.5+27,verticalLocation,'N_{Jets}=3')
        latex.DrawLatex(4.5+36,verticalLocation,'N_{Jets}=4')
    if args.measurementType == 'ljpt':
        if channel == 'tt':
            latex.DrawLatex(4.5,verticalLocation,'30<p_{t}^{Jet}<60')
            latex.DrawLatex(4.5+9,verticalLocation,'60<p_{t}^{Jet}<120')
            latex.DrawLatex(4.5+18,verticalLocation,'120<p_{t}^{Jet}<200')
            latex.DrawLatex(4.5+27,verticalLocation,'200<p_{t}^{Jet}<350')
            latex.DrawLatex(4.5+36,verticalLocation,'350<p_{t}^{Jet}')            
            latex.DrawLatex(4.5+45,verticalLocation,'0 Jet')            
        else:
            latex.DrawLatex(4.5,verticalLocation,'0 Jet')
            latex.DrawLatex(4.5+9,verticalLocation,'30<p_{t}^{Jet}<60')
            latex.DrawLatex(4.5+18,verticalLocation,'60<p_{t}^{Jet}<120')
            latex.DrawLatex(4.5+27,verticalLocation,'120<p_{t}^{Jet}<200')
            latex.DrawLatex(4.5+36,verticalLocation,'200<p_{t}^{Jet}<350')
            latex.DrawLatex(4.5+45,verticalLocation,'350<p_{t}^{Jet}')

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

    plotGridHisto = histograms['data'].Clone()
    plotGridHisto.Reset()    
    plotGridHisto.SetTitle('')
    plotGridHisto.GetXaxis().SetLabelSize(0.0)
    plotGridHisto.GetYaxis().SetLabelSize(0.0)
    plotGridHisto.GetXaxis().SetTickLength(0.0)
    plotGridHisto.GetYaxis().SetTickSize(0.0)
    plotGridHisto.GetYaxis().SetTitleSize(0.0)
    plotGridHisto.Draw()
    plotGridHisto.GetXaxis().SetNdivisions(-900-(histograms['data'].GetNbinsX()/9))
    plotGridHisto.SetFillStyle(4000)

    #we also want a legend on this pad, so let's make that
    theLegend = ROOT.TLegend(0.12,0.78,0.88,0.88)
    theLegend.AddEntry(histograms['data'],'Obs.','P')
    theLegend.AddEntry(histograms['embedded'],'#tau#tau bkg.','F')
    if channel != 'em':
        theLegend.AddEntry(histograms['jetFakes'],'Jet#rightarrow#tau_{h} mis-ID','F')
    else:
        theLegend.AddEntry(histograms['jetFakes'],'Jet#rightarrow e/#mu} mis-ID','F')
    theLegend.AddEntry(histograms['ZL'],'Z #rightarrow ee/#mu#mu','F')
    theLegend.AddEntry(histograms['ttbar'],'t#bar{t} + Jets','F')
    theLegend.AddEntry(histograms['other'],'Others','F')
    theLegend.AddEntry(theStackErrors,'Bkg. uncertainty','F')
    theLegend.AddEntry(histograms['signal'],'Higgs #rightarrow #tau#tau','F')    

    theLegend.SetNColumns(4)
    theLegend.SetBorderSize(0)
    theLegend.SetFillStyle(0)
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
        setMassBinAxisTitles(ratioPlotErrors)
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
        dataSignalStrength.Draw('E0P SAME')
        unitRatioError.SetTitle('')
        unitRatioError.GetXaxis().SetLabelSize(0.1)
        setMassBinAxisTitles(unitRatioError)
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
    ratioGridHisto.GetXaxis().SetNdivisions(-900-(histograms['data'].GetNbinsX()/9))
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
    
