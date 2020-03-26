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
parser.add_argument('--MeasurementType',nargs = '?', choices=['mjj','pth','njets'],help="Specify the kind of differential measurement to make",required=True)
parser.add_argument('--DecorrelateForMe',help="Run the decorrelator as part of the overall run.",action="store_true")
parser.add_argument('--MakePlots',help="run a special fit dedicated to extracting relevant",action="store_true")

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
        #depending on the type of measurement we're running, we also want to pass 
        # a new parameter to the card to handle the new type of signal.
        if args.MeasurementType=='mjj':
            DataCardCreationCommand+=" -dm "
        elif args.MeasurementType=="pth":
            DataCardCreationCommand+= ' -dp '
        elif args.MeasurementType=="njets":
            DataCardCreationCommand+= ' -dn '
        DataCardCreationCommand+=" --Categories"
        DataCardCreationCommand+=" "+channel+"_0jet"
        DataCardCreationCommand+=" "+channel+"_1jet"
        DataCardCreationCommand+=" "+channel+"_2jetlow"
        DataCardCreationCommand+=" "+channel+"_2jethigh"
        DataCardCreationCommand+=" "+channel+"_3jetlow"
        DataCardCreationCommand+=" "+channel+"_3jethigh"
        DataCardCreationCommand+=" "+channel+"_2jet"
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
        for i in range(1,8):
            CardFile = open(OutputDir+"smh"+year+"_"+channel+"_"+str(i)+"_13TeV_.txt","a+")
            CardFile.write("* autoMCStats 0.0\n")
            CardFile.close()
        #now at this point we have a bit of a divergence with the normal analysis.
        # we may not necessarily want all of these cards
        #mjj measurements do not use the high and low mjj categories
        #njets measurements do not use the mjj rolled category
        CardCombiningCommand+=" "+channel+"_"+year+"_0jet="+OutputDir+"smh"+year+"_"+channel+"_1_13TeV_.txt"
        CardCombiningCommand+=" "+channel+"_"+year+"_1jet="+OutputDir+"smh"+year+"_"+channel+"_2_13TeV_.txt"
        if not args.MeasurementType == "mjj":
            CardCombiningCommand+=" "+channel+"_"+year+"_2jetlow="+OutputDir+"smh"+year+"_"+channel+"_3_13TeV_.txt"
            CardCombiningCommand+=" "+channel+"_"+year+"_2jethigh="+OutputDir+"smh"+year+"_"+channel+"_4_13TeV_.txt"
            CardCombiningCommand+=" "+channel+"_"+year+"_3jetlow="+OutputDir+"smh"+year+"_"+channel+"_5_13TeV_.txt"
            CardCombiningCommand+=" "+channel+"_"+year+"_3jethigh="+OutputDir+"smh"+year+"_"+channel+"_6_13TeV_.txt"
        if args.MeasurementType == "mjj":
            CardCombiningCommand+=" "+channel+"_"+year+"_2jet="+OutputDir+"smh"+year+"_"+channel+"_7_13TeV_.txt"

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
if args.MeasurementType == "pth":
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_0_20.*htt.*:r_H_PTH_0_20[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_20_45.*htt.*:r_H_PTH_20_45[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_45_80.*htt.*:r_H_PTH_45_80[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_80_120.*htt.*:r_H_PTH_80_120[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_120_200.*htt.*:r_H_PTH_120_200[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_200_350.*htt.*:r_H_PTH_200_350[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_350_600.*htt.*:r_H_PTH_350_600[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_GE600.*htt.*:r_H_PTH_GE600[1,-25,25]' "
    parametersToMeasure = [        
        'r_H_PTH_0_20',
        'r_H_PTH_20_45',
        'r_H_PTH_45_80',
        'r_H_PTH_80_120',
        'r_H_PTH_120_200',
        'r_H_PTH_200_350',
        'r_H_PTH_350_600',
        'r_H_PTH_GE600'
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
    WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_GE3.*htt.*:r_H_NJETS_GE3[1,-25,25]' "
    parametersToMeasure=[
        'r_H_NJETS_0',
        'r_H_NJETS_1',
        'r_H_NJETS_2',
        'r_H_NJETS_GE3'
    ]
WorkspaceCommand+= CombinedCardName+" -o "+WorkspaceName+" -m 125"

logging.info("Workspace Command:")
logging.info('\n\n'+WorkspaceCommand+'\n')
os.system(WorkspaceCommand+" | tee -a "+outputLoggingFile)

for parameter in parametersToMeasure:

    MeasurementCommand = "combineTool.py -M MultiDimFit "+WorkspaceName+" -t -1 --robustFit=1 --X-rtd MINIMIZER_analytic --algo=singles --cl=0.68 -n "+DateTag+"_"+args.MeasurementType+" --setParameters "

    for parameterName in parametersToMeasure:
        MeasurementCommand+=parameterName+"=1,"
    MeasurementCommand+= " -P "+parameter+" --floatOtherPOIs=1"

    logging.info("Measurement Command:")
    logging.info("\n\n"+MeasurementCommand+"\n")
    os.system(MeasurementCommand+" | tee -a "+outputLoggingFile)
    os.system(" mv *"+DateTag+"* "+OutputDir)

if args.MakePlots:
    plotMakingCommand = "combineTool.py -M FitDiagnostics "+WorkspaceName+" -t -1 --robustFit=1 --X-rtd MINIMIZER_analytic --cl=0.68 --saveShapes --plots --expectSignal=1 -n "+DateTag+"_Plots --setParameters "
    
    for parameterName in parametersToMeasure:
        plotMakingCommand+=parameterName+"=1,"
    logging.info("General plot making command:")
    logging.info('\n\n'+plotMakingCommand+'\n')
    os.system(plotMakingCommand+" | tee -a "+outputLoggingFile)
    os.system(" mv *"+DateTag+"* "+OutputDir)

outputArea.PrintSessionInfo(DateTag)

