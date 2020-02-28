#!/usr/bin/env python
import argparse
import ROOT
import os
import CombineHarvester.Run2HTT_Combine.PlottingModules.prefitPostfitSettings as prefitPostfitSettings
import CombineHarvester.Run2HTT_Combine.PlottingModules.Utilities as Utils
import CombineHarvester.Run2HTT_Combine.PlottingModules.globalSettings as globalSettings
import CombineHarvester.Run2HTT_Combine.PlottingModules as plotModules

def MakePrefitPlots(tag,years,channels,DontPerformCalculation = False):
    globalSettings.style.setPASStyle()    
    ROOT.gROOT.SetStyle('pasStyle')
    

    theDirectory = os.environ['CMSSW_BASE']+"/src/CombineHarvester/Run2HTT_Combine/HTT_Output/Output_"+tag+"/"
    if not os.path.isdir(theDirectory):
        raise RuntimeError("Couldn't find the output directory. Check the tag to make sure you have the right one.")
    os.chdir(theDirectory)
    
    fileName = "fitDiagnostics"+tag+"_Inclusive.root"
    if not os.path.exists(fileName):
        raise RuntimeError("Coudn't find the output file. Are you sure you have the right directory and ran the option to store plots?")

    #let's go find the final card
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

    outputRootFile = ROOT.TFile("prefitHistos.root","RECREATE")

    needALegend = True
        
    plotFile = ROOT.TFile(prefitPostfitFile)    
    histograms = prefitPostfitSettings.RetrievePlots.RetrievePlotsFromAllDirectories(channels,plotFile,years)

    dataCards = []
    for channel in channels:
        for year in years:
            dataCard = ROOT.TFile.Open(prefitPostfitSettings.RetrievePlots.RetrieveOriginalDatacardPath(channel,year))
            dataCards.append(dataCard)
            for category in histograms[channel][year]:
                dataHistogram = dataCard.Get(category).Get("data_obs")
                for prefitOrPostfit in ['prefit','postfit']:
                    #retrieve original data                    
                    #print(dataHistogram)
                    histograms[channel][year][category][prefitOrPostfit]['Data']={'data_obs':dataHistogram}                    
    outputRootFile.cd()                                        
    #let's do the rebinning
    prefitPostfitSettings.rebinning.RebinCollection(histograms)    
    
    #let's add all the histograms together and get a run 2 collection as well now.
    #PROVIDED we have all the years present and accounted for
    print("Making run 2...")
    if ('2016' in years
        and '2017' in years
        and '2018' in years):
        prefitPostfitSettings.histogramAddition.PerformAllAdditions(histograms)

    for channel in channels:
        for year in histograms[channel]:
            for category in histograms[channel][year]:
                for prefitOrPostfit in ['prefit','postfit']:                                                            
                    prefitPostfitSettings.dataSettings.ApplyDataSettings(histograms[channel][year][category][prefitOrPostfit]['Data']['data_obs'])
                    #perform blinding                    
                    print("blinding...")
                    prefitPostfitSettings.blinding.BlindDataPoints(                        
                        histograms[channel][year][category][prefitOrPostfit]['Data'],
                        category,
                        year
                    )                    

                    #Create the canvas and pads needed
                    theCanvas = ROOT.TCanvas(channel+"_"+year+"_"+category+"_"+prefitOrPostfit,channel+"_"+year+"_"+category+"_"+prefitOrPostfit)
                    theCanvas.SetFillColor(0)
                    print("Performing pad set-up...")
                    plotPad,ratioPad = prefitPostfitSettings.plotPad.CreatePads(theCanvas)
                    prefitPostfitSettings.plotPad.SetupPad(plotPad)
                    #make the ratio plots
                    prefitPostfitSettings.ratioPad.SetUpRatioPad(ratioPad)
                    
                    #color in any distributions
                    print("Creating colors...")
                    prefitPostfitSettings.colors.ColorizePrefitDistribution(histograms[channel][year][category][prefitOrPostfit]['Slimmed'] )
                    prefitPostfitSettings.colors.ColorizePrefitDistribution(histograms[channel][year][category][prefitOrPostfit]['Signals'])
                    
                    #make the stack and errors
                    print("Making stack...")
                    backgroundStack = prefitPostfitSettings.stack.CreateStack(histograms[channel][year][category][prefitOrPostfit]['Slimmed'])
                    print("Making stack errors...")
                    backgroundStackErrors = Utils.MakeStackErrors(backgroundStack)
                    print("Making ratio...")
                    ratioPlot, ratioErrors = prefitPostfitSettings.ratioPlot.MakeRatioPlot(backgroundStack,
                                                                  histograms[channel][year][category][prefitOrPostfit]['Data']['data_obs'])
                                        

                    #draw everything
                    #upscale the higgs distribution
                    histograms[channel][year][category][prefitOrPostfit]['Signals']['Higgs'].Scale(20.0)                
                    print("Drawing...")
                    plotPad.cd()
                    plotPad.SetFillColor(0)
                    backgroundStack.SetMinimum(max(backgroundStack.GetMinimum()*0.9,0.1))
                    backgroundStack.SetMaximum(backgroundStack.GetMaximum()*10)
                    backgroundStack.Draw()
                    backgroundStackErrors.Draw("SAME e2")
                    histograms[channel][year][category][prefitOrPostfit]['Signals']['Higgs'].Draw("SAME HIST")
                    histograms[channel][year][category][prefitOrPostfit]['Data']['data_obs'].Draw("SAME e1")
                    #axes
                    #prefitPostfitSettings.axis.CreateAxisLabels(ratioPlot)
                    prefitPostfitSettings.axis.SetPlotYAxis(backgroundStack.GetHistogram()) 
                    #slice lines
                    plotSlicePad,plotSlices = prefitPostfitSettings.sliceLines.CreateSliceLines(category,backgroundStack.GetHistogram(),plotPad,year)
                    plotSlicePad.Draw()
                    plotSlicePad.cd()
                    plotSlices.Draw()
                    
                    #other text
                    plotModules.lumiText.CreateLumiText(year)
                    plotModules.CMStext.DrawCMSText()
                    #Titles                    
                    prefitPostfitSettings.title.CreateTitle(year,channel,category,backgroundStack)
                    plotModules.prefitText.CreatePrefitText()
                    prefitPostfitSettings.channelText.DrawCategoryName(category)
                    prefitPostfitSettings.channelText.DrawChannelName(channel)

                    #slice text
                    prefitPostfitSettings.sliceLabels.CreateSliceText(category,year)
                    #ratio plot
                    ratioPad.cd()
                    #ratioPlot.Draw("AP")
                    #ratioPlot.Draw('ex0')                    
                    
                    ratioErrors.Draw('e2')
                    ratioPlot.Draw('E0P')

                    #Axis junk
                    prefitPostfitSettings.axis.CreateAxisLabels(ratioErrors,category,year)                    
                    
                    ratioPlotSlicePad,ratioPlotSlices = prefitPostfitSettings.sliceLines.CreateRatioSliceLines(plotSlices,ratioPad)
                    ratioPlotSlicePad.Draw()
                    ratioPlotSlicePad.cd()
                    #ratioPad.cd()
                    ratioPlotSlices.Draw()

                    #prefitPostfitSettings.sliceLines.CreateRatioSliceLines(category,ratioPlot)
                    #print(ratioErrors.GetNdivisions())
                    #ratioErrors.GetXaxis().SetNdivisions(plotSlices.GetXaxis().GetNdivisions())                    
                    #print(ratioErrors.GetNdivisions())
                    
                    
                    
                    raw_input("Press enter to continue...")
                    
                    theCanvas.SaveAs(outputDir+theCanvas.GetName()+".png")
                    theCanvas.SaveAs(outputDir+theCanvas.GetName()+".pdf")
                    theCanvas.Write()
                    
                    del theCanvas
                    
                    if needALegend:
                        #create the legend
                        print("Creating legend...")
                        prefitPostfitSettings.legend.CreateLegend(histograms[channel][year][category][prefitOrPostfit]['Slimmed'])
                        prefitPostfitSettings.legend.AppendToLegend(histograms[channel][year][category][prefitOrPostfit]['Signals']['Higgs'],'Higgs')
                        prefitPostfitSettings.legend.AppendToLegend(histograms[channel][year][category][prefitOrPostfit]['Data']['data_obs'],'data_obs')
                        prefitPostfitSettings.legend.AppendToLegend(backgroundStackErrors,'background_error')
                        prefitPostfitSettings.legend.DrawLegend(outputDir)
                        needALegend = False
                                                    
    #write things to the files
    outputRootFile.Write()
    #for year in years:
    #    for channel in channels

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = "Create prefit plots from a fit diagnostic output file")
    parser.add_argument('--tag',nargs = "?",help="Tag of the output directory to create plots for",required=True)
    parser.add_argument('--years',nargs="+",choices=['2016','2017','2018'],help="year of results to run.",required=True)
    parser.add_argument('--channels',nargs="+",choices=['mt','tt','et','em'],help="specify the channels to run",required=True)
    parser.add_argument('--DontRecalculate',help="Dont preform the PostfitShapesFromWorkspace step again.",action = 'store_true')

    args = parser.parse_args()
    MakePrefitPlots(args.tag,args.years,args.channels,args.DontRecalculate)
