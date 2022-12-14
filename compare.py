# Make comparison plots from input files
# Note: currently does up to 5 input files. Can be extended to do more if desired
#
# A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/20

import sys
import os
import ROOT

# Extend to 10 colors
#color = [ROOT.kBlack, ROOT.kRed+1, ROOT.kBlue+1, ROOT.kGreen+3, ROOT.kMagenta+3, ROOT.kOrange+7,
#color = [ROOT.kBlack, ROOT.kRed+1, ROOT.kBlue+1, ROOT.kGreen+3, ROOT.kMagenta+3, ROOT.kOrange+7,
#			ROOT.kPink+2, ROOT.kCyan-3, ROOT.kTeal+1, ROOT.kYellow+2]

color = [ROOT.kBlack, ROOT.kBlack, ROOT.kBlue+1, ROOT.kBlue+1, ROOT.kGreen+3, ROOT.kGreen+3,
		ROOT.kMagenta+3, ROOT.kMagenta+3, ROOT.kOrange+7, ROOT.kOrange+7]

#style = [1,9,2,3,4,6,7,8,5,10]
style = [1,3,1,3,1,3,1,3,1,3]
#color = [ROOT.kRed,ROOT.kBlue,ROOT.kOrange-3,ROOT.kMagenta+2,ROOT.kGreen+2]
#style = [1,9,2,3,4]

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print('Usage: %s  input1.root legendtitle1 input2.root legendtitle2 [input3.root legendtitle3 input4.root legendtitle4 input5.root legendtitle5]' % sys.argv[0])
        sys.exit(0)

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPalette(ROOT.kBird)
    ROOT.gROOT.SetBatch(True)

    i = 0
    fv = []
    legendtitle = []
    for arg in sys.argv:
        if i<1:
            i = i+1
            continue
        if i%2 != 0:
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

            l = ROOT.TLegend(0.5, 0.65, 0.88, 0.88)
            l.SetBorderSize(0)
            l.SetFillColor(0)
            l.SetFillStyle(0)
            for i in range(len(ov)):
                l.AddEntry(ov[i],legendtitle[i])
            l.Draw()

            plotname = '_'.join(['1DComp'] + k.GetName().split('_')[1:]) + '.pdf'
            c.SaveAs(plotname)

        elif ov[0].IsA() == ROOT.TH2F.Class():
            c.SetRightMargin(0.27)
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
                    #ov[i].GetZaxis().SetTitle('Ratio'+legendtitle[i]+'/'+legendtitle[0])
                    #ov[i].GetZaxis().SetTitle(legendtitle[i])
                    #ov[i].SetMinimum(0)
                    #ov[i].SetMaximum(zmax)
                    ov[i].Draw('colz')
                    ov[i].GetZaxis().SetTitleOffset(1.25)
                    
                plotname = '_'.join(['2D'] + k.GetName().split('_')[1:] + legendtitle[i].split(' ')[0:]) + '.pdf'
                c.SaveAs(plotname)

                # Draw ratio to the first one given in arguments
                # Comment out for now because it's just confusing
                # ov[i].Divide(ov[0])
                # ov[i].Draw('colz')
                # #ov[i].GetZaxis().SetRangeUser(0,2)
                #
                # plotname = '_'.join(['cmp'] + legendtitle[i].split(' ')[1:] + legendtitle[0].split(' ')[0:] + k.GetName().split('_')[1:]) + '.pdf'
                # c.SaveAs(plotname)

        del c

os.system("pdfunite 1D*numu_ccqe* numu_CCQE_1D.pdf")
os.system("pdfunite 1D*numu_ccmec* numu_CCMEC_1D.pdf")
os.system("pdfunite 1D*numu_ccres* numu_CCRES_1D.pdf")
os.system("pdfunite 1D*numu_ccdis* numu_CCDIS_1D.pdf")
os.system("pdfunite 1D*numu_nc* numu_NC_1D.pdf")

os.system("pdfunite 2D*numu_ccqe* numu_CCQE_2D.pdf")
os.system("pdfunite 2D*numu_ccmec* numu_CCMEC_2D.pdf")
os.system("pdfunite 2D*numu_ccres* numu_CCRES_2D.pdf")
os.system("pdfunite 2D*numu_ccres* numu_CCDIS_2D.pdf")
os.system("pdfunite 2D*numu_nc* numu_NC_2D.pdf")

os.system("pdfunite 1D*numubar_ccqe* numubar_CCQE_1D.pdf")
os.system("pdfunite 1D*numubar_ccmec* numubar_CCMEC_1D.pdf")
os.system("pdfunite 1D*numubar_ccres* numubar_CCRES_1D.pdf")
os.system("pdfunite 1D*numubar_ccdis* numubar_CCDIS_1D.pdf")
os.system("pdfunite 1D*numubar_nc* numubar_NC_1D.pdf")

os.system("pdfunite 2D*numubar_ccqe* numubar_CCQE_2D.pdf")
os.system("pdfunite 2D*numubar_ccmec* numubar_CCMEC_2D.pdf")
os.system("pdfunite 2D*numubar_ccres* numubar_CCRES_2D.pdf")
os.system("pdfunite 2D*numubar_ccres* numubar_CCDIS_2D.pdf")
os.system("pdfunite 2D*numubar_nc* numubar_NC_2D.pdf")

os.system("pdfunite 1D*nue_ccqe* nue_CCQE_1D.pdf")
os.system("pdfunite 1D*nue_ccmec* nue_CCMEC_1D.pdf")
os.system("pdfunite 1D*nue_ccres* nue_CCRES_1D.pdf")
os.system("pdfunite 1D*nue_ccdis* nue_CCDIS_1D.pdf")
os.system("pdfunite 1D*nue_nc* nue_NC_1D.pdf")

os.system("pdfunite 2D*nue_ccqe* nue_CCQE_2D.pdf")
os.system("pdfunite 2D*nue_ccmec* nue_CCMEC_2D.pdf")
os.system("pdfunite 2D*nue_ccres* nue_CCRES_2D.pdf")
os.system("pdfunite 2D*nue_ccres* nue_CCDIS_2D.pdf")
os.system("pdfunite 2D*nue_nc* nue_NC_2D.pdf")

os.system("pdfunite 1D*nuebar_ccqe* nuebar_CCQE_1D.pdf")
os.system("pdfunite 1D*nuebar_ccmec* nuebar_CCMEC_1D.pdf")
os.system("pdfunite 1D*nuebar_ccres* nuebar_CCRES_1D.pdf")
os.system("pdfunite 1D*nuebar_ccdis* nuebar_CCDIS_1D.pdf")
os.system("pdfunite 1D*nuebar_nc* nuebar_NC_1D.pdf")

os.system("pdfunite 2D*nuebar_ccqe* nuebar_CCQE_2D.pdf")
os.system("pdfunite 2D*nuebar_ccmec* nuebar_CCMEC_2D.pdf")
os.system("pdfunite 2D*nuebar_ccres* nuebar_CCRES_2D.pdf")
os.system("pdfunite 2D*nuebar_ccres* nuebar_CCDIS_2D.pdf")
os.system("pdfunite 2D*nuebar_nc* nuebar_NC_2D.pdf")

os.system("pdfunite 1D*nutau_ccqe* nutau_CCQE_1D.pdf")
os.system("pdfunite 1D*nutau_ccmec* nutau_CCMEC_1D.pdf")
os.system("pdfunite 1D*nutau_ccres* nutau_CCRES_1D.pdf")
os.system("pdfunite 1D*nutau_ccdis* nutau_CCDIS_1D.pdf")
os.system("pdfunite 1D*nutau_nc* nutau_NC_1D.pdf")

os.system("pdfunite 2D*nutau_ccqe* nutau_CCQE_2D.pdf")
os.system("pdfunite 2D*nutau_ccmec* nutau_CCMEC_2D.pdf")
os.system("pdfunite 2D*nutau_ccres* nutau_CCRES_2D.pdf")
os.system("pdfunite 2D*nutau_ccres* nutau_CCDIS_2D.pdf")
os.system("pdfunite 2D*nutau_nc* nutau_NC_2D.pdf")

os.system("pdfunite 1D*nutaubar_ccqe* nutaubar_CCQE_1D.pdf")
os.system("pdfunite 1D*nutaubar_ccmec* nutaubar_CCMEC_1D.pdf")
os.system("pdfunite 1D*nutaubar_ccres* nutaubar_CCRES_1D.pdf")
os.system("pdfunite 1D*nutaubar_ccdis* nutaubar_CCDIS_1D.pdf")
os.system("pdfunite 1D*nutaubar_nc* nutaubar_NC_1D.pdf")

os.system("pdfunite 2D*nutaubar_ccqe* nutaubar_CCQE_2D.pdf")
os.system("pdfunite 2D*nutaubar_ccmec* nutaubar_CCMEC_2D.pdf")
os.system("pdfunite 2D*nutaubar_ccres* nutaubar_CCRES_2D.pdf")
os.system("pdfunite 2D*nutaubar_ccres* nutaubar_CCDIS_2D.pdf")
os.system("pdfunite 2D*nutaubar_nc* nutaubar_NC_2D.pdf")

os.system("rm 1D* 2D*")
