import ROOT

prefitTextFont = 52
prefitTextAlign = 31
prefitTextSize = 0.05
prefitTextPosition = (0.95,0.9)

def CreatePrefitPostfitText(prefitOrPostfit):
    prefitText = ROOT.TLatex()
    prefitText.SetNDC()
    prefitText.SetTextColor(ROOT.kBlack)
    prefitText.SetTextFont(prefitTextFont)
    prefitText.SetTextAlign(prefitTextAlign)
    prefitText.SetTextSize(prefitTextSize)
    prefitText.DrawLatex(prefitTextPosition[0],prefitTextPosition[1],prefitOrPostfit.capitalize())
