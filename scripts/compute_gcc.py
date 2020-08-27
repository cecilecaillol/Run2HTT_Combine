"""
Compute the global correlation coefficient of the POIs.
"""
import sys,os
import math
import array
import ROOT
from ROOT import gROOT
gROOT.SetBatch(ROOT.kTRUE)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetPaintTextFormat("4.2f")

source_path, outpath, delta, MeasurementType  = sys.argv[1:5]

source = ROOT.TFile.Open(source_path)
fitres = source.Get('fit_mdf')
if fitres.status() != 0 : 
    print "fit_mdf status is non-zero"
    sys.exit(0)
pars = fitres.floatParsFinal()

pois = ROOT.RooArgList('POIs')
poiNames = []
boundaries={"pth":[0,45,80,120,200,350,450],
            "njets":[0,1,2,3,4],
            "ljpt":[30,60,120,200,350]}
for idx in range(0,len(boundaries[MeasurementType])):
    if MeasurementType == "pth":
        poiName = 'r_H_PTH_%d_%d' % (boundaries[MeasurementType][idx],boundaries[MeasurementType][idx+1]) if idx<len(boundaries[MeasurementType])-1 else 'r_H_PTH_GT450'
    elif MeasurementType == 'njets':
        poiName = 'r_H_NJETS_%d' % (boundaries[MeasurementType][idx]) if idx<len(boundaries[MeasurementType])-1 else 'r_H_NJETS_GE4'
    elif MeasurementType == 'ljpt':
        poiName = 'r_H_LJPT_%d_%d' % (boundaries[MeasurementType][idx],boundaries[MeasurementType][idx+1]) if idx<len(boundaries[MeasurementType])-1 else 'r_H_LJPT_GT350'
    print poiName
    poiNames.append(poiName)
    poi = pars.find(poiName)
    if not poi:
        break
        
    pois.add(poi)

vxx = fitres.reducedCovarianceMatrix(pois)
vxxinv = vxx.Clone()
vxxinv.Invert()

# Save covariance matrices as image
canvas = ROOT.TCanvas('c1', 'c1', 600, 600)
#canvas.SetBottomMargin(0.15)
#canvas.SetLeftMargin(0.15)
vxx.Draw("COLZ TEXT")
title = ROOT.TPaveText(0.3,0.9,0.7,0.99, "NDC")
title.AddText("#delta = "+delta)
title.Draw("same")
canvas.Print("test.png")

outfile = ROOT.TFile.Open(outpath, 'update')
out = ROOT.TTree('gcc', 'global correlation coefficient')
adelta = array.array('d', [float(delta)])
out.Branch('delta', adelta, 'delta/D')
acorrs = []

for ip in range(len(boundaries[MeasurementType])):
    denom = vxxinv[ip][ip] * vxx[ip][ip]
    if denom < 1.:
        # Probably won't happen 
        corr = 0.
        printer.warning("Imaginary gcc "+str(denom))
        os.system("rm "+source_path)
        outfile.Close()
        os.system("rm "+outpath)
        sys.exit(1)
    else:
        corr = math.sqrt(1. - 1. / denom)

    #corr = fitres.globalCorr(poiNames[ip])
    acorr = array.array('d', [corr])
    acorrs.append(acorr)
    out.Branch('c_%d' % ip, acorr, 'c_%d/D' % ip)
    print ip, "\t", poiNames[ip], "\t" ,corr

out.Fill()
outfile.cd()
out.Write()
outfile.Close()
