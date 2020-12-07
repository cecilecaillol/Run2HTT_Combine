import ROOT
from .. import Utilities
from array import array

YBounds = (0.5,1.5)

ratioMarkerStyle = 20
ratioYAxisTitle = 'Obs / H#rightarrow#tau#tau + Bkg.'
ratioYAxisTitleSize = 0.08
ratioYAxisTitleOffset = 0.62
ratioYAxisLabelSize = 0.1
ratioYAxisNDivisions = (6,0,0)

errorFillStyle = 3001 #3444
errorFillColor = 15

def poisson_errors(N,coverage=0.6827):
    alpha = 1.0-coverage 
    L,U = 0,0
    if N>0:
        L = ROOT.Math.gamma_quantile(alpha/2, N, 1.)
    U = ROOT.Math.gamma_quantile_c(alpha/2,N+1,1)
    return L,U

def convert(histogram):
    output = ROOT.TGraphAsymmErrors(histogram)
    
    for i in range(0,histogram.GetSize()-2):
        yield_in_bin = output.GetY()[i]
        if yield_in_bin<0:
            yield_in_bin = 0
        N=int(yield_in_bin)

        L,U = poisson_errors(N)

        output.SetPointEYlow(i,N-L)
        output.SetPointEYhigh(i,U-N)
    return output

def setRatioErrors(ratio,theData):
    output = ROOT.TGraphAsymmErrors(ratio)
    for i in range(1,ratio.GetNbinsX()+1):
        try:
            #print ''
            #print("bin #"+str(i))
            #print("Data: +"+str(theData.GetBinErrorUp(i))+"/-"+str(theData.GetBinErrorLow(i)))
            #print("Ratio: "+str(ratio.GetBinContent(i))+" Error:"+str(theData.GetBinErrorUp(i)/theData.GetBinContent(i)*ratio.GetBinContent(i))+'/-'+str(theData.GetBinErrorLow(i)/theData.GetBinContent(i)*ratio.GetBinContent(i)))            
            output.SetPointEYlow(i-1,theData.GetBinErrorLow(i)/theData.GetBinContent(i)*ratio.GetBinContent(i))
            output.SetPointEYhigh(i-1,theData.GetBinErrorUp(i)/theData.GetBinContent(i)*ratio.GetBinContent(i))
        except ZeroDivisionError:
            output.SetPointEYlow(i,0)
            output.SetPointEYhigh(i,0)
    return output

def MakeRatioPlot(theStack,stackErrors,theData):    
        
    ratioHist = theData.Clone()

    denominatorHistos = theStack.GetHists().At(0).Clone()
    denominatorHistos.Reset()
    listOfStackHistograms = theStack.GetHists()
    for i in range(theStack.GetNhists()):        
        denominatorHistos.Add(theStack.GetHists().At(i))        
        
    p_x = ratioHist.GetX()
    p_y = ratioHist.GetY()
    
    for i in range(0,theData.GetN()):
        ratioHist.SetPoint(i,p_x[i],p_y[i]/(denominatorHistos.GetBinContent(i+1)+0.0001))
        ratioHist.SetPointEYhigh(i,ratioHist.GetErrorYhigh(i)/(denominatorHistos.GetBinContent(i+1)+0.0001))
        ratioHist.SetPointEYlow(i,ratioHist.GetErrorYlow(i)/(denominatorHistos.GetBinContent(i+1)+0.0001))
        
    #ratioHist.Divide(theData,denominatorHistos,'pois')    
                
    #ratioHist = convert(ratioHist)
    
    #ratioHist = setRatioErrors(ratioHist,theData)

    ratioHist.SetMarkerStyle(ratioMarkerStyle)
    #ratioHist.GetXaxis().SetRangeUser(theData.GetXaxis().GetXmin(),theData.GetXaxis().GetXmax())     

    #MCErrors = ROOT.TH1F("MCErrors","MCErrors",
    #                     nBins,
    #                     binBoundaryArray)
    MCErrors = stackErrors.Clone()
    MCErrors.Reset()
    for i in range (1,MCErrors.GetNbinsX()+1):
        MCErrors.SetBinContent(i,1.0)
        try:
            MCErrors.SetBinError(i,stackErrors.GetBinError(i)/denominatorHistos.GetBinContent(i))
        except:
            MCErrors.SetBinError(i,0)
    MCErrors.SetFillStyle(errorFillStyle)
    MCErrors.SetFillColor(errorFillColor)
    MCErrors.SetMarkerStyle(1)

    MCErrors.GetYaxis().SetTitle(ratioYAxisTitle)
    MCErrors.GetYaxis().SetTitleSize(ratioYAxisTitleSize)
    MCErrors.GetYaxis().SetTitleOffset(ratioYAxisTitleOffset)
    MCErrors.GetYaxis().CenterTitle()
    MCErrors.GetYaxis().SetLabelSize(ratioYAxisLabelSize)
    MCErrors.GetYaxis().SetNdivisions(ratioYAxisNDivisions[0],
                                       ratioYAxisNDivisions[1],
                                       ratioYAxisNDivisions[2])
    MCErrors.GetYaxis().SetRangeUser(YBounds[0],YBounds[1])           

    return ratioHist,MCErrors
