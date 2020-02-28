import ROOT
import CombineHarvester.Run2HTT_Combine.CategoryConfigurations as catConfig
from rebinning import GetNSlices

sliceTextFont = 52
sliceTextAlign = 11

boosted_latex = ["p_{t}^{H} #leq 60","60 #leq p_{t}^{H} #leq 120","120 #leq p_{t}^{H} #leq 200","200 #leq p_{t}^{H} #leq 250","250 #leq p_{t}^{H}"]

tautau_boosted_latex = ["p_{t}^{H} #leq 60","60 #leq p_{t}^{H} #leq 120","120 #leq p_{t}^{H} #leq 200","200 #leq p_{t}^{H} #leq 250","250 #leq p_{t}^{H} #leq 300","300 #leq p_{t}^{H}"]

semileptonic_vbf_high_latex = ["350 #leq m_{jj} #leq 700","700 #leq m_{jj} #leq 1200","1200 #leq m_{jj}"]
vbf_low_latex = ["350 #leq m_{jj} #leq 700","700 #leq m_{jj} #leq 1000","1000 #leq m_{jj} #leq 1500","1500 #leq m_{jj} #leq 1800","1800 #leq m_{jj}"]
et_vbf_low_latex = ["350 #leq m_{jj} #leq 700","700 #leq m_{jj} #leq 1000","1000 #leq m_{jj} #leq 1500","1500 #leq m_{jj}"]

tautau_vbf_high_latex_2016 = ["m_{jj} #leq 350","350 #leq m_{jj} #leq 700","700 #leq m_{jj}"]
tautau_vbf_high_latex = ["m_{jj} #leq 350","350 #leq m_{jj} #leq 700","700 #leq m_{jj} #leq 1200","1200 #leq m_{jj}"]

tautau_vbf_low_latex_2016 = ["m_{jj} #leq 350","350 #leq m_{jj} #leq 700","700 #leq m_{jj} #leq 1000","1000 #leq m_{jj} #leq 1500","1500 #leq m_{jj}"]
tautau_vbf_low_latex = ["m_{jj} #leq 350","350 #leq m_{jj} #leq 700","700 #leq m_{jj} #leq 1000","1000 #leq m_{jj} #leq 1500","1500 #leq m_{jj} #leq 1800","1800 #leq m_{jj}"]


zerojet_latex = ["30 #leq p_{t}^{#tau} #leq 40","40 #leq p_{t}^{#tau} #leq 50","50 #leq p_{t}^{#tau}"]

def CreateSliceText(category,year):
    nSlices = GetNSlices(category,year)

    sliceLatex = ROOT.TLatex()
    sliceLatex.SetNDC()
    sliceLatex.SetTextColor(ROOT.kBlack)
    sliceLatex.SetTextAlign(sliceTextAlign)    
    if nSlices == 0:
        sliceLatex.SetTextSize(0.12)        
    else:
        sliceLatex.SetTextSize(0.12/(1.0*nSlices))        
    
    if (category ==  catConfig.tt_vbf_high_category):
        if year == "2016" or year == "Run2":
            labels = tautau_vbf_high_latex_2016
        else:
            labels = tautau_vbf_high_latex

    elif (category == catConfig.tt_vbf_low_category):
        if year == "2016" or year == "Run2":
            labels = tautau_vbf_low_latex_2016        
        else:
            labels = tautau_vbf_low_latex
    
    elif(category == catConfig.et_boosted_1J_category or 
          category == catConfig.et_boosted_GE2J_category):        
            labels = boosted_latex

    elif (category ==  catConfig.mt_vbf_high_category or 
        category == catConfig.et_vbf_high_category or 
        category == catConfig.em_vbf_high_category):        
        labels = semileptonic_vbf_high_latex

    elif (category == catConfig.et_vbf_low_category):
        if year == "2016" or year == "Run2":
            labels = et_vbf_low_latex
        else:
            labels = vbf_low_latex

    elif (category == catConfig.mt_vbf_low_category or 
          category == catConfig.em_vbf_low_category):        
        labels = vbf_low_latex

    elif (category == catConfig.tt_boosted_1J_category or 
          category == catConfig.tt_boosted_GE2J_category):
        labels = tautau_boosted_latex

    elif (category == catConfig.mt_boosted_1J_category or 
          category == catConfig.mt_boosted_GE2J_category or           
          category == catConfig.em_boosted_1J_category or 
          category == catConfig.em_boosted_GE2J_category):
        labels = boosted_latex

    elif (category == catConfig.mt_0jet_high_category or 
          category == catConfig.mt_0jet_low_category or 
          category == catConfig.et_0jet_high_category or 
          category == catConfig.et_0jet_low_category or 
          category == catConfig.em_0jet_high_category or 
          category == catConfig.em_0jet_low_category):
        labels = zerojet_latex
    else:
        return

    for i in range (nSlices):
        xpos = 0.18+i*((1.0-0.18)/nSlices)
        ypos = 0.83
        sliceLatex.DrawLatex(xpos,ypos,labels[i])    
