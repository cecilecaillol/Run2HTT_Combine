#class designed to datacards, group uncertainties, 
#and then run fits with the various groups frozen
import ROOT
import re
import os
import math

#TODO, extend the number and type of groups measured
class UncertaintySplitter():
    def __init__(self):        
        pass
    def FindAndTagGroups(self,DataCard):
        Systematics = []
        CardFile = open(DataCard,"a+")
        CardContents = CardFile.readlines()
        #find all our systematics
        for line in CardContents:
            if re.search("lnN|shape(?!s)",line):
                #the line contains a systematics, get it's name
                Systematics.append(re.match("^(\S)+",line).group(0))
        #create the syst group
        SystGroup = "Syst group = "
        for Systematic in Systematics:
            SystGroup += Systematic+" "
        SystGroup+="\n"
        CardFile.write(SystGroup)       

        #let's find theory systematics and perform a similar analysis on them.
        theorySysts = []
        for line in CardContents:
            if re.search("BR_htt|(?<!CMS_)(?<!boson_)[sS]cale|pdf_Higgs",line):
                #the line contains one of our theory uncertainties. Get the name
                theorySysts.append(re.match("^\S+",line).group(0))
        #create the theory group
        theoryGroup = "Theory group = "
        for systematic in theorySysts:
            theoryGroup += systematic+" "
        theoryGroup+='\n'
        CardFile.write(theoryGroup)
        
        CardFile.close()
    #create a special function dedicated to creating condor jobs for measurements
    def CreateGridMeasurement(self,parameter,OutputDir,workspace,tag,npoints,logging):
        print("Creating specialized condor tasks for making in depth uncertainty sweeps...")
        allUncertsCommand = 'combineTool.py -M MultiDimFit -d '+workspace+' --robustFit 1 --X-rtd MINIMIZER_Analytic --algo grid --points '+str(npoints)+' --splitPoints 1 -n '+parameter+'AllUncerts_'+tag+' -P '+parameter+' --floatOtherPOIs=1 --job-mode condor --task-name '+parameter+'AllUncerts_'+tag+' --merge 1'
        theoryFreezeCommand = 'combineTool.py -M MultiDimFit -d '+workspace+' --robustFit 1 --X-rtd MINIMIZER_Analytic --algo grid --points '+str(npoints)+' --splitPoints 1 -n '+parameter+'TheoryFreeze_'+tag+' -P '+parameter+' --floatOtherPOIs=1 --job-mode condor --task-name '+parameter+'TheoryFreeze_'+tag+' --merge 1 --freezeNuisanceGroups Theory'
        theoryAndBBBFreezeCommand = 'combineTool.py -M MultiDimFit -d '+workspace+' --robustFit 1 --X-rtd MINIMIZER_Analytic --algo grid --points '+str(npoints)+' --splitPoints 1 -n '+parameter+'TheoryAndBBBFreeze_'+tag+' -P '+parameter+' --floatOtherPOIs=1 --job-mode condor --task-name '+parameter+'TheoryAndBBFreeze_'+tag+' --merge 1 --freezeNuisanceGroups Theory,autoMCStats'
        allFreezeCommand = 'combineTool.py -M MultiDimFit -d '+workspace+' --robustFit 1 --X-rtd MINIMIZER_Analytic --algo grid --points '+str(npoints)+' --splitPoints 1 -n '+parameter+'AllFreeze_'+tag+' -P '+parameter+' --floatOtherPOIs=1 --job-mode condor --task-name '+parameter+'AllFreeze_'+tag+' --merge 1 --freezeNuisanceGroups Syst,autoMCStats'
        logging.info('Condor Based Grid Sweep Commands: ')
        logging.info('\n\n'+allUncertsCommand+'\n\n'+theoryFreezeCommand+'\n\n'+theoryAndBBBFreezeCommand+'\n\n'+allFreezeCommand+'\n')
        os.system(allUncertsCommand)
        os.system(theoryFreezeCommand)
        os.system(theoryAndBBBFreezeCommand)
        os.system(allFreezeCommand)

    #this is old and incorrect? Maybe needs redoing. For now, avoid use.
    def SplitMeasurement(self,Command,OutputDir):
        print("Splitting the Uncertainty...")        
        #secondary method? should evaluate the same?
        #print("Method Four:")
        #rerun the measurement
        #this fit can be VERY touchy. It doesn't like to run stats only. 
        #this seems to be an issue with the robust fittting, because this command runs:
        #StatOnlyCommand = Command.replace("--robustFit=1","")
        #os.system(StatOnlyCommand+" --freezeNuisanceGroups Syst,autoMCStats -v 3 &>"+OutputDir+"SplitOutput.txt")       
        #let's see if we can ease the robusut fit tolerance out a bit to allow for proper fitting.
        SplitCommand = Command.replace("--robustFit=1","")
        os.system(SplitCommand+" --freezeNuisanceGroups Syst,autoMCStats -v 3 &>"+OutputDir+"SplitOutput.txt")
        #then freeze the systs
        os.system(SplitCommand+" --freezeNuisanceGroups autoMCStats &>> "+OutputDir+"SplitOutput.txt")
        #then freeze the auto mc stats
        os.system(SplitCommand+" --freezeNuisanceGroups Syst &>> "+OutputDir+"SplitOutput.txt")
        
        SplitFile = open(OutputDir+"SplitOutput.txt","r")
        SplitFileContents = SplitFile.readlines()
        Sigma_Up=[]
        Sigma_Down=[]
        for line in SplitFileContents:            
            FoundParamter = re.search("^\s+r.*:\s*",line)
            if FoundParamter: # identify a line with a measurement                
                Parameter= FoundParamter.group(0)
                #print("Match!")
                #print(line)
                Sigma_Down.append(float(re.search("(?<=-)[0-9]+\.[0-9]*",line).group(0)))
                Sigma_Up.append(float(re.search("(?<=/\+)[0-9]+\.[0-9]*",line).group(0)))                
        Stat_Var_Up = Sigma_Up[0]*Sigma_Up[0]
        Syst_Var_Up = Sigma_Up[1]*Sigma_Up[1]-Stat_Var_Up        
        BBB_Var_Up = Sigma_Up[2]*Sigma_Up[2]-Stat_Var_Up        

        Stat_Var_Down = Sigma_Down[0]*Sigma_Down[0]
        Syst_Var_Down = Sigma_Down[1]*Sigma_Down[1]-Stat_Var_Down        
        BBB_Var_Down = Sigma_Down[2]*Sigma_Down[2]-Stat_Var_Down        
        
        if Stat_Var_Up < 0:
            print("+Stat variance < 0. Setting it to 0")
            Stat_Var_Up = 0
        if Syst_Var_Up < 0:
            print("+Syst variance < 0. Setting it to 0")
            Syst_Var_Up = 0
        if BBB_Var_Up < 0:
            print("+BBB variance < 0. Setting it to 0")
            BBB_Var_Up = 0
        if Stat_Var_Down < 0:
            print("-Stat variance < 0. Setting it to 0")
            Stat_Var_Down = 0
        if Syst_Var_Down < 0:
            print("-Syst variance < 0. Setting it to 0")
            Syst_Var_Down = 0
        if BBB_Var_Down < 0:
            print("-BBB variance < 0. Setting it to 0")
            BBB_Var_Down = 0
        
        BBBContribution_Up = math.sqrt(BBB_Var_Up)
        SystContribution_Up = math.sqrt(Syst_Var_Up)
        StatContribution_Up = math.sqrt(Stat_Var_Up)        
        BBBContribution_Down = math.sqrt(BBB_Var_Down)
        SystContribution_Down = math.sqrt(Syst_Var_Down)
        StatContribution_Down = math.sqrt(Stat_Var_Down)        
        #print("+"+str(StatContribution_Up)+"(stat)+"+str(SystContribution_Up)+"(syst)+"+str(BBBContribution_Up)+"(bin-by-bin)".format())
        #print("-"+str(StatContribution_Down)+"(stat)-"+str(SystContribution_Down)+"(syst)-"+str(BBBContribution_Down)+"(bin-by-bin)")
        print(Parameter+"-%3.3f(stat)-%3.3f(syst)-%3.3f(bin-by-bin)/+%3.3f(stat)+%3.3f(syst)+%3.3f(bin-by-bin)"%(StatContribution_Down,SystContribution_Down,BBBContribution_Down,StatContribution_Up,SystContribution_Up,BBBContribution_Up))
