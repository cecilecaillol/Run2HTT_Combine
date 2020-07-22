#!/usr/bin/env python
import ROOT
import argparse
import CombineHarvester.Run2HTT_Combine.PlottingModules.globalSettings as globalSettings
import math

def roundToSigDigits(number,sigDigits):    
    return round(number,sigDigits-int(math.floor(math.log10(abs(number))))-1)

parameters_STXS = {
    'r':{
        'SMXS': 3422.279, #higgs XS is 54.257 pb, scale to fb, and use tau BR
        'mu_value':0.865, #+0.865   -0.108/+0.115
        'uncert_down':0.108,
        'uncert_up':0.115,
        'color':ROOT.kBlack,
        'axis':'H#rightarrow#tau#tau',
        'SMXS_mu_up': 0.0503,
        'SMXS_mu_down': 0.0503,        
    },
    'r_ggH':{
        'SMXS': 3051.344, #ggH XS is 48.68 pb, scale to fb and use tau BR (+ggZH)
        'mu_value':1.002, #+1.002   -0.181/+0.194
        'uncert_down':0.181,
        'uncert_up':0.194,
        'color':ROOT.kAzure+7,
        'axis':'ggH#rightarrow#tau#tau',
        'SMXS_mu_up': 0.0523,
        'SMXS_mu_down':0.0523,
        },
    'r_qqH':{
        'SMXS': 328.6833, #total qqH cross section as we consider it
        'mu_value': 0.672, #+0.672   -0.223/+0.229
        'uncert_down': 0.223,
        'uncert_up': 0.229,
        'color':ROOT.kOrange-3,
        'axis':'qqH#rightarrow#tau#tau',
        'SMXS_mu_up': 0.0294,
        'SMXS_mu_down':0.0294,
        },
    'r_ggH_0J':{
        'SMXS': 1752.942,
        'mu_value':-0.55,
        'uncert_down':0.452,
        'uncert_up':0.493,
        'color':ROOT.kAzure+7,
        'axis':'ggH: 0 Jet',
        'SMXS_mu_up': 0.09125,
        'SMXS_mu_down': 0.09125,        
    },
    'r_ggH_1J_PTH_0_60':{
        'SMXS': 451.0862,
        'mu_value':-1.676,
        'uncert_down':1.131,
        'uncert_up':1.150,
        'color':ROOT.kAzure+7,
        'axis':'ggH: 1 Jet, p_{T}^{H}[0,60]',
        'SMXS_mu_up': 0.140,
        'SMXS_mu_down': 0.140,        
    },
    'r_ggH_1J_PTH_60_120':{
        'SMXS': 287.6776,
        'mu_value':3.514,
        'uncert_down':0.909,
        'uncert_up':0.918,
        'color':ROOT.kAzure+7,
        'axis':'ggH: 1 Jet, p_{T}^{H}[60,120]',
        'SMXS_mu_up': 0.142,
        'SMXS_mu_down': 0.142,        
    },
    'r_ggH_1J_PTH_120_200':{
        'SMXS': 50.04276,
        'mu_value':1.981,
        'uncert_down':0.941,
        'uncert_up':1.038,
        'color':ROOT.kAzure+7,
        'axis':'ggH: 1 Jet, p_{T}^{H}[120,200]',
        'SMXS_mu_up': 0.193,
        'SMXS_mu_down': 0.193,        
    },
    'r_ggH_PTH_0_200_GE2J':{
        'SMXS': 306.2587,
        'mu_value':0.062,
        'uncert_down':0.886,
        'uncert_up':0.818,
        'color':ROOT.kAzure+7,
        'axis':'ggH: #geq 2 Jet',
        'SMXS_mu_up': 0.2296,
        'SMXS_mu_down': 0.2296,        
    },
    'r_ggH_PTH_200_300':{
        'SMXS': 27.51224,
        'mu_value':0.878,
        'uncert_down':0.892,
        'uncert_up':0.885,
        'color':ROOT.kAzure+7,
        'axis':'ggH: p_{T}^{H}[200,300]',
        'SMXS_mu_up': 0.41885,
        'SMXS_mu_down': 0.41885,        
    },
    'r_ggH_PTH_GE300':{
        'SMXS': 7.188811,
        'mu_value':1.803,
        'uncert_down':1.099,
        'uncert_up':1.088,
        'color':ROOT.kAzure+7,
        'axis':'ggH: p_{T}^{H} > 300',
        'SMXS_mu_up': 0.467,
        'SMXS_mu_down': 0.467,        
    },
    'r_qqH_NONVBFTOPO':{
        'SMXS': 209.42,
        'mu_value':1.787,
        'uncert_down':2.460,
        'uncert_up':2.636,
        'color':ROOT.kOrange-3,
        'axis':'qqH: Non-VBF Topology',
        'SMXS_mu_up': 0.02919,
        'SMXS_mu_down': 0.02919,        
    },
    'r_qqH_GE2J_MJJ_350_700_PTH_0_200':{
        'SMXS': 34.430,
        'mu_value':-0.55,
        'uncert_down':1.336,
        'uncert_up':1.331,
        'color':ROOT.kOrange-3,
        'axis':'qqH: #geq  2 Jets, m_{jj}[350,700]',
        'SMXS_mu_up': 0.03639,
        'SMXS_mu_down': 0.03639,        
    },
    'r_qqH_GE2J_MJJ_GE700_PTH_0_200':{
        'SMXS': 47.481,
        'mu_value':0.693,
        'uncert_down':0.364,
        'uncert_up':0.372,
        'color':ROOT.kOrange-3,
        'axis':'qqH: #geq 2 Jets, m_{jj} > 700',
        'SMXS_mu_up': 0.0379,
        'SMXS_mu_down': 0.0379,        
    },    
    'r_qqH_BSM':{
        'SMXS': 9.8996,
        'mu_value':0.648,
        'uncert_down':0.421,
        'uncert_up':0.444,
        'color':ROOT.kOrange-3,
        'axis':'qqH: p_{T}^{H} > 200',
        'SMXS_mu_up': 0.03426,
        'SMXS_mu_down': 0.03426,        
    },    
}

parameterOrder = ['r','r_ggH','r_qqH',
                  'r_ggH_0J',
                  'r_ggH_1J_PTH_0_60',
                  'r_ggH_1J_PTH_60_120',
                  'r_ggH_1J_PTH_120_200',
                  'r_ggH_PTH_0_200_GE2J',
                  'r_ggH_PTH_200_300',
                  'r_ggH_PTH_GE300',
                  'r_qqH_NONVBFTOPO',
                  'r_qqH_GE2J_MJJ_350_700_PTH_0_200',
                  'r_qqH_GE2J_MJJ_GE700_PTH_0_200', 
                  'r_qqH_BSM',]
latexAlignments = [10000,10000,900,
                   5000,
                   1300,
                   2000,
                   300,
                   850,
                   100,
                   40,
                   1800,
                   100,
                   150,
                   25,]

ggHParameters = [
    'r_ggH_0J',
    'r_ggH_1J_PTH_0_60',
    'r_ggH_1J_PTH_60_120',
    'r_ggH_1J_PTH_120_200',
    'r_ggH_PTH_0_200_GE2J',
    'r_ggH_PTH_200_300',
    'r_ggH_PTH_GE300',
    ]
qqHParameters= [
    'r_qqH_NONVBFTOPO',
    'r_qqH_GE2J_MJJ_350_700_PTH_0_200',
    'r_qqH_GE2J_MJJ_GE700_PTH_0_200', 
    'r_qqH_BSM',
]

#parser = argparse.ArgumentParser(description = 'Create a fancy cross section plot table ')
#args = parser.parse_args()

globalSettings.style.setPASStyle()
ROOT.gROOT.SetStyle('pasStyle')

#okay, let's start by creating the canvas
theCanvas = ROOT.TCanvas("crossSectionCanvas","crossSectionCanvas")
theCanvas.SetCanvasSize(950,600)
theCanvas.Divide(1,2)
plotPad = theCanvas.GetPrimitive(theCanvas.GetName()+'_1')
ratioPad = theCanvas.GetPrimitive(theCanvas.GetName()+'_2')
plotPad.SetPad(0.0,0.4,1.,1.)
ratioPad.SetPad(0.0,0.0,1.0,0.475)
plotPad.SetTopMargin(1)
plotPad.SetLeftMargin(1)


plotPad.SetLogy()
plotPad.SetLogy()
plotPad.SetTickx()
plotPad.SetTicky()
plotPad.SetGridx()

plotGridHisto = ROOT.TH1F("XSGraphFrame","XSGraphFrame",len(parameters_STXS),0.0,float(len(parameters_STXS)))
plotGridHisto.GetXaxis().SetNdivisions(len(parameters_STXS),0,0)
plotGridHisto.GetXaxis().SetTickLength(0)
#plotGridHisto.GetYaxis().SetTickLength(0)

ratioPad.SetTickx()
ratioPad.SetTicky()
ratioPad.SetGridy()
ratioPad.SetGridx()
ratioPad.SetTopMargin(0.04)
ratioPad.SetBottomMargin(0.5)
ratioPad.SetLeftMargin(1)

ratioGridHisto = ROOT.TH1F("RatioGraphFrame","RatioGraphFrame",len(parameters_STXS),0.0,float(len(parameters_STXS)))
ratioGridHisto.GetXaxis().SetNdivisions(len(parameters_STXS),0,0)
ratioGridHisto.GetXaxis().SetTickLength(0)


#okay, now, let's create a multi graph that contains the points

inclusiveGraph = ROOT.TGraphAsymmErrors(1)
inclusiveGraph.SetPoint(0,0.5,parameters_STXS['r']['SMXS']*parameters_STXS['r']['mu_value'])
inclusiveGraph.SetPointEYlow(0,parameters_STXS['r']['SMXS']*parameters_STXS['r']['uncert_down'])
inclusiveGraph.SetPointEYhigh(0,parameters_STXS['r']['SMXS']*parameters_STXS['r']['uncert_up'])
inclusiveGraph.SetMarkerColor(parameters_STXS['r']['color'])
inclusiveGraph.SetLineColor(parameters_STXS['r']['color'])
inclusiveGraph.SetMarkerStyle(21)
inclusiveGraph.SetLineWidth(3)

ggHGraph = ROOT.TGraphAsymmErrors(1)
ggHGraph.SetPoint(0,1.5,parameters_STXS['r_ggH']['SMXS']*parameters_STXS['r_ggH']['mu_value'])
ggHGraph.SetPointEYlow(0,parameters_STXS['r_ggH']['SMXS']*parameters_STXS['r_ggH']['uncert_down'])
ggHGraph.SetPointEYhigh(0,parameters_STXS['r_ggH']['SMXS']*parameters_STXS['r_ggH']['uncert_up'])
ggHGraph.SetMarkerColor(parameters_STXS['r_ggH']['color'])
ggHGraph.SetLineColor(parameters_STXS['r_ggH']['color'])
ggHGraph.SetMarkerStyle(21)
ggHGraph.SetLineWidth(3)

qqHGraph = ROOT.TGraphAsymmErrors(1)
qqHGraph.SetPoint(0,2.5,parameters_STXS['r_qqH']['SMXS']*parameters_STXS['r_qqH']['mu_value'])
qqHGraph.SetPointEYlow(0,parameters_STXS['r_qqH']['SMXS']*parameters_STXS['r_qqH']['uncert_down'])
qqHGraph.SetPointEYhigh(0,parameters_STXS['r_qqH']['SMXS']*parameters_STXS['r_qqH']['uncert_up'])
qqHGraph.SetMarkerColor(parameters_STXS['r_qqH']['color'])
qqHGraph.SetLineColor(parameters_STXS['r_qqH']['color'])
qqHGraph.SetMarkerStyle(21)
qqHGraph.SetLineWidth(3)

ggHSTXS = ROOT.TGraphAsymmErrors(len(ggHParameters))
for ggHParameterIndex in range(len(ggHParameters)):
    ggHSTXS.SetPoint(ggHParameterIndex,ggHParameterIndex+3.5,parameters_STXS[ggHParameters[ggHParameterIndex]]['SMXS']*parameters_STXS[ggHParameters[ggHParameterIndex]]['mu_value'])
    ggHSTXS.SetPointEYlow(ggHParameterIndex,parameters_STXS[ggHParameters[ggHParameterIndex]]['SMXS']*parameters_STXS[ggHParameters[ggHParameterIndex]]['uncert_down'])
    ggHSTXS.SetPointEYhigh(ggHParameterIndex,parameters_STXS[ggHParameters[ggHParameterIndex]]['SMXS']*parameters_STXS[ggHParameters[ggHParameterIndex]]['uncert_up'])
    ggHSTXS.SetMarkerColor(parameters_STXS[ggHParameters[ggHParameterIndex]]['color'])
    ggHSTXS.SetLineColor(parameters_STXS[ggHParameters[ggHParameterIndex]]['color'])
ggHSTXS.SetMarkerStyle(21)
ggHSTXS.SetLineWidth(3)

qqHSTXS = ROOT.TGraphAsymmErrors(len(qqHParameters))
for qqHParameterIndex in range(len(qqHParameters)):
    qqHSTXS.SetPoint(qqHParameterIndex,qqHParameterIndex+3.5+len(ggHParameters),parameters_STXS[qqHParameters[qqHParameterIndex]]['SMXS']*parameters_STXS[qqHParameters[qqHParameterIndex]]['mu_value'])
    qqHSTXS.SetPointEYlow(qqHParameterIndex,parameters_STXS[qqHParameters[qqHParameterIndex]]['SMXS']*parameters_STXS[qqHParameters[qqHParameterIndex]]['uncert_down'])
    qqHSTXS.SetPointEYhigh(qqHParameterIndex,parameters_STXS[qqHParameters[qqHParameterIndex]]['SMXS']*parameters_STXS[qqHParameters[qqHParameterIndex]]['uncert_up'])
    qqHSTXS.SetMarkerColor(parameters_STXS[qqHParameters[qqHParameterIndex]]['color'])
    qqHSTXS.SetLineColor(parameters_STXS[qqHParameters[qqHParameterIndex]]['color'])
qqHSTXS.SetMarkerStyle(21)
qqHSTXS.SetLineWidth(3)

overallXSGraph = ROOT.TMultiGraph()
overallXSGraph.Add(inclusiveGraph)
overallXSGraph.Add(ggHGraph)
overallXSGraph.Add(qqHGraph)
overallXSGraph.Add(ggHSTXS)
overallXSGraph.Add(qqHSTXS)

SMXSErrorHisto = ROOT.TGraphAsymmErrors(len(parameterOrder))
for parameterIndex in range(len(parameters_STXS)):
    SMXSErrorHisto.SetPoint(parameterIndex,parameterIndex+0.5,parameters_STXS[parameterOrder[parameterIndex]]['SMXS'])
    SMXSErrorHisto.SetPointEYlow(parameterIndex,parameters_STXS[parameterOrder[parameterIndex]]['SMXS_mu_down']*parameters_STXS[parameterOrder[parameterIndex]]['SMXS'])
    SMXSErrorHisto.SetPointEYhigh(parameterIndex,parameters_STXS[parameterOrder[parameterIndex]]['SMXS_mu_up']*parameters_STXS[parameterOrder[parameterIndex]]['SMXS'])
    SMXSErrorHisto.SetPointEXlow(parameterIndex,0.25)
    SMXSErrorHisto.SetPointEXhigh(parameterIndex,0.25)
SMXSErrorHisto.SetFillStyle(3001)
SMXSErrorHisto.SetFillColor(15)

#let's also create the ratio plot
inclusiveRatioGraph = ROOT.TGraphAsymmErrors(1)
inclusiveRatioGraph.SetPoint(0,0.5,parameters_STXS['r']['mu_value'])
inclusiveRatioGraph.SetPointEYlow(0,parameters_STXS['r']['uncert_down'])
inclusiveRatioGraph.SetPointEYhigh(0,parameters_STXS['r']['uncert_up'])
inclusiveRatioGraph.SetMarkerColor(parameters_STXS['r']['color'])
inclusiveRatioGraph.SetLineColor(parameters_STXS['r']['color'])
inclusiveRatioGraph.SetMarkerStyle(21)
inclusiveRatioGraph.SetLineWidth(3)

ggHRatioGraph = ROOT.TGraphAsymmErrors(1)
ggHRatioGraph.SetPoint(0,1.5,parameters_STXS['r_ggH']['mu_value'])
ggHRatioGraph.SetPointEYlow(0,parameters_STXS['r_ggH']['uncert_down'])
ggHRatioGraph.SetPointEYhigh(0,parameters_STXS['r_ggH']['uncert_up'])
ggHRatioGraph.SetMarkerColor(parameters_STXS['r_ggH']['color'])
ggHRatioGraph.SetLineColor(parameters_STXS['r_ggH']['color'])
ggHRatioGraph.SetMarkerStyle(21)
ggHRatioGraph.SetLineWidth(3)

qqHRatioGraph = ROOT.TGraphAsymmErrors(1)
qqHRatioGraph.SetPoint(0,2.5,parameters_STXS['r_qqH']['mu_value'])
qqHRatioGraph.SetPointEYlow(0,parameters_STXS['r_qqH']['uncert_down'])
qqHRatioGraph.SetPointEYhigh(0,parameters_STXS['r_qqH']['uncert_up'])
qqHRatioGraph.SetMarkerColor(parameters_STXS['r_qqH']['color'])
qqHRatioGraph.SetLineColor(parameters_STXS['r_qqH']['color'])
qqHRatioGraph.SetMarkerStyle(21)
qqHRatioGraph.SetLineWidth(3)

ggHRatioSTXS = ROOT.TGraphAsymmErrors(len(ggHParameters))
for ggHParameterIndex in range(len(ggHParameters)):
    ggHRatioSTXS.SetPoint(ggHParameterIndex,ggHParameterIndex+3.5,parameters_STXS[ggHParameters[ggHParameterIndex]]['mu_value'])
    ggHRatioSTXS.SetPointEYlow(ggHParameterIndex,parameters_STXS[ggHParameters[ggHParameterIndex]]['uncert_down'])
    ggHRatioSTXS.SetPointEYhigh(ggHParameterIndex,parameters_STXS[ggHParameters[ggHParameterIndex]]['uncert_up'])
    ggHRatioSTXS.SetMarkerColor(parameters_STXS[ggHParameters[ggHParameterIndex]]['color'])
    ggHRatioSTXS.SetLineColor(parameters_STXS[ggHParameters[ggHParameterIndex]]['color'])
ggHRatioSTXS.SetMarkerStyle(21)
ggHRatioSTXS.SetLineWidth(3)

qqHRatioSTXS = ROOT.TGraphAsymmErrors(len(qqHParameters))
for qqHParameterIndex in range(len(qqHParameters)):
    qqHRatioSTXS.SetPoint(qqHParameterIndex,qqHParameterIndex+3.5+len(ggHParameters),parameters_STXS[qqHParameters[qqHParameterIndex]]['mu_value'])
    qqHRatioSTXS.SetPointEYlow(qqHParameterIndex,parameters_STXS[qqHParameters[qqHParameterIndex]]['uncert_down'])
    qqHRatioSTXS.SetPointEYhigh(qqHParameterIndex,parameters_STXS[qqHParameters[qqHParameterIndex]]['uncert_up'])
    qqHRatioSTXS.SetMarkerColor(parameters_STXS[qqHParameters[qqHParameterIndex]]['color'])
    qqHRatioSTXS.SetLineColor(parameters_STXS[qqHParameters[qqHParameterIndex]]['color'])
qqHRatioSTXS.SetMarkerStyle(21)
qqHRatioSTXS.SetLineWidth(3)

ratioGraph = ROOT.TMultiGraph()
ratioGraph.Add(inclusiveRatioGraph)
ratioGraph.Add(ggHRatioGraph)
ratioGraph.Add(qqHRatioGraph)
ratioGraph.Add(ggHRatioSTXS)
ratioGraph.Add(qqHRatioSTXS)

SMXSRatioErrorHisto = ROOT.TGraphAsymmErrors(len(parameterOrder))
for parameterIndex in range(len(parameters_STXS)):
    SMXSRatioErrorHisto.SetPoint(parameterIndex,parameterIndex+0.5,1)
    SMXSRatioErrorHisto.SetPointEYlow(parameterIndex,parameters_STXS[parameterOrder[parameterIndex]]['SMXS_mu_down'])
    SMXSRatioErrorHisto.SetPointEYhigh(parameterIndex,parameters_STXS[parameterOrder[parameterIndex]]['SMXS_mu_up'])
    SMXSRatioErrorHisto.SetPointEXlow(parameterIndex,0.25)
    SMXSRatioErrorHisto.SetPointEXhigh(parameterIndex,0.25)
SMXSRatioErrorHisto.SetFillStyle(3001)
SMXSRatioErrorHisto.SetFillColor(15)

for parameterIndex in range(len(parameterOrder)):
    ratioGridHisto.GetXaxis().SetBinLabel(parameterIndex+1,parameters_STXS[parameterOrder[parameterIndex]]['axis'])

#

#draw the overall plot elements
plotPad.cd()
plotGridHisto.Draw()
plotGridHisto.SetMaximum(20000)
plotGridHisto.SetMinimum(5)
plotGridHisto.GetYaxis().SetTitle("#sigmaB (fb)")
plotGridHisto.GetYaxis().SetTitleOffset(0.6)
plotGridHisto.GetYaxis().SetTickLength(0.01)
SMXSErrorHisto.Draw("2")
overallXSGraph.Draw("0PZ")

#let's draw a legend.
theLegend = ROOT.TLegend(0.4,0.8,0.98,0.9)
theLegend.SetNColumns(3)
theLegend.AddEntry(inclusiveGraph,"Observed","P")
theLegend.AddEntry(inclusiveGraph,"#pm1#sigma","L")
theLegend.AddEntry(SMXSErrorHisto,"Uncertainty on SM prediction","F")
theLegend.SetBorderSize(0)
theLegend.SetFillStyle(0)
theLegend.Draw()
    

#okay, let's draw the latex
latex = ROOT.TLatex()
latex.SetTextSize(0.035)
latex.SetTextAlign(23)
latex.SetTextFont(42)
for parameterIndex in range(len(parameterOrder)):
    try:
        quotedNominal = roundToSigDigits(parameters_STXS[parameterOrder[parameterIndex]]['mu_value']*parameters_STXS[parameterOrder[parameterIndex]]['SMXS'],3)
        if math.floor(math.log10(abs(quotedNominal))) >= 2 and quotedNominal%1 == 0:
            quotedNominal = int(quotedNominal)
    except ValueError:
        quotedNominal = 0
    try:
        quotedUp = roundToSigDigits(parameters_STXS[parameterOrder[parameterIndex]]['uncert_up']*parameters_STXS[parameterOrder[parameterIndex]]['SMXS'],3)
        if math.floor(math.log10(abs(quotedUp))) >= 2 and quotedUp%1 == 0:
            quotedUp = int(quotedUp)
    except:
        quotedUp = 0
    try:
        quotedDown = roundToSigDigits(parameters_STXS[parameterOrder[parameterIndex]]['uncert_down']*parameters_STXS[parameterOrder[parameterIndex]]['SMXS'],3)
        if math.floor(math.log10(abs(quotedDown))) >= 2 and quotedDown%1 == 0:
            quotedDown = int(quotedDown)
    except ValueError:
        quotedDown = 0        
    latex.DrawLatex(parameterIndex+0.5,latexAlignments[parameterIndex],str(quotedNominal)+'^{+'+str(quotedUp)+'}_{-'+str(quotedDown)+'}')
    
#let's draw the other text pieces.
cmsLatex = ROOT.TLatex()
cmsLatex.SetTextSize(0.09)
cmsLatex.SetNDC(True)
cmsLatex.SetTextFont(61)
cmsLatex.SetTextAlign(11)
cmsLatex.DrawLatex(0.1,0.92,"CMS")
cmsLatex.SetTextFont(52)
cmsLatex.DrawLatex(0.1+0.09,0.92,"Preliminary")

cmsLatex.SetTextAlign(31)
cmsLatex.SetTextFont(42)
cmsLatex.DrawLatex(0.98,0.92,"137 fb^{-1} (13 TeV)")

#draw the ratio plot elements
ratioPad.cd()
ratioGridHisto.Draw()
ratioGridHisto.GetYaxis().SetTitle("Ratio to SM")
ratioGridHisto.GetYaxis().CenterTitle()
ratioGridHisto.GetYaxis().SetTitleOffset(0.45)
ratioGridHisto.GetYaxis().SetTitleSize(0.075)
ratioGridHisto.GetYaxis().SetNdivisions(7,0,0)
ratioGridHisto.GetXaxis().LabelsOption("v")
ratioGridHisto.GetXaxis().SetLabelSize(0.064)
ratioGridHisto.SetMaximum(4)
ratioGridHisto.SetMinimum(-3)

#let's make a strike out plot?
strikeOutPlot = ROOT.TH1F("StikeOut","StirkeOut",len(parameters_STXS),0.0,float(len(parameters_STXS)))
for i in range(1,len(parameters_STXS)+1):
    strikeOutPlot.SetBinContent(i,-10)
strikeOutPlot.SetFillColor(ROOT.kBlack)
strikeOutPlot.SetFillStyle(3354)

SMXSRatioErrorHisto.Draw('2')
strikeOutPlot.Draw("SAME")
ratioGraph.Draw("P0Z")

theCanvas.SaveAs("crossSectionTable.png")
theCanvas.SaveAs("crossSectionTable.pdf")
raw_input("Press Enter To Continue...")
