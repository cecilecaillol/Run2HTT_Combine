import os,sys
import array
import numpy as np
import argparse
import ROOT
import uproot
from ROOT import TFile
from ROOT import gROOT
gROOT.SetBatch(ROOT.kTRUE)

parser = argparse.ArgumentParser(description = "DeltaScan ")
parser.add_argument('--MeasurementType',nargs = '?', choices=['mjj','pth','njets','ljpt'],help="Specify the kind of differential measurement to make",required=True)
parser.add_argument('--Tag',nargs = 1,help="Output tag: HTT_Output/Output_?",required=True)
args = parser.parse_args()

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
parametersTagDic = {
    'pth':'Higgs p_{T} [GeV]',
    'njets':'Jet multiplicity',
    'ljpt':'Leading jet p_{T} [GeV]',
}

uncSize = {
    'pth':6.0,
    'njets':0.15,
    'ljpt':6.0
}

def GetMu(path, par): #, cents, downs, ups):
    # Open file
    file = TFile(path,'READ')
    tree = file.Get("limit")
    # Get Mu values and uncertainties
    mu = []
    tree.GetEntry(0)
    mu.append(getattr(tree,par))
    tree.GetEntry(1)
    mu.append(-mu[0] + getattr(tree,par))
    tree.GetEntry(2)
    mu.append(-mu[0] + getattr(tree,par))
    cent, down, up = mu[0], mu[1], mu[2]
    print "%.2f\t %.2f/+%.2f"%(cent, down, up)

    return cent, abs(down), up



def plot_outofrange(gr, col, sty):
    lines = []
    for ip in range(gr.GetN()):
        if gr.GetY()[ip] > ymax:
            line = ROOT.TLine(gr.GetX()[ip], gr.GetY()[ip] - gr.GetErrorYlow(ip), gr.GetX()[ip], ymax)
            line.SetLineColor(col)
            line.SetLineStyle(sty)
            line.SetLineWidth(2)
            line.Draw()
            lines.append(line)

print "Start Mu plot making!"
inputFolder="HTT_Output/Output_%s/"%(args.Tag[0])
unreg_path = inputFolder+"higgsCombine%s_%s_idx.MultiDimFit.mH120.root"%(args.Tag[0],args.MeasurementType)
reg_path = unreg_path.replace("_idx","_reg_idx")
parametersToMeasure = parametersToMeasureDic[args.MeasurementType]
npoi = len(parametersToMeasure)
graph_unreg = ROOT.TGraphAsymmErrors(npoi)
graph_reg = ROOT.TGraphAsymmErrors(npoi)


unreg_cents, unreg_downs, unreg_ups = np.array([]), np.array([]), np.array([])
reg_cents, reg_downs, reg_ups = np.array([]), np.array([]), np.array([])


for par in parametersToMeasure:
    print "\n\n"+par
    x = float(par[par.rfind("_")+1:]) if "G" not in par else float(par[par.rfind("_")+3:])*1.25 
    # Unreg
    unreg_path_idx = unreg_path.replace("idx",str(parametersToMeasure.index(par)))
    unreg_cent, unreg_down, unreg_up = GetMu(unreg_path_idx, par)
    unreg_cents, unreg_downs, unnreg_ups = np.append(unreg_cents, [unreg_cent]), np.append(unreg_downs, [unreg_down]), np.append(unreg_ups, [unreg_up])
    graph_unreg.SetPoint(parametersToMeasure.index(par), x ,unreg_cent)
    graph_unreg.SetPointError(parametersToMeasure.index(par), uncSize[args.MeasurementType], uncSize[args.MeasurementType], unreg_down , unreg_up)
    # Reg
    reg_path_idx = reg_path.replace("idx",str(parametersToMeasure.index(par)))
    reg_cent, reg_down, reg_up = GetMu(reg_path_idx, par)
    reg_cents, reg_downs, reg_ups = np.append(reg_cents, [reg_cent]), np.append(reg_downs, [reg_down]), np.append(reg_ups, [reg_up])
    graph_reg.SetPoint(parametersToMeasure.index(par), x ,reg_cent)
    graph_reg.SetPointError(parametersToMeasure.index(par), 0, 0, reg_down ,reg_up)

    print "improvement: %.2f"%((((unreg_up+unreg_down)/(reg_up+reg_down))-1)*100.0)

######################
#    Build canvas    #    
######################

canvas = ROOT.TCanvas('c1', 'c1', 1000, 1000)
canvas.SetRightMargin(0.05)
canvas.SetLeftMargin(0.15)
canvas.SetGrid(False, True)

ymin, ymax = -4., 6.

legend = ROOT.TLegend(0.7, 0.8, 0.9, 0.9)
legend.SetBorderSize(0)
legend.SetFillStyle(0)

graph_unreg.SetLineWidth(0)
graph_unreg.SetMarkerColor(ROOT.kOrange+10)
graph_unreg.SetMarkerStyle(8)
graph_unreg.SetMarkerSize(2)
graph_unreg.SetFillColor(ROOT.kOrange)
graph_unreg.SetFillStyle(3001)
graph_unreg.Draw('AP2')

graph_unreg.SetTitle('Signal strengths')
graph_unreg.GetYaxis().SetRangeUser(ymin, ymax)
graph_unreg.GetYaxis().SetTitle('#mu')
graph_unreg.GetXaxis().SetTitle(parametersTagDic[args.MeasurementType])
#graph_unreg.GetXaxis().SetLimits(binning[0], binning[-1])
plot_outofrange(graph_unreg, ROOT.kBlue, ROOT.kDashed)

graph_reg.SetLineColor(ROOT.kBlack)
graph_reg.SetLineWidth(2)
graph_reg.SetMarkerColor(ROOT.kBlack)
graph_reg.SetMarkerStyle(8)
graph_reg.SetMarkerSize(0.8)
graph_reg.Draw('P')
plot_outofrange(graph_reg, ROOT.kBlack, ROOT.kSolid)

legend.AddEntry(graph_reg, 'Regularized', 'PL')
legend.AddEntry(graph_unreg, 'Unregularized', 'PF')
legend.Draw()

print "\n\n"    
canvas.Print(inputFolder+"mu.pdf")
canvas.Print(inputFolder+"mu.png")
