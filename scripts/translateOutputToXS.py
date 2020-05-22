import argparse
import re

#just get the nominal cross section of parameter in pb
def printLine():
    print '----------------------------------------'
def getNominalParameterCrossSection(parameter):
    crossSection = 0.0
    branchingRatio = 0.0627
    if parameter == 'r':
        #this will just be the addition of all of our considered production mechanisms
        #at the moment this is just whatever goes into ggH and qqH
        # total = 48.58 + 5.677 = 54.257 pb
        crossSection = 54.257
    elif parameter == 'r_ggH':
        #in our case, this is actually ggH samples, plus ggZH hadronic decays
        #ggH has a cross section of 48.58 pb
        #ggZH has a cross section of ???
        crossSection = 48.58
    elif parameter == 'r_qqH':
        #in our case, this is actually vbf samples, plus VH hadronic decays
        #vbf has a cross section of 3.782 pb
        #VH hadronic decays have a cross section of (1.373 - (0.09462 + 0.05983)) + (0.8839 - (0.02982+ 0.1776)) (note: check the subtraction?, based on YR#4)
        # = 1.895 pb
        # total = 5.677 pb
        crossSection = 5.677
    else:
        # we don't have a number for that parameter. Just return 0
        return 0
    return crossSection * branchingRatio

parser = argparse.ArgumentParser(description='Script for taking HTT text output from combine, and translating it into cross section output')
parser.add_argument('outputFile',help='Specify the file to try and read outputFrom')

args = parser.parse_args()

outputFile = open(args.outputFile)
printLine()
for line in outputFile.readlines():    
    #this re matches the general format of a line with a result. Some white space, an r with (possibly) something, some more whitespace
    # and then some formatted numbers with numbers on them
    if re.search('^\s+r(_\S*)*.*[0-9]*\.[0-9]+.*-[0-9]*\.[0-9]+/\+[0-9]*\.[0-9]+',line):
        print line
        parameter = re.search('r(_\S*)*',line).group(0)
        print('parameter: '+parameter)
        signalStrength = float(re.search('[-\+][0-9]+\.[0-9]*(?!/)',line).group(0))
        #print("signal: "+str(signalStrength))
        sigmaDown = float(re.search('-[0-9]+\.[0-9]*(?=/)',line).group(0))
        #print("sigma down: "+str(sigmaDown))
        sigmaUp = float(re.search('(?<=/)\+[0-9]+\.[0-9]*',line).group(0))
        #print("sigma up: "+str(sigmaUp))
        crossSection = getNominalParameterCrossSection(parameter)
        print("Nominal cross section: "+str(crossSection)+" pb")
        print "Cross section: %.3f pb %.3f/+%.3f" % (signalStrength*crossSection,sigmaDown*crossSection,sigmaUp*crossSection)
        printLine()
