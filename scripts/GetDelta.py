#!/usr/bin/env python
import ROOT
import re, os, sys
import argparse

parser = argparse.ArgumentParser(description = "DeltaScan for differential XS regularization")
parser.add_argument('--MeasurementType',nargs = '?', choices=['mjj','pth','njets','ljpt'],help="Specify the kind of differential measurement to make",required=True)
parser.add_argument('--Tag',nargs = 1,help="Output tag: HTT_Output/Output_?",required=True)
parser.add_argument('--CreateWorkspace',help="Run text2workspace",action="store_true")
args = parser.parse_args()

delta, step, repeat = 0.1, 1.0, 15

def runCmd(cmd):
    print cmd
    os.system(cmd)

def cdDir(dir):
    print "Where are we???\n"+os.getcwd()+"\n\n"
    print "cd "+dir
    if os.path.isdir(dir) is False:
        print "There is no directory - "+ dir
        if args.printTrue is False: 
            sys.exit(0)
    else: 
        os.chdir(dir)
    print "Where are we???\n"+os.getcwd()+"\n\n"


parametersToMeasureDic = {
    'pth':[
        'r_H_PTH_0_45',
        'r_H_PTH_45_80',
        'r_H_PTH_80_120',
        'r_H_PTH_120_200',
        'r_H_PTH_200_350',
        'r_H_PTH_350_450',
        'r_H_PTH_GT450'
        ],
    'njets':[
        'r_H_NJETS_0',
        'r_H_NJETS_1',
        'r_H_NJETS_2',
        'r_H_NJETS_3',
        'r_H_NJETS_GE4'
    ],
    'ljpt':[
        'r_H_LJPT_30_60',        
        'r_H_LJPT_60_120',
        'r_H_LJPT_120_200',
        'r_H_LJPT_200_350',
        'r_H_LJPT_GT350',
    ]  
}


print "Start Delta Scan!"
inputFolder="HTT_Output/Output_%s"%(args.Tag[0])
print "Measurement type : %s,\t Tag : %s\n\n"%(args.MeasurementType, inputFolder)
parametersToMeasure = parametersToMeasureDic[args.MeasurementType]
# Check if input folder exist
if not(os.path.isdir(inputFolder)):
    print "There is no "+inputFolder
    sys.exit(0)

# Copy txt datacards
UnregTxtDatacard = "%s/FinalCard_%s.txt"%(inputFolder, args.Tag[0])  
RegTxtDatacard = UnregTxtDatacard.replace(".txt","_Reg.txt")
if not(os.path.isfile(UnregTxtDatacard)):
    print "There is no "+UnregTxtDatacard
    sys.exit(0)

UnregTxtDatacardFile = open(UnregTxtDatacard, "r")
RegTxtDatacardFile = open(RegTxtDatacard, "w")
while True:
    line = UnregTxtDatacardFile.readline()
    if not line: break
    if "imax" in line: newline = "imax * number of bins\n"
    elif "jmax" in line: newline = "jmax * number of processes minus 1\n"
    elif "kmax" in line: newline = "kmax * number of nuisance parameters\n"
    else: newline = line
    if "autoMCStats" not in newline: RegTxtDatacardFile.write(newline)


# Add lines for 
print "\n\nBelow lines are added for regularization"
RegTxtDatacardFile.write("## Below lines are added for regularization")

for idx in range(1,len(parametersToMeasure)):    
    i0, i1, i2 = idx-1, idx, idx+1
    if idx==len(parametersToMeasure)-1: i2 = 0

    RegLines = "\nconstr%.0f constr %s+%s-2*%s delta[4.6]"%(idx, parametersToMeasure[i0],parametersToMeasure[i2],parametersToMeasure[i1])
    print RegLines
    RegTxtDatacardFile.write(RegLines)
RegTxtDatacardFile.close()


# Make new workspace
WorkspaceCommand = "text2workspace.py -P HiggsAnalysis.CombinedLimit.PhysicsModel:multiSignalModel "
if args.MeasurementType == "pth":
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_0_45.*htt.*:r_H_PTH_0_45[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_45_80.*htt.*:r_H_PTH_45_80[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_80_120.*htt.*:r_H_PTH_80_120[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_120_200.*htt.*:r_H_PTH_120_200[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_200_350.*htt.*:r_H_PTH_200_350[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_350_450.*htt.*:r_H_PTH_350_450[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*PTH_G.450.*htt.*:r_H_PTH_GT450[1,-25,25]' "
elif args.MeasurementType == 'njets':
    WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_0.*htt.*:r_H_NJETS_0[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_1.*htt.*:r_H_NJETS_1[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_2.*htt.*:r_H_NJETS_2[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_3.*htt.*:r_H_NJETS_3[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*NJETS_GE4.*htt.*:r_H_NJETS_GE4[1,-25,25]' "
elif args.MeasurementType == 'ljpt':
    WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_30_60.*htt.*:r_H_LJPT_30_60[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_60_120.*htt.*:r_H_LJPT_60_120[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_120_200.*htt.*:r_H_LJPT_120_200[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_200_350.*htt.*:r_H_LJPT_200_350[1,-25,25]' "
    WorkspaceCommand += "--PO 'map=.*/.*H.*LJPT_G.350.*htt.*:r_H_LJPT_GT350[1,-25,25]' "
WorkspaceCommand+= RegTxtDatacard+" -o "+RegTxtDatacard.replace("txt","root")+" -m 125"
if args.CreateWorkspace is True : runCmd(WorkspaceCommand)


# Scan
if (os.path.isdir(inputFolder+"/deltaScan")):
    os.system("rm -r "+inputFolder+"/deltaScan")
os.makedirs(os.path.join(inputFolder+"/deltaScan"))
os.makedirs(os.path.join(inputFolder+"/deltaScan/multidimfit"))
os.makedirs(os.path.join(inputFolder+"/deltaScan/higgsCombine"))
os.makedirs(os.path.join(inputFolder+"/deltaScan/matrix"))
    
InitialDir=os.getcwd()
cdDir(inputFolder)
haddFileList="hadd -f deltaScan/higgsCombine.root "

for i in range(1, repeat):
    print "delta = "+str(delta)
    fitOutputfile = "deltaScan/higgsCombine/higgsCombineTest.MultiDimFit.mH120."+str(i)+".root "
    cmd = "combine -M MultiDimFit "+"FinalCard_%s_Reg.root"%(args.Tag[0])+" -t -1 --setParameters delta=%f --saveFitResult \n"%(delta)
    cmd += "mv multidimfit.root deltaScan/multidimfit/multidimfit_"+str(i)+".root\n"
    cmd += "mv higgsCombineTest.MultiDimFit.mH120.root "+fitOutputfile+"\n" 
    cmd += "python "+InitialDir+"/scripts/compute_gcc.py deltaScan/multidimfit/multidimfit_"+str(i)+".root "+fitOutputfile+str(delta)+" "+args.MeasurementType+"\n\n\n"
    cmd += "mv test.png deltaScan/matrix/mat_"+str(i)+".png"
    runCmd(cmd)
    delta += step
    haddFileList += fitOutputfile



cmd = haddFileList+"\n"
cmd += "python "+InitialDir+"/scripts/plot_delta_scan.py deltaScan/higgsCombine.root deltaScan/globalCorrelationScan.png\n\n"
runCmd(cmd)


print "\n\n"+inputFolder+"/deltaScan/globalCorrelationScan.png is created!\n\n"
