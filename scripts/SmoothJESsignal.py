#!/usr/bin/env python
import ROOT
from ROOT import *
import CombineHarvester.Run2HTT_Combine.CategoryConfigurations as catConfig
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

if args.channel=="et":
  #file_in=ROOT.TFile("smh_et.root","r")
  #file_out=ROOT.TFile("smh_et_smooth.root","recreate")
  #categories=["et_0jetlow","et_0jethigh","et_boosted1","et_boosted2","et_vbflow","et_vbfhigh"]
  categories=catConfig.Categories['et']
  #ncategories=6
  ncategories = len(categories)
  #for the moment we'll keep this, because I'm not sure how to handle year by year variations in this stuff...
  bins_mtt=[50,70,90,110,130,150,170,210,250,290]
  bins_0jetlow=[30,40,50,9000]
  bins_0jethigh=[30,40,50,9000]
  bins_boosted1=[0,60,120,200,250,9000]
  bins_boosted2=[0,60,120,200,250,9000]
  bins_vbflow=[350,700,1000,1500,1800,9000]
  bins_vbfhigh=[350,700,1200,9000]
  slices=[(len(bins_0jetlow)-1),(len(bins_0jethigh)-1),(len(bins_boosted1)-1),(len(bins_boosted2)-1),(len(bins_vbflow)-1),(len(bins_vbfhigh)-1)]

if args.channel == "mt":
  categories=catConfig.Categories['mt']
  ncategories = len(categories)
  bins_mtt=[50,70,90,110,130,150,170,210,250,290]
  bins_0jetlow=[30,40,50,9000]
  bins_0jethigh=[30,40,50,9000]
  bins_boosted1=[0,60,120,200,250,9000]
  bins_boosted2=[0,60,120,200,250,9000]
  bins_vbflow=[350,700,1000,1500,1800,9000]
  bins_vbfhigh=[350,700,1200,9000]
  slices=[(len(bins_0jetlow)-1),(len(bins_0jethigh)-1),(len(bins_boosted1)-1),(len(bins_boosted2)-1),(len(bins_vbflow)-1),(len(bins_vbfhigh)-1)]

if args.channel == "em":
  categories=catConfig.Categories['em']
  ncategories = len(categories)
  bins_mtt=[50,70,90,110,130,150,170,210,250,290]
  #bins_0jetlow=[30,40,50,9000]
  #bins_0jethigh=[30,40,50,9000]
  #bins_boosted1=[0,60,120,200,250,9000]
  #bins_boosted2=[0,60,120,200,250,9000]
  #bins_vbflow=[350,700,1000,1500,1800,9000]
  #bins_vbfhigh=[350,700,1200,9000]
  #slices=[(len(bins_0jetlow)-1),(len(bins_0jethigh)-1),(len(bins_boosted1)-1),(len(bins_boosted2)-1),(len(bins_vbflow)-1),(len(bins_vbfhigh)-1)]
  bins_0jet=[0,9000]
  bins_boosted1=[0,60,120,200,9000]
  bins_boosted2=[0,60,120,200,9000]
  bins_vbflow=[350,700,1000,1500,9000]
  bins_vbfhigh=[350,700,1000,9000]
  slices=[(len(bins_0jet)-1),(len(bins_boosted1)-1),(len(bins_boosted2)-1),(len(bins_vbflow)-1),(len(bins_vbfhigh)-1)]
  
if args.channel == "tt":
  categories=catConfig.Categories['tt']
  ncategories = len(categories)
  bins_mtt=[50,70,90,110,130,150,170,210,250,290]
  bins_0=[0,9000]  
  bins_boosted1=[0,60,120,200,250,9000]
  bins_boosted2=[0,60,120,200,250,9000]
  if args.Year == '2016':
    bins_vbflow=[0,350,700,1000,1500,9000]
    bins_vbfhigh=[0,350,700,9000]
  else:
    bins_vbflow=[0,350,700,1000,1500,1800,9000]
    bins_vbfhigh=[0,350,700,1200,9000]
  slices=[(len(bins_0)-1),(len(bins_boosted1)-1),(len(bins_boosted2)-1),(len(bins_vbflow)-1),(len(bins_vbfhigh)-1)]

file_in.cd()
nslices=0
bins_per_slice=0

dirList = gDirectory.GetListOfKeys()
for k1 in dirList:
         h1 = k1.ReadObj()
         file_out.mkdir(h1.GetName())
	 for icat in range(0,ncategories):
	    if categories[icat]==h1.GetName():
	       nslices=slices[icat]
	       bins_per_slice=len(bins_mtt)-1

         h1.cd()
         dirList2 = gDirectory.GetListOfKeys()
         for k2 in dirList2:
	    h2 = k2.ReadObj()
	    h_shape=h2.Clone()
	    if (("CMS_Jet" in k2.GetName() or "JER" in k2.GetName()) and ("ggH" in k2.GetName() or "qqH" in k2.GetName() or "TTT" in k2.GetName() or "TTL" in k2.GetName() or "VVL" in k2.GetName() or "VVT" in k2.GetName() or "STL" in k2.GetName() or "STT" in k2.GetName() or "ZL" in k2.GetName())) or (("boson_reso" in k2.GetName() or "boson_scale" in k2.GetName()) and ("ZL" in k2.GetName() or "hww" in k2.GetName())):
	      shortname=k2.GetName().split("_")[0]
	      if shortname=="ggH" or shortname=="qqH" or shortname=="WH" or shortname=="ZH":
		shortname=k2.GetName().split("_")[0]+"_"+k2.GetName().split("_")[1]

              h_nominal=h2.Clone()

	      for k3 in dirList2:
		if k3.GetName()==shortname:
		   h_nominal=k3.ReadObj()
              #print k2.GetName(),h1.GetName(),nslices
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



