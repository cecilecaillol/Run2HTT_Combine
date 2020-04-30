#!/usr/bin/env python
import sys
import re

limits = open(sys.argv[1], "r")
#limits = open("testsorting2017.txt", "r")

UncertaintiesDic = {}
GapDic = {}
FitDic = {}
dashLine = '---------------------------------------------------------------------------------------------------'

# Store POI and expected uncertainties
for line in limits:
  resultLine = re.search('r.*:\s+[+\-][0-9]+\.[0-9]+\s+-[0-9]+\.[0-9]+/\+[0-9]+\.[0-9]+',line)  
  if not resultLine:
    continue
  #print resultLine.group(0)
  POI = re.search('r.*(?=\s+:)',line).group(0)
  #print(POI)
  results = re.findall('[+\-][0-9]+\.[0-9]+',line)
  #print results[0]
  #print results[1]
  #print results[2]
  #indexSpace = line.find(" :")
  #POI = line[3:indexSpace]  
  #indexBeginUnc = line.find("-")
  #indexEndUnc = line.find(" (")
  #Uncertainty = line[indexBeginUnc:indexEndUnc]
  #indexSlash = Uncertainty.find("/")
  #LowerLimit = Uncertainty[0:indexSlash]
  LowerLimit = results[1]
  #UpperLimit = Uncertainty[indexSlash+1:-1]
  UpperLimit = results[2]
  #UncertaintiesDic[POI] = Uncertainty
  UncertaintiesDic[POI] = results[1]+'/'+results[2]
  GapDic[POI] = float(UpperLimit)-float(LowerLimit)  
  FitDic[POI] = results[0]
#print UncertaintiesDic

# Bins
STXSBin_stage0 = ['r_qqH','r_ggH','r_WH','r_ZH']
singleSTXSBin_ggH = [
  "r_ggH_FWDH_htt125",
  'r_ggH_PTH_0_200_0J_PTH_0_10_htt125',# 0J
  'r_ggH_PTH_0_200_0J_PTH_10_200_htt125', 
  'r_ggH_PTH_0_200_1J_PTH_0_60_htt125',
  'r_ggH_PTH_0_200_1J_PTH_60_120_htt125', # 1J  
  'r_ggH_PTH_0_200_1J_PTH_120_200_htt125',
  'r_ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125',
  'r_ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_60_120_htt125', # GE2J_MJJ_0_350  
  'r_ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_120_200_htt125',
  'r_ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125', #GE2J_MJJ_350_700
  'r_ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_GE25_htt125',
  'r_ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_0_25_htt125', # GE2J_MJJ_GE700
  'r_ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125',
  #'r_ggH_PTH_GE200_htt125', # PTH_GE200  
  "r_ggH_PTH_200_300_htt125",
  "r_ggH_PTH_300_450_htt125",
  "r_ggH_PTH_450_650_htt125",
  "r_ggH_PTH_GE650_htt125"
]
mergedSTXSBin_ggH = [
  'r_ggH_PTH_0_200_GE2J_MJJ_GE350',
  'r_ggH_PTH_GE300'
]
singleSTXSBin_qqH = [
  "r_qqH_FWDH_htt125",
  'r_qqH_0J_htt125', # 0J
  'r_qqH_1J_htt125', # 1J
  'r_qqH_GE2J_MJJ_0_60_htt125', # GE2J_MJJ_0_350
  'r_qqH_GE2J_MJJ_60_120_htt125',
  'r_qqH_GE2J_MJJ_120_350_htt125',
  'r_qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125', # GE2J_MJJ_350_700_PTH_0_200
  'r_qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125',
  'r_qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125', # GE2J_MJJ_GE700_PTH_0_200
  'r_qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125',
  'r_qqH_GE2J_MJJ_GE350_PTH_GE200_htt125',  
]
mergedSTXSBin_qqH = [
  'r_qqH_LT2J',
  'r_qqH_GE2J_MJJ_0_350',
  'r_qqH_GE2J_MJJ_350_700_PTH_0_200',
  'r_qqH_GE2J_MJJ_GE700_PTH_0_200'
]
colorCode = ''

def gapComputer(POI, threshold):
  gap = " ("+str(GapDic[POI])+")"
  if GapDic[POI] < 0:
    gap = ""
  elif GapDic[POI] < threshold:
    gap = "\33[33m"+gap+"\33[0m"
  return gap 

# in case there is no computed limit
allDics = [STXSBin_stage0, singleSTXSBin_ggH, singleSTXSBin_qqH, mergedSTXSBin_ggH, mergedSTXSBin_qqH]
for dic in allDics:
  for missingPOI in dic:
    if missingPOI not in UncertaintiesDic.keys():
      UncertaintiesDic[missingPOI] = "N/A"
      FitDic[missingPOI] = 'N/A'
      GapDic[missingPOI] = -1

print ""
print "\t\33[1;30;47m<<  STXS Stage0  >>\33[0m"
print ""
for poi in UncertaintiesDic.keys():
  if poi == 'r':
    gap = gapComputer(poi,0)
    colorCode = '\33[93m'
    print (colorCode + poi + '\33[0m \t:\t'+FitDic[poi]+'\t'+ UncertaintiesDic[poi] + gap)

# Stage 0
for poi in STXSBin_stage0:
  colorCode = '\33[96m'
  gap = gapComputer(poi,0)
  print (colorCode + poi + '\33[0m \t:\t'+FitDic[poi]+'\t'+ UncertaintiesDic[poi] + gap)
print ""
# Stage 1.2 ggH 
print ""
print "\t\t\t\t\t\33[1;30;47m<< STXS Stage1.2 >>\33[0m"
print ""
for poi in singleSTXSBin_ggH:
  colorCode = '\33[34m'
  if poi == 'r_ggH_PTH_0_200_0J_PTH_0_10_htt125' or poi == 'r_ggH_PTH_0_200_1J_PTH_0_60_htt125' or poi == 'r_ggH_PTH_0_200_GE2J_MJJ_0_350_PTH_0_60_htt125' or poi == 'r_ggH_PTH_0_200_GE2J_MJJ_350_700_PTHJJ_0_25_htt125' or poi == "r_ggH_PTH_200_300_htt125" or poi == "r_ggH_FWDH_htt125":
    print dashLine
  gap = gapComputer(poi,30)
  print (colorCode + poi + '\33[0m \t:\t'+FitDic[poi]+'\t' + UncertaintiesDic[poi] + gap)

  # merged bins
  mergedPOI = ""
  if poi == 'r_ggH_PTH_0_200_GE2J_MJJ_GE700_PTHJJ_GE25_htt125':
    mergedPOI = 'r_ggH_PTH_0_200_GE2J_MJJ_GE350'
  if poi == "r_ggH_PTH_GE650_htt125":
    mergedPOI = 'r_ggH_PTH_GE300'
  if mergedPOI != "":
    colorCode = '\33[1;37;44m'
    print (colorCode + mergedPOI + '\33[0m \t:\t'+FitDic[mergedPOI]+'\t' + UncertaintiesDic[mergedPOI] + gap)

# Stage 1.2 qqH 
for poi in singleSTXSBin_qqH:
  colorCode = '\33[91m'
  if poi == 'r_qqH_0J_htt125' or poi == 'r_qqH_GE2J_MJJ_0_60_htt125' or poi == 'r_qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_0_25_htt125' or poi == 'r_qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_0_25_htt125' or poi == 'r_qqH_GE2J_MJJ_GE350_PTH_GE200_htt125' or poi == "r_qqH_FWDH_htt125":
    print dashLine
  gap = gapComputer(poi,30)
  print (colorCode + poi + '\33[0m \t:\t'+FitDic[poi]+'\t' + UncertaintiesDic[poi] + gap)

  # merged bins
  mergedPOI = ""
  if poi == 'r_qqH_GE2J_MJJ_120_350_htt125':
    mergedPOI = 'r_qqH_GE2J_MJJ_0_350'
  if poi == 'r_qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_350_700_PTHJJ_GE25_htt125':
    mergedPOI = 'r_qqH_GE2J_MJJ_350_700_PTH_0_200'
  if poi == 'r_qqH_GE2J_MJJ_GE350_PTH_0_200_MJJ_GE700_PTHJJ_GE25_htt125':
    mergedPOI = 'r_qqH_GE2J_MJJ_GE700_PTH_0_200'
  if poi == 'r_qqH_1J_htt125':
    mergedPOI = 'r_qqH_LT2J'
  if mergedPOI!= "":
    colorCode = '\33[1;37;41m'
    gap = gapComputer(mergedPOI,5)
    print (colorCode + mergedPOI + '\33[0m \t:\t'+FitDic[mergedPOI]+'\t' + UncertaintiesDic[mergedPOI] + gap)

print dashLine
print ""


