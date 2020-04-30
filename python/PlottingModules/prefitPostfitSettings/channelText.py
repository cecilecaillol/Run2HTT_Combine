import ROOT
import CombineHarvester.Run2HTT_Combine.CategoryConfigurations as catConfig

channelTextFont = 42
channelTextAlign = 21
channelTextSize = 0.05
channelTextPosition = (0.6,0.97)

categoryTextFont = 52
categoryTextAlign = 11
categoryTextSize = 0.05
categoryTextPosition = (0.18,0.89)

zeroJetLowText = "0 Jet, #DeltaR > 3"
zeroJetHighText = "0 Jet, #DeltaR < 3"
VBFLowText = "VBF, p_{t}^{H} < 200"
VBFHighText = "VBF, p_{t}^{H} > 200"
BoostedOneJetText = "Boosted monojet"
BoostedGE2JText = "Boosted multijet"

tt_ZeroJetText = 'Zero Jet Category'

def DrawChannelName(channel):
    print channel
    theChannel = 'Default'
    if channel == 'tt':
        theChannel = '#tau_{h}#tau_{h}'
    elif channel == 'mt':
        theChannel = '#mu#tau_{h}'
    elif channel == 'et':
        theChannel = 'e#tau_{h}'
    elif channel == 'em':
        theChannel = 'e#mu'
    channelLatex = ROOT.TLatex()
    channelLatex.SetNDC()
    channelLatex.SetTextColor(ROOT.kBlack)
    channelLatex.SetTextFont(channelTextFont)
    channelLatex.SetTextAlign(channelTextAlign)
    channelLatex.SetTextSize(channelTextSize)
    channelLatex.DrawLatex(channelTextPosition[0],channelTextPosition[1],theChannel)
    

def DrawCategoryName(category):    
    theText = 'Default'
    if (category == catConfig.tt_0jet_category
        or category == catConfig.em_0jet_category):
        theText = tt_ZeroJetText
    elif (category == catConfig.mt_0jet_low_category
          or category == catConfig.et_0jet_low_category):
        theText = zeroJetLowText
    elif (category == catConfig.mt_0jet_high_category
          or category == catConfig.et_0jet_high_category):
        theText = zeroJetHighText
    elif (category == catConfig.tt_vbf_low_category
          or category == catConfig.mt_vbf_low_category
          or category == catConfig.et_vbf_low_category
          or category == catConfig.em_vbf_low_category):
        theText = VBFLowText
    elif (category == catConfig.tt_vbf_high_category
          or category == catConfig.mt_vbf_high_category
          or category == catConfig.et_vbf_high_category
          or category == catConfig.em_vbf_high_category):
        theText = VBFHighText
    elif (category == catConfig.tt_boosted_1J_category
          or category == catConfig.mt_boosted_1J_category
          or category == catConfig.et_boosted_1J_category
          or category == catConfig.em_boosted_1J_category):
        theText = BoostedOneJetText
    elif (category == catConfig.tt_boosted_GE2J_category
          or category == catConfig.mt_boosted_GE2J_category
          or category == catConfig.et_boosted_GE2J_category
          or category == catConfig.em_boosted_GE2J_category):
        theText = BoostedGE2JText

    categoryLatex = ROOT.TLatex()
    categoryLatex.SetNDC()
    categoryLatex.SetTextFont(categoryTextFont)
    categoryLatex.SetTextColor(ROOT.kBlack)
    categoryLatex.SetTextAlign(categoryTextAlign)
    categoryLatex.SetTextSize(categoryTextSize)
    categoryLatex.DrawLatex(categoryTextPosition[0],categoryTextPosition[1],theText)
