#!/usr/bin/env python
import ROOT
from ROOT import *
import re
from array import array

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--channel', '-c', nargs='?',required = True,choices=['tt','mt','et','em'], default=None, help='channel to smooth')
parser.add_argument('--InputFile','-i',nargs='?',required = True,help="File to smooth")
parser.add_argument('--OutputFile','-o',nargs='?',required = True,help="Output file name")
parser.add_argument('--Year','-y',nargs='?',required=True,choices=['2016','2017','2018'],help="Year to smooth. Used for figuring out differences in binning/rolling.")
args = parser.parse_args()


file_in = ROOT.TFile(args.InputFile)
file_out = ROOT.TFile(args.OutputFile,"RECREATE")

file_in.cd()
nslices=1
bins_per_slice=9

dirList = gDirectory.GetListOfKeys()
for k1 in dirList:
         h1 = k1.ReadObj()
         file_out.mkdir(h1.GetName())

         h1.cd()
         dirList2 = gDirectory.GetListOfKeys()
         for k2 in dirList2:
	    h2 = k2.ReadObj()
	    nslices=(h2.GetSize()-2)/bins_per_slice
	    h_shape=h2.Clone()
	    if (("CMS_scale_j" in k2.GetName() or "CMS_res_j" in k2.GetName()) or (("boson_reso" in k2.GetName() or "boson_scale" in k2.GetName()) and ("ZL" in k2.GetName() or "OutsideAcceptance" in k2.GetName()))):
	      shortname=k2.GetName().split("_CMS")[0]

              h_nominal=h2.Clone()

	      for k3 in dirList2:
		if k3.GetName()==shortname:
		   h_nominal=k3.ReadObj()

	      for i in range(0,nslices):
		    factor=0.0
		    integral=h_shape.Integral(i*bins_per_slice+1,i*bins_per_slice+bins_per_slice)

		    if h_nominal.Integral(i*bins_per_slice+1,i*bins_per_slice+bins_per_slice)>0:
		       factor=h_shape.Integral(i*bins_per_slice+1,i*bins_per_slice+bins_per_slice)/h_nominal.Integral(i*bins_per_slice+1,i*bins_per_slice+bins_per_slice)

		    for j in range(0,bins_per_slice):
		       h_shape.SetBinContent(i*bins_per_slice+1+j,h_nominal.GetBinContent(i*bins_per_slice+1+j)*factor)

	    file_out.cd(h1.GetName())
            h_shape.SetName(k2.GetName())
            h_shape.Write()



