#!/usr/bin/env python
import ROOT
import argparse
import os

import CombineHarvester.Run2HTT_Combine.PlottingModules.globalSettings as globalSettings
import CombineHarvester.Run2HTT_Combine.PlottingModules.prefitPostfitSettings as prefitPostfitSettings
import CombineHarvester.Run2HTT_Combine.PlottingModules.Utilities as Utils
import CombineHarvester.Run2HTT_Combine.PlottingModules as plotModules

axisLabels = {
    'mt':{'MT':"MT",
          'dr':'#Delta R',
          'higgspt':'p_{t}^{H}',
          'met':"MET",
          'mjj':"m_{jj}",
          'mtt':"m_{#tau#tau}",
          'mueta':"#eta_{#mu}",
          'mupt':"p_{t}^{#mu}",
          'mvis':"m_{vis}",
          'njets':"N_{jets}",
          'taueta':"#eta_{#tau}",
          'taupt':"p_{t}^{#tau}",
          'jpt_1':"Leading Jet p_{t}",
          'jeta_1':"Leading Jet #eta",
          'jpt_2':"Sub-Leading Jet p_{t}",
          'jeta_2':"Sub-Leading Jet #eta",
          'DEtaJJ':"#Delta#eta_{jj}"},
    'tt':{},
    'et':{},
    'em':{},
    }
tickLabels = {
    'MT':(20,0.0,200.0),
    'dr':(40,0.0,6.0),
    'higgspt':(24,0.0,120.0),
    'met':(20,0.0,200.0),
    'mjj':(20,0.0,500.0),
    'mtt':(30,0.0,300.0),
    'mueta':(48,-2.4,2.4),
    'mupt':(30,20.0,80.0),
    'mvis':(30,50.0,200.0),
    'njets':(6,0.0,6.0),
    'taueta':(45,-2.5,2.5),
    'taupt':(25,30.0,80.0),
    'jpt_1':(50,0.0,200.0),
    'jeta_1':(50,-5.0,5.0),
    'jpt_2':(50,0.0,200.0),
    'jeta_2':(50,-5.0,5.0),
    'DEtaJJ':(25,0.0,2.5),
    }

def DrawControls(tag,year,channel,DontPerformCalculation=False):
    globalSettings.style.setPASStyle()
    ROOT.gROOT.SetStyle('pasStyle')

    theDirectory = os.environ['CMSSW_BASE']+"/src/CombineHarvester/Run2HTT_Combine/HTT_Output/Output_"+tag+"/"
    if not os.path.isdir(theDirectory):
        raise RuntimeError("Couldn't find the output directory. Check the tag to make sure you have the right one.")
    os.chdir(theDirectory)

    fileName = "fitDiagnostics"+tag+"_Inclusive.root"
    if not os.path.exists(fileName):
        raise RuntimeError("Coudn't find the output file. Are you sure you have the right directory and ran the option to store plots?")

    finalCardName = 'FinalCard_'+tag+'.root'
    finalTextCardName = 'FinalCard_'+tag+'.txt'
    if not os.path.exists(finalCardName) or not os.path.exists(finalTextCardName):
        raise RuntimeError("Failed to find the one of the original workspace cards (root/txt). Are you sure the fit all the way through?")

    #first things first, we need to make the actual prefit and post-fit shapes    
    prefitPostfitFile = 'FitHistos.root'
    if not DontPerformCalculation:
        prefitPostfitResult = os.system('PostFitShapesFromWorkspace -o '+prefitPostfitFile+' -m 125 -f '+fileName+':fit_s --postfit --sampling --print -d '+finalTextCardName+' -w '+finalCardName)
        assert prefitPostfitResult == 0, "There was an error while creating the prefits and postfits..."
        
    outputDir = theDirectory+"HistogramOutput/"
    if not os.path.isdir(outputDir):
        os.mkdir(outputDir)

    plotFile = ROOT.TFile(prefitPostfitFile)

    datacardLocation = os.environ['CMSSW_BASE']+"/src/auxiliaries/shapes/"+channel+"_controls_"+year+".root"
    datacardFile = ROOT.TFile(datacardLocation)

    #Alright, let's loop over directories/variables in the the original data card, then we can get the corresponding
    #directory in the prefit file, from there we can get the appropriate stuff to make histograms out of.
    
    #datacardFile.ls()
    #plotFile.ls()

    for directoryKey in datacardFile.GetListOfKeys():
        directoryName = directoryKey.GetName()
        print directoryName
        #print(directoryName)
        dataDirectory = datacardFile.Get(directoryName)
        prefitDirectoryName = directoryName+"_"+year+"_prefit"
        #print(prefitDirectoryName)
        prefitDirectory = plotFile.Get(prefitDirectoryName)

        #dataDirectory.ls()
        #prefitDirectory.ls()

        theCanvas = ROOT.TCanvas("Canvas_"+directoryName,directoryName)
        
        plotPad,ratioPad = prefitPostfitSettings.plotPad.CreatePads(theCanvas)
        prefitPostfitSettings.plotPad.SetupPad(plotPad)
        #make the ratio plots
        prefitPostfitSettings.ratioPad.SetUpRatioPad(ratioPad)
        plotPad.SetLogy(False)
                
        Data = dataDirectory.Get("data_obs")
        VVL = prefitDirectory.Get("VVL")
        VVT = prefitDirectory.Get("VVT")
        WH_htt = prefitDirectory.Get("WH_htt125")
        WH_hww = prefitDirectory.Get("WH_hww125")
        ZH_htt = prefitDirectory.Get("ZH_htt125")
        ZH_hww = prefitDirectory.Get("ZH_hww125")
        ZL = prefitDirectory.Get("ZL")
        embedded = prefitDirectory.Get("embedded")
        ggH_htt = prefitDirectory.Get("ggH_htt125")
        ggH_hww = prefitDirectory.Get("ggH_hww125")
        jetFakes = prefitDirectory.Get("jetFakes")
        qqH_htt = prefitDirectory.Get("qqH_htt125")
        qqH_hww = prefitDirectory.Get("qqH_hww125")
        STL = prefitDirectory.Get("STL")
        STT = prefitDirectory.Get("STT")
        TTL = prefitDirectory.Get("TTL")
        TTT = prefitDirectory.Get("TTT")

        TT = TTL.Clone()
        TT.Add(TTT)
        
        Other = VVL.Clone()
        Other.Add(VVT)
        Other.Add(WH_htt)
        Other.Add(WH_hww)
        Other.Add(ZH_hww)
        Other.Add(ZH_htt)
        Other.Add(ggH_htt)
        Other.Add(ggH_hww)
        Other.Add(qqH_htt)
        Other.Add(qqH_hww)

        AllHiggs = ggH_htt.Clone()
        AllHiggs.Add(qqH_htt)
        AllHiggs.Add(WH_htt)
        AllHiggs.Add(ZH_htt)

        Data.SetMarkerStyle(20)
        Data.Sumw2()

        jetFakes.SetLineColor(ROOT.kBlack)
        jetFakes.SetFillColor(ROOT.TColor.GetColor("#ffccff"))
        
        embedded.SetLineColor(ROOT.kBlack)
        embedded.SetFillColor(ROOT.TColor.GetColor("#ffcc66"))
        
        ZL.SetLineColor(ROOT.kBlack)
        ZL.SetFillColor(ROOT.TColor.GetColor("#4496c8"))
        
        TT.SetLineColor(ROOT.kBlack)
        TT.SetFillColor(ROOT.TColor.GetColor("#9999cc"))

        Other.SetLineColor(ROOT.kBlack)
        Other.SetFillColor(ROOT.TColor.GetColor("#12cadd"))

        AllHiggs.SetLineColor(ROOT.kRed)
        AllHiggs.SetLineWidth(3)
        AllHiggs.Scale(50)

        backgroundStack = ROOT.THStack("backgroundStack","backgroundStack")

        backgroundStack.Add(jetFakes,"hist")
        backgroundStack.Add(TT,"hist")
        backgroundStack.Add(ZL,"hist")
        backgroundStack.Add(Other,"hist")
        backgroundStack.Add(embedded,"hist")

        backgroundStackErrors = Utils.MakeStackErrors(backgroundStack)
        ratioPlot, ratioErrors = prefitPostfitSettings.ratioPlot.MakeRatioPlot(backgroundStack,Data)
                
        plotPad.cd()
        plotPad.SetFillColor(0)
        backgroundStack.SetMinimum(0.0)
        backgroundStack.SetMaximum(backgroundStack.GetMaximum()*1.05)
        backgroundStack.Draw()
        backgroundStackErrors.Draw("SAME e2")
        backgroundStack.GetHistogram().GetYaxis().SetTitle("Events")
        backgroundStack.GetHistogram().GetYaxis().CenterTitle()
        #backgroundStack.GetHistogram().SetTitle(axisLabels[channel][directoryName])
        backgroundStack.GetHistogram().GetYaxis().SetTitleOffset(1.5)        
        AllHiggs.Draw("SAME HIST")
        Data.Draw("SAME e1")

        plotModules.lumiText.CreateLumiText(year)
        plotModules.CMStext.DrawCMSText()
        
        ratioPad.cd()        
        ratioErrors.Draw('e2')
        ratioErrors.GetXaxis().SetTitle(axisLabels[channel][directoryName])
        ratioErrors.GetXaxis().SetTitleOffset(1.5)        
        #ratioErrors.GetXaxis().SetLabelSize(0.10)
        #ratioErrors.GetXaxis().SetLabelOffset(0.02)
        ratioErrors.SetLabelSize(0.0)
        ratioErrors.GetXaxis().SetTitleSize(0.12)
        ratioPlot.Draw('E0P')        
        #ratioErrors.GetXaxis().SetBinLabel(1,str(tickLabels[directoryName][0]))
        #ratioErrors.GetXaxis().SetBinLabel(ratioErrors.GetNbinsX(),str(tickLabels[directoryName][1]))
        #let's get some better tick labeling on there.
        """
        axisPad = ROOT.TPad("axis_"+ratioPad.GetName(),"axis_"+ratioPad.GetName(),0,0,1,1)
        #axisPad.SetTopMargin(ratioPad.GetTopMargin())
        #axisPad.SetBottomMargin(ratioPad.GetBottomMargin())        
        axisPad.SetGridx()
        axisPad.SetGridy()
        axisPad.SetFillStyle(4100)
        axisPad.cd()
        axisPad.Draw()
        axisHisto = ROOT.TH1F("Axes","Axes",tickLabels[directoryName][0],tickLabels[directoryName][1],tickLabels[directoryName][2])        
        axisHisto = ratioErrors.Clone()
        axisHisto.Reset()
        axisHisto.GetYaxis().SetRangeUser(0.5,1.5)
        axisHisto.GetYaxis().SetLabelSize(0.0)
        axisHisto.Draw()  

        axisPad.Draw()
        """
        theAxis=ROOT.TGaxis(ratioErrors.GetXaxis().GetXmin(),
                            0.5,
                            ratioErrors.GetXaxis().GetXmax(),
                            0.5,
                            tickLabels[directoryName][1],
                            tickLabels[directoryName][2],
                            510,
                            '')
        theAxis.SetLabelSize(0.1)
        theAxis.SetLabelOffset(0.02)
        theAxis.Draw()
        

        #Create a legend
        plotPad.cd()
        legend = ROOT.TLegend(0.68,0.65,0.95,0.92)
        legend.AddEntry(Data,"Observed","pe")
        legend.AddEntry(embedded,"Embedded","f")
        legend.AddEntry(Other,"Other","f")
        legend.AddEntry(ZL,"Z #rightarrow ee/#mu#mu","f")
        legend.AddEntry(TT,"t#bar{t} + Jets","f")
        legend.AddEntry(jetFakes,"Jet mis-ID",'f')
        legend.AddEntry(ratioErrors,"Bkg. uncertainty","f")
        legend.AddEntry(AllHiggs,"Standard Model Higgs (#times 50)","l")

        legend.SetBorderSize(0)
        
        legend.Draw()
        
        #raw_input("Press Enter to Continue...")

        theCanvas.SaveAs(outputDir+"/"+directoryName+".png")
        theCanvas.SaveAs(outputDir+"/"+directoryName+".pdf")
        
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = "Create control plots from a fit diagnostic output file")
    parser.add_argument('--tag',nargs='?',help="Tag of the output directory to create plots for",required = True)
    parser.add_argument('--year',nargs='?',choices=['2016','2017','2018'],help="data year",required=True)
    parser.add_argument('--channel',nargs="?",choices=['mt','tt','et','em'],help="data channel",required=True)
    parser.add_argument('--DontRecalculate',help="Dont perform the PostfitShapesFromWorkspace step again.",action = 'store_true')

    args = parser.parse_args()
    DrawControls(args.tag,args.year,args.channel,args.DontRecalculate)
