# TH: plot to compare 5 plot files (all same line style)
# Make comparison plots from input files
# Note: currently does up to 5 input files. Can be extended to do more if desired
#
# A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/20

import sys
import os
import ROOT

# Extend to 10 colors
color = [ROOT.kAzure+1, ROOT.kViolet+1, ROOT.kPink-9, ROOT.kOrange+7, ROOT.kOrange-2]
style = [1,1,1,1,1]

if __name__ == '__main__':
    if len(sys.argv) < 5:
        print('Usage: %s printMeans(true/false) input1.root legendtitle1 input2.root legendtitle2 [input3.root legendtitle3 input4.root legendtitle4 input5.root legendtitle5]' % sys.argv[0])
        sys.exit(0)
    
    printMeans = sys.argv[1]
    print("Printing distribution means on legend = " + str(printMeans))

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPalette(ROOT.kBird)
    ROOT.gROOT.SetBatch(True)
    ROOT.TGaxis.SetMaxDigits(3);

    i = 0
    fv = []
    legendtitle = []
    for arg in sys.argv:
        if i<2:
            i = i+1
            continue
        if i%2 == 0:
            fv.append(ROOT.TFile(arg))
            print('Adding to comparison: %s' % arg)
        else:
            legendtitle.append(arg)
            print(' -- legend title: %s' % arg)
        i = i+1

    for i in range(len(fv)):
        assert(fv[i] and fv[i].IsOpen())

    assert(len(fv)==len(legendtitle))

    for k in fv[0].GetListOfKeys():
        ov = []
        print('OBJ %s' % k.GetName())
        for i in range(len(fv)):
            ov.append(fv[i].Get(k.GetName()))
            if(ov[i]):
                ov[i].SetLineColor(color[i])
                ov[i].SetMarkerStyle(21)
                #ov[0].SetFillColorAlpha(color[0], 0.2)
                ov[i].SetLineWidth(4)
                ov[i].SetLineStyle(style[i])

        c = ROOT.TCanvas('c', '', 1500, 1500)
        c.SetLeftMargin(0.15)

        ov[0].Sumw2()
        if ov[0].IsA() == ROOT.TH1F.Class():
            ov[0].GetXaxis().SetTitleOffset(1.2)
            ov[0].Draw("E")
            for i in range(len(ov)):
                if (ov[i]): 
                    ov[i].Draw('hist same')

            ymax = ov[0].GetMaximum()
            for i in range(len(ov)):
                if (ov[i] and ov[i].GetMaximum() > ymax):
                    ymax = ov[i].GetMaximum()
            ymax *= 1.5

            ov[0].GetYaxis().SetRangeUser(0, ymax)

            l = ROOT.TLegend(0.19, 0.67, 0.91, 0.89)
            l.SetBorderSize(0)
            l.SetFillColor(0)
            l.SetFillStyle(0)
            for i in range(len(ov)):
                if printMeans == 'true':
					l.AddEntry(ov[i],legendtitle[i] + " (#mu = " + str(round(1000*ov[i].GetMean(), 2)) + " MeV)")
                else:
					l.AddEntry(ov[i],legendtitle[i])
            l.Draw()

            plotname_pdf = '_'.join(['1DComp'] + k.GetName().split('_')[1:]) + '.pdf'
            plotname_root = '_'.join(['1DComp'] + k.GetName().split('_')[1:]) + '.root'
            c.SaveAs(plotname_pdf)
            c.SaveAs(plotname_root)

        elif ov[0].IsA() == ROOT.TH2F.Class():
            c.SetRightMargin(0.21)
            # First do a loop through all these plots to find the maximum z value and set all plots to have the same z scale
            zmax=0;
            for i in range(len(ov)):
                if (ov[i] and ov[i].GetMaximum() > zmax):
                    zmax = ov[i].GetMaximum()
            for i in range(len(ov)):
                if (ov[i]):
                    # Draw distribution as-is (comparisons are hard for 2D plots)
                    #c.SetLogz()
                    ov[i].SetTitle(' '.join(ov[i].GetTitle().split(' ')[0:] + legendtitle[i].split(' ')[0:]))
                    ov[i].GetXaxis().SetTitleOffset(1.2)
                    ov[i].GetZaxis().SetTitleOffset(2.)
                    ov[i].Draw('colz')
                    
                plotname_pdf = '_'.join(['2D'] + k.GetName().split('_')[1:] + legendtitle[i].split(' ')[0:]) + '.pdf'
                plotname_root = '_'.join(['2D'] + k.GetName().split('_')[1:] + legendtitle[i].split(' ')[0:]) + '.root'
                c.SaveAs(plotname_pdf)
                c.SaveAs(plotname_root)

        del c
