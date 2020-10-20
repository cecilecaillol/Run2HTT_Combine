#!/usr/bin/env python
import os
import argparse
import ROOT
import logging
import datetime
import string
import random
import CombineHarvester.Run2HTT_Combine.CategoryConfigurations as cfg
import CombineHarvester.Run2HTT_Combine.CategoryMaps as CategoryMaps
from CombineHarvester.Run2HTT_Combine.EmbeddedConfiguration import EmbeddedConfiguration as embedded_cfg
from CombineHarvester.Run2HTT_Combine.SplitUncertainty import UncertaintySplitter
from CombineHarvester.Run2HTT_Combine.ThreadManager import ThreadManager
import CombineHarvester.Run2HTT_Combine.outputArea as outputArea

parser = argparse.ArgumentParser(description = "Central script for running the differential analysis fits")
parser.add_argument('--years',nargs="+",choices=['2016','2017','2018'],help="Specify the year(s) to run the fit for",required=True)
parser.add_argument('--channels',nargs="+",choices=['mt','et','tt','em'],help="specify the channels to create data cards for",required=True)
parser.add_argument('--MeasurementType',nargs = '?', choices=['mjj','pth','njets','ljpt'],help="Specify the kind of differential measurement to make",required=True)
parser.add_argument('--DecorrelateForMe',help="Run the decorrelator as part of the overall run.",action="store_true")
parser.add_argument('--MakePlots',help="run a special fit dedicated to extracting relevant plots",action="store_true")
parser.add_argument('--RunShapeless',help="Run combine model without using any shape uncertainties",action="store_true")
parser.add_argument('--workspaceOnly',help='Create the text cards, and workspaces only, and then exit. Do not attempt any fits.',action='store_true')
parser.add_argument('--fiducialCrossSection',help='create a fiducial cross section measurement, not a standard full parameter sweeping measurement',action='store_true')
parser.add_argument('--ComputeGOF',help="Compute saturated GOF use on forcefully blinded datacards",action="store_true")
parser.add_argument('--Unblind',help="Unblind the analysis, and do it for real. BE SURE ABOUT THIS.",action="store_true")
parser.add_argument('--DontPrintResults',help='For use in unblinding carefully. Doesn\'t print the acutal results to screen or draw them on any plots',action="store_true")

args = parser.parse_args()

DateTag,OutputDir = outputArea.PrepareNewOutputArea()

logging.basicConfig(filename=OutputDir+"DifferentialCombineHistory_"+DateTag+".log",filemode="w",level=logging.INFO,format='%(asctime)s %(message)s')

outputLoggingFile = "outputLog_"+DateTag+".txt"

for year in args.years:
    for channel in args.channels:
        if args.DecorrelateForMe:
            AddShapeCommand = "python scripts/PrepDecorrelatedCard.py --year "+year+" --DataCard "+os.environ['CMSSW_BASE']+"/src/auxiliaries/shapes/smh"+year+channel+"_Differential_nocorrelation.root --OutputFileName "+os.environ['CMSSW_BASE']+"/src/auxiliaries/shapes/smh"+year+channel+"_Differential.root "
            if channel == "et" or channel == "em":
                AddShapeCommand += "--TrimYears"
            print("Duplicating shapes for year correlations")
            logging.info("Shape duplication command:")
            logging.info('\n\n'+AddShapeCommand+'\n')
            os.system(AddShapeCommand+" | tee -a "+outputLoggingFile)            

        DataCardCreationCommand="SMHTT"+year
        DataCardCreationCommand+="_"+channel+" "+OutputDir
        DataCardCreationCommand+=" -b "
        if args.RunShapeless:
            DataCardCreationCommand+=' -s '
        #depending on the type of measurement we're running, we also want to pass 
        # a new parameter to the card to handle the new type of signal.
        if args.MeasurementType=='mjj':
            DataCardCreationCommand+=" -dm "
        elif args.MeasurementType=="pth":
            DataCardCreationCommand+= ' -dp '
        elif args.MeasurementType=="njets":
            DataCardCreationCommand+= ' -dn '
        elif args.MeasurementType=='ljpt':
            DataCardCreationCommand+= ' -dljpt '
        DataCardCreationCommand+=" --Categories"        
        if channel == 'tt':
            measurementString = ''
            if args.MeasurementType == 'pth':
                measurementString = 'HiggsPt'
            elif args.MeasurementType == 'njets':
                measurementString = 'NJets'
            elif args.MeasurementType == 'ljpt':
                measurementString = 'LJPT'
            DataCardCreationCommand+=" "+channel+"_LowTauPt_"+measurementString
            DataCardCreationCommand+=" "+channel+"_IntermediateTauPt_"+measurementString
            DataCardCreationCommand+=" "+channel+"_HighTauPt_"+measurementString
        elif channel == 'mt':
            measurementString = ''
            if args.MeasurementType == 'pth':
                measurementString = 'HiggsPt'
            elif args.MeasurementType == 'njets':
                measurementString = 'NJets'
            elif args.MeasurementType == 'ljpt':
                measurementString = 'LeadingJetPt'
            DataCardCreationCommand+=" "+channel+"_LowTauPt_"+measurementString
            DataCardCreationCommand+=" "+channel+"_IntermediateTauPt_"+measurementString            
            DataCardCreationCommand+=" "+channel+"_HighTauPt_"+measurementString
        elif channel == 'et':
            measurementString = ''
            if args.MeasurementType == 'pth':
                measurementString = 'HiggsPt'
            elif args.MeasurementType == 'njets':
                measurementString = 'njets'
            elif args.MeasurementType == 'ljpt':
                measurementString = 'j1pt'
            DataCardCreationCommand+=" "+channel+"_LowTauPt_"+measurementString
            DataCardCreationCommand+=" "+channel+"_IntermediateTauPt_"+measurementString
            DataCardCreationCommand+=" "+channel+"_HighTauPt_"+measurementString
        elif channel == 'em':
            measurementString = ''
            if args.MeasurementType == 'pth':
                measurementString = 'HiggsPt'
            elif args.MeasurementType == 'njets':
                measurementString = 'njets'
            elif args.MeasurementType == 'ljpt':
                measurementString = 'j1pt'
            DataCardCreationCommand+=" "+channel+"_"+measurementString
            
        print("Creating data cards")
        logging.info('Data Card Creation Command:')
        logging.info('\n\n'+DataCardCreationCommand+'\n')
        os.system(DataCardCreationCommand+" | tee -a "+outputLoggingFile)

#Combine all of our cards together
CombinedCardName= OutputDir+"FinalCard_"+DateTag+".txt"
CardCombiningCommand="combineCards.py"
for year in args.years:
    for channel in args.channels:
        #add in the autoMCstats
        nCategories = 0
        #need to do this so we don't accidentally create cards by appending text to non-existing ones
        if channel == 'em':
            nCategories = 1
        else:
            nCategories = 3
        for i in range(1,nCategories+1):
            CardFile = open(OutputDir+"smh"+year+"_"+channel+"_"+str(i)+"_13TeV_.txt","a+")
            CardFile.write("* autoMCStats 0.0\n")
            CardFile.close()
        #now at this point we have a bit of a divergence with the normal analysis.
        # we may not necessarily want all of these cards
        #mjj measurements do not use the high and low mjj categories
        #njets measurements do not use the mjj rolled category
        if channel == 'em':
            CardCombiningCommand+=" "+channel+"_"+year+"="+OutputDir+"smh"+year+"_"+channel+"_1_13TeV_.txt"
        else:
            CardCombiningCommand+=" "+channel+"_"+year+"_LowTauPt="+OutputDir+"smh"+year+"_"+channel+"_1_13TeV_.txt"
            CardCombiningCommand+=" "+channel+"_"+year+"_IntermediateTauPt="+OutputDir+"smh"+year+"_"+channel+"_2_13TeV_.txt"            
            CardCombiningCommand+=" "+channel+"_"+year+"_HighTauPt="+OutputDir+"smh"+year+"_"+channel+"_3_13TeV_.txt"

CardCombiningCommand+=" > "+CombinedCardName
logging.info("Final Card Combining Command:")
logging.info('\n\n'+CardCombiningCommand+'\n')
os.system(CardCombiningCommand+" | tee -a "+outputLoggingFile)

#Okay, at this point, we now diverge significantly from the normal analysis
#we have a pretty precise way we need to set up cards to get the overall signal extraction right
print("Setting up the workspace")
parametersToMeasure = []
WorkspaceName = OutputDir+"Workspace_"+args.MeasurementType+".root"
WorkspaceCommand = "text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel "
if args.fiducialCrossSection:
    if args.MeasurementType == 'pth':
        WorkspaceCommand = "text2workspace.py -P CombineHarvester.Run2HTT_Combine.Models.FiducialPTHModel:fiducialPTH "
        parametersToMeasure = [
            'rho_0_45',
            'rho_45_80',
            'rho_80_120',
            'rho_120_200',
            'rho_200_350',
            'rho_350_450',
            'mu_fid',
            ]
    elif args.MeasurementType == 'njets':
        WorkspaceCommand = "text2workspace.py -P CombineHarvester.Run2HTT_Combine.Models.FiducialNJETSModel:fiducialNJETS "
        parametersToMeasure = [
            'rho_0',
            'rho_1',
            'rho_2',
            'rho_3',            
            'mu_fid',
            ]
    elif args.MeasurementType == 'ljpt':
        raise RuntimeError("Proper fiducial cross section measurement for leading jet pt not written yet. FIX ME!")
else:
    if args.MeasurementType == "pth":
        WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_0_45.*htt.*:r_H_PTH_0_45[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_45_80.*htt.*:r_H_PTH_45_80[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_80_120.*htt.*:r_H_PTH_80_120[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_120_200.*htt.*:r_H_PTH_120_200[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_200_350.*htt.*:r_H_PTH_200_350[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_350_450.*htt.*:r_H_PTH_350_450[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_G.450.*htt.*:r_H_PTH_GT450[1,-25,25]' "
        parametersToMeasure = [        
            'r_H_PTH_0_45',
            'r_H_PTH_45_80',
            'r_H_PTH_80_120',
            'r_H_PTH_120_200',
            'r_H_PTH_200_350',
            'r_H_PTH_350_450',
            'r_H_PTH_GT450'
            ]
    elif args.MeasurementType == "mjj":
        WorkspaceCommand += "--PO 'map=.*/.*H.*MJJ_0_150.*htt.*:r_H_MJJ_0_150[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*MJJ_150_300.*htt.*:r_H_MJJ_150_300[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*MJJ_300_450.*htt.*:r_H_MJJ_300_450[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*MJJ_450_600.*htt.*:r_H_MJJ_450_600[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*MJJ_600_1000.*htt.*:r_H_MJJ_600_1000[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*MJJ_1000_1400.*htt.*:r_H_MJJ_1000_1400[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*MJJ_1400_1800.*htt.*:r_H_MJJ_1400_1800[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*MJJ_GE1800.*htt.*:r_H_MJJ_GE1800[1,-25,25]' "
        parametersToMeasure = [
            'r_H_MJJ_0_150',
            'r_H_MJJ_150_300',
            'r_H_MJJ_300_450',
            'r_H_MJJ_450_600',
            'r_H_MJJ_600_1000',
            'r_H_MJJ_1000_1400',
            'r_H_MJJ_1400_1800',
            'r_H_MJJ_GE1800'
        ]
    elif args.MeasurementType == 'njets':
        WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_0.*htt.*:r_H_NJETS_0[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_1.*htt.*:r_H_NJETS_1[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_2.*htt.*:r_H_NJETS_2[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_3.*htt.*:r_H_NJETS_3[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_GE4.*htt.*:r_H_NJETS_GE4[1,-25,25]' "
        parametersToMeasure=[
            'r_H_NJETS_0',
            'r_H_NJETS_1',
            'r_H_NJETS_2',
            'r_H_NJETS_3',
            'r_H_NJETS_GE4'
        ]
    elif args.MeasurementType == 'ljpt':
        WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_30_60.*htt.*:r_H_LJPT_30_60[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_60_120.*htt.*:r_H_LJPT_60_120[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_120_200.*htt.*:r_H_LJPT_120_200[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_200_350.*htt.*:r_H_LJPT_200_350[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_G.350.*htt.*:r_H_LJPT_GT350[1,-25,25]' "
        WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_0.*htt.*:r_H_NJETS_0[1,-25,25]' "
        parametersToMeasure=[
            'r_H_LJPT_30_60',        
            'r_H_LJPT_60_120',
            'r_H_LJPT_120_200',
            'r_H_LJPT_200_350',
            'r_H_LJPT_GT350',
            'r_H_NJETS_0',
        ]
WorkspaceCommand+= CombinedCardName+" -o "+WorkspaceName+" -m 125"

logging.info("Workspace Command:")
logging.info('\n\n'+WorkspaceCommand+'\n')
os.system(WorkspaceCommand+" | tee -a "+outputLoggingFile)

#terminate here if we only wanted a workspace
if args.workspaceOnly:
    #move the log file into output
    os.system('mv '+outputLoggingFile+' '+OutputDir)
    #move anything we may have made in parallel, or that may be left over to the output
    os.system(" mv *"+DateTag+"* "+OutputDir)

    outputArea.PrintSessionInfo(DateTag)
    exit()

for parameter in parametersToMeasure:

    if args.fiducialCrossSection and 'rho' in parameter: #these are irrelevant in the overall measurement
        continue
    MeasurementCommand = "combineTool.py -M MultiDimFit "+WorkspaceName+"  --robustFit=1 --X-rtd MINIMIZER_analytic --X-rtd FAST_VERTICAL_MORPH --algo=singles --cl=0.68 -n "+DateTag+"_"+args.MeasurementType
    if not args.Unblind:
        MeasurementCommand+=" -t -1"
    MeasurementCommand+=" --setParameters "
    for parameterName in parametersToMeasure:
        MeasurementCommand+=parameterName+"=1,"
    MeasurementCommand+= " -P "+parameter+" --floatOtherPOIs=1"

    logging.info("Measurement Command:")
    logging.info("\n\n"+MeasurementCommand+"\n")
    if args.DontPrintResults:
        os.system(MeasurementCommand+" > /dev/null")
    else:
        os.system(MeasurementCommand+" | tee -a "+outputLoggingFile)        
    os.system(" mv *"+DateTag+"* "+OutputDir)

if args.MakePlots:
    plotMakingCommand = "combineTool.py -M FitDiagnostics "+WorkspaceName+" --robustFit=1 --X-rtd MINIMIZER_analytic --cl=0.68 --saveShapes --plots --expectSignal=1 -n "+DateTag+"_Plots "
    if not args.Unblind:
        plotMakingCommand+=' -t -1' 
    plotMakingCommand+= " --setParameters "
    for parameterName in parametersToMeasure:
        plotMakingCommand+=parameterName+"=1,"
    logging.info("General plot making command:")
    logging.info('\n\n'+plotMakingCommand+'\n')
    if args.DontPrintResults:
        os.system(plotMakingCommand+" > /dev/null ")
    else:
        os.system(plotMakingCommand+" | tee -a "+outputLoggingFile)
    os.system(" mv *"+DateTag+"* "+OutputDir)
    
    #Run the postfit processing
    currentDir = os.getcwd()
    os.chdir(OutputDir)
    prefitPostfitProcessingCommand = 'PostFitShapesFromWorkspace -o prefitFile_'+DateTag+'.root -m 125 -f fitDiagnostics'+DateTag+'_Plots.root:fit_s --postfit --sampling --print -d FinalCard_'+DateTag+'.txt -w Workspace_'+args.MeasurementType+'.root'
    logging.info('Compelete pre/post-fit processing command:')
    logging.info('\n\n'+prefitPostfitProcessingCommand+'\n')
    os.system(prefitPostfitProcessingCommand)
    os.chdir(currentDir)

if args.ComputeGOF:
    os.chdir(OutputDir)
    GOFJsonName = 'gof_final_'+DateTag+'.json'

    ImpactCommand = "combineTool.py -M GoodnessOfFit --algorithm saturated -m 125 --there -d " + WorkspaceName+" -n '.saturated.toys'  -t 25 -s 0:19:1 --parallel 12"
    if args.DontPrintResults:
        os.system(ImpactCommand+" > /dev/null")
    else:
        os.system(ImpactCommand+" | tee -a "+outputLoggingFile)

    ImpactCommand = "combineTool.py -M GoodnessOfFit --algorithm saturated -m 125 --there -d " + WorkspaceName+" -n '.saturated'"
    if args.DontPrintResults:
        os.system(ImpactCommand+" > /dev/null")
    else:
        os.system(ImpactCommand+" | tee -a "+outputLoggingFile)

    ImpactCommand = "combineTool.py -M CollectGoodnessOfFit --input higgsCombine.saturated.GoodnessOfFit.mH125.root higgsCombine.saturated.toys.GoodnessOfFit.mH125.*.root -o "+GOFJsonName
    if args.DontPrintResults:
        os.system(ImpactCommand+" > /dev/null")
    else:
        os.system(ImpactCommand+" | tee -a "+outputLoggingFile)

    ImpactCommand = "python ../../../CombineTools/scripts/plotGof.py --statistic saturated --mass 125.0 "+GOFJsonName+" --title-right='' --output='saturated' --title-left='All GoF'"
    if args.DontPrintResults:
        os.system(ImpactCommand+" > /dev/null")
    else:
        os.system(ImpactCommand+" | tee -a "+outputLoggingFile)
    
    for year in args.years:
        for channel in args.channels:
            if channel=="mt":
                channelTitle = "#mu#tau"
            if channel=="et":
                channelTitle = "e#tau"
            if channel=="tt":
                channelTitle = "#tau#tau"
            if channel=="em":
                channelTitle = "e#mu"            
            TheFile = ROOT.TFile(os.environ['CMSSW_BASE']+"/src/auxiliaries/shapes/smh"+year+channel+"_Differential.root")
            #okay, we have to do some finicky channel specific junk here.
            #because the number of categories, and what's in a card 
            #differs
            if channel == 'mt' or channel == 'et' or channel == 'tt':
                rangeEnd = 4
            elif channel == 'em':
                rangeEnd = 2
            for i in range(1,rangeEnd):
                ImpactCommand = 'text2workspace.py -m 125 smh'+year+"_"+channel+"_"+str(i)+"_13TeV_.txt "
                if args.DontPrintResults:
                    os.system(ImpactCommand+" > /dev/null")
                else: 
                    os.system(ImpactCommand+" | tee -a "+outputLoggingFile)
                GOFJsonName = "gof_"+channel+"_"+year+"_"+str(i)+"_"+DateTag+".json"
                ImpactCommand = "combineTool.py -M GoodnessOfFit --algorithm saturated -m 125 --there -d smh"+year+"_"+channel+"_"+str(i)+"_13TeV_.root -n '.saturated."+year+"_"+channel+"_"+str(i)+".toys'  -t 25 -s 0:19:1 --parallel 12"
                if args.DontPrintResults:
                    os.system(ImpactCommand+" > /dev/null")
                else:
                    os.system(ImpactCommand+" | tee -a "+outputLoggingFile)
                ImpactCommand = "combineTool.py -M GoodnessOfFit --algorithm saturated -m 125 --there -d smh"+year+"_"+channel+"_"+str(i)+"_13TeV_.root -n '.saturated."+year+"_"+channel+"_"+str(i)+"'"
                if args.DontPrintResults:
                    os.system(ImpactCommand+" > /dev/null")
                else:
                    os.system(ImpactCommand+" | tee -a "+outputLoggingFile)

                ImpactCommand = "combineTool.py -M CollectGoodnessOfFit --input higgsCombine.saturated."+year+"_"+channel+"_"+str(i)+".GoodnessOfFit.mH125.root higgsCombine.saturated."+year+"_"+channel+"_"+str(i)+".toys.GoodnessOfFit.mH125.*.root -o "+GOFJsonName
                os.system(ImpactCommand+" | tee -a "+outputLoggingFile)

                titleRight = ''
                if channel == 'mt' or channel == 'et' or channel == 'tt':
                    if i == 1:
                        titleRight = 'Low Tau Pt'
                    if i == 2:
                        titleRight = 'Medium Tau Pt'
                    if i == 3:
                        titleRight = 'High Tau Pt'
                if channel == 'em':
                    titleRight = 'Overall em category'
                ImpactCommand = "python ../../../CombineTools/scripts/plotGof.py --statistic saturated --mass 125.0 "+GOFJsonName+" --title-right='' --output='saturated_"+year+"_"+channel+"_"+str(i)+"' --title-left='"+year+" "+channelTitle+"' --title-right='"+titleRight+"'"
                if args.DontPrintResults:
                    os.system(ImpactCommand+" > /dev/null")
                else:
                    os.system(ImpactCommand+" | tee -a "+outputLoggingFile)
                      
outputArea.PrintSessionInfo(DateTag)
