#!/usr/bin/env python
import ROOT
import argparse
import CombineHarvester.Run2HTT_Combine.PlottingModules.globalSettings as globalSettings

parameters_STXS = {
    'r':{
        'SMXS': 54.257*1e3 * 0.0627, #higgs XS is 54.257 pb, scale to fb, and use tau BR
        'mu_value':0.782,
        'uncert_down':0.123,
        'uncert_up':0.137,
        'color':ROOT.kBlack,
        'axis':'#sigma_{H#rightarrow#tau#tau}',
        'SMXS_mu_up': 0.0642,
        'SMXS_mu_down': 0.0530,        
        'quotedXS':'2660^{+218}_{-180}'
    },
    'r_ggH':{
        'SMXS': 3051.344, #ggH XS is 48.68 pb, scale to fb and use tau BR (+ggZH)
        'mu_value':0.884,
        'uncert_down':0.188,
        'uncert_up':0.218,
        'color':ROOT.kAzure+7,
        'axis':'#sigma_{ggH#rightarrow#tau#tau}',
        'SMXS_mu_up': 0.0795,
        'SMXS_mu_down':0.0335,
        'quotedXS':'2700^{+243}_{-102}'
        },
    'r_qqH':{
        'SMXS': 328.6833, #total qqH cross section as we consider it
        'mu_value': 0.647,
        'uncert_down': 0.232,
        'uncert_up': 0.237,
        'color':ROOT.kOrange-3,
        'axis':'#sigma_{qqH#rightarrow#tau#tau}',
        'SMXS_mu_up': 0.0572,
        'SMXS_mu_down':0.0640,
        'quotedXS':'213^{+18.8}_{-21.0}'
        },
    'r_ggH_0J':{
        'SMXS': 1752.942,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kAzure+7,
        'axis':'ggH: 0 Jet',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},
    'r_ggH_1J_PTH_0_60':{
        'SMXS': 451.0862,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kAzure+7,
        'axis':'ggH: 1 Jet, p_{T}^{H}[0,60]',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},
    'r_ggH_1J_PTH_60_120':{
        'SMXS': 287.6776,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kAzure+7,
        'axis':'ggH: 1 Jet, p_{T}^{H}[60,120]',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},
    'r_ggH_1J_PTH_120_200':{
        'SMXS': 50.04276,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kAzure+7,
        'axis':'ggH: 1 Jet, p_{T}^{H}[120,200]',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},
    'r_ggH_PTH_0_200_GE2J':{
        'SMXS': 306.2587,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kAzure+7,
        'axis':'ggH: #geq 2 Jet',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},
    'r_ggH_PTH_200':{
        'SMXS': 27.51224,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kAzure+7,
        'axis':'ggH: p_{T}^{H}[200,300]',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},
    'r_ggH_PTH_GE300 To':{
        'SMXS': 7.188811,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kAzure+7,
        'axis':'ggH: p_{T}^{H} > 300',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},
    'r_qqH_NONVBFTOPO':{
        'SMXS': 209.42,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kOrange-3,
        'axis':'qqH: Non-VBF Topology',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},
    'r_qqH_GE2J_MJJ_350_700_PTH_0_200':{
        'SMXS': 35.51896,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kOrange-3,
        'axis':'qqH: #geq 2 Jets, m_{jj}[350,700]',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},
    'r_qqH_GE2J_MJJ_GE700_PTH_0_200':{
        'SMXS': 46.1111,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kOrange-3,
        'axis':'qqH: #geq 2 Jets, m_{jj} > 700',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},    
    'r_qqH_BSM':{
        'SMXS': 10.18151,
        'mu_value':1,
        'uncert_down':0.5,
        'uncert_up':0.5,
        'color':ROOT.kOrange-3,
        'axis':'qqH: p_{T}^{H} > 200',
        'SMXS_mu_up': 0.05,
        'SMXS_mu_down': 0.05,        
        'quotedXS':'1^{+1}_{-1}'},    
    }

parameterOrder = ['r','r_ggH','r_qqH',
                  'r_ggH_0J',
                  'r_ggH_1J_PTH_0_60',
                  'r_ggH_1J_PTH_60_120',
                  'r_ggH_1J_PTH_120_200',
                  'r_ggH_PTH_0_200_GE2J',
                  'r_ggH_PTH_200',
                  'r_ggH_PTH_GE300 To',
                  'r_qqH_NONVBFTOPO',
                  'r_qqH_GE2J_MJJ_350_700_PTH_0_200',
                  'r_qqH_GE2J_MJJ_GE700_PTH_0_200', 
                  'r_qqH_BSM',]
latexAlignments = [10000,10000,900,
                   5000,
                   1300,
                   1200,
                   150,
                   850,
                   100,
                   20,
                   800,
                   100,
                   150,
                   25,]

ggHParameters = [
    'r_ggH_0J',
    'r_ggH_1J_PTH_0_60',
    'r_ggH_1J_PTH_60_120',
    'r_ggH_1J_PTH_120_200',
    'r_ggH_PTH_0_200_GE2J',
    'r_ggH_PTH_200',
    'r_ggH_PTH_GE300 To',
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

#framePad = plotPad.Clone()
#framePad.SetLogy()
#framePad.SetTickx()
#framePad.SetTicky()
#framePad.SetGridx()

plotPad.SetLogy()
plotPad.SetLogy()
plotPad.SetTickx()
plotPad.SetTicky()
plotPad.SetGridx()
#plotPad.SetTopMargin(0.1)
#plotPad.SetLeftMargin(0.1)
#plotPad.SetFillStyle(4000)

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

ratioGridHisto = ROOT.TH1F("RatioGraphFrame","RatioGraphFrame",len(parameters_STXS),0.0,float(len(parameters_STXS)))
ratioGridHisto.GetXaxis().SetNdivisions(len(parameters_STXS),0,0)
ratioGridHisto.GetXaxis().SetTickLength(0)


#okay, now, let's create a multi graph that contains the points
#crossSectionGraph = ROOT.TGraphAsymmErrors(len(parameters_STXS.keys()))
#i=0
#for parameter in parameters_STXS.keys():
#    crossSectionGraph.SetPoint(i,i,parameters_STXS[parameter]['SMXS']*parameters_STXS[parameter]['mu_value'])
#    crossSectionGraph.SetPointEYlow(i,parameters_STXS[parameter]['SMXS']*parameters_STXS[parameter]['uncert_down'])
#    crossSectionGraph.SetPointEYhigh(i,parameters_STXS[parameter]['SMXS']*parameters_STXS[parameter]['uncert_up'])
#    i+=1
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
SMXSErrorHisto.SetFillStyle(3244)
SMXSErrorHisto.SetFillColor(ROOT.kGray+2)

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
SMXSRatioErrorHisto.SetFillStyle(3244)
SMXSRatioErrorHisto.SetFillColor(ROOT.kGray+2)

for parameterIndex in range(len(parameterOrder)):
    ratioGridHisto.GetXaxis().SetBinLabel(parameterIndex+1,parameters_STXS[parameterOrder[parameterIndex]]['axis'])

#draw the overall plot elements
plotPad.cd()
plotGridHisto.Draw()
plotGridHisto.SetMaximum(20000)
plotGridHisto.SetMinimum(5)
plotGridHisto.GetYaxis().SetTitle("#sigma [fb]")
SMXSErrorHisto.Draw("2")
overallXSGraph.Draw("0PZ")

#okay, let's draw the latex
latex = ROOT.TLatex()
latex.SetTextSize(0.035)
latex.SetTextAlign(23)
latex.SetTextFont(42)
for parameterIndex in range(len(parameterOrder)):
    latex.DrawLatex(parameterIndex+0.5,latexAlignments[parameterIndex],parameters_STXS[parameterOrder[parameterIndex]]['quotedXS'])
    
#let's draw the other text pieces.
cmsLatex = ROOT.TLatex()
cmsLatex.SetTextSize(0.05)
cmsLatex.SetNDC(True)
cmsLatex.SetTextFont(61)
cmsLatex.SetTextAlign(11)
cmsLatex.DrawLatex(0.16,0.955,"CMS")
cmsLatex.SetTextFont(52)
cmsLatex.DrawLatex(0.16+0.045,0.955,"Preliminary")

cmsLatex.SetTextAlign(31)
cmsLatex.SetTextFont(42)
cmsLatex.DrawLatex(0.98,0.955,"137 fb^{-1} (13 TeV)")

#draw the ratio plot elements
ratioPad.cd()
ratioGridHisto.Draw()
ratioGridHisto.GetYaxis().SetTitle("Ratio to SM")
ratioGridHisto.GetYaxis().CenterTitle()
ratioGridHisto.GetYaxis().SetTitleOffset(0.6)
ratioGridHisto.GetYaxis().SetTitleSize(0.1)
ratioGridHisto.GetXaxis().LabelsOption("v")
ratioGridHisto.GetXaxis().SetLabelSize(0.05)
ratioGridHisto.SetMaximum(2)
ratioGridHisto.SetMinimum(0)
SMXSRatioErrorHisto.Draw('2')
ratioGraph.Draw("P0Z")

raw_input("Press Enter To Continue...")
