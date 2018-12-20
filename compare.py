# Make comparison plots from two input files
#
# A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/20

import sys
import ROOT

colorv2 = ROOT.kRed
colorv3 = ROOT.kBlue
normalize = False

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Usage: %s v2.root v3.root' % sys.argv[0])
        sys.exit(0)

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPalette(ROOT.kBird)
    ROOT.gROOT.SetBatch(True)

    inv2, inv3 = sys.argv[1:3]

    fv2 = ROOT.TFile(inv2)
    assert(fv2 and fv2.IsOpen())
    print('GENIEv2: %s' % inv2)

    fv3 = ROOT.TFile(inv3)
    assert(fv3 and fv3.IsOpen())
    print('GENIEv3: %s' % inv3)

    for k in fv2.GetListOfKeys():
        print('OBJ %s' % k.GetName())
        ov2 = fv2.Get(k.GetName())
        ov3 = fv3.Get(k.GetName())
        assert(ov2 and ov3)

        ov2.SetLineColor(colorv2)
        ov2.SetLineWidth(2)

        ov3.SetLineColor(colorv3)
        ov3.SetLineWidth(2)

        if normalize:
            ov2.Scale(1.0 / ov2.Integral())
            ov3.Scale(1.0 / ov3.Integral())

        c = ROOT.TCanvas('c', '', 500, 500)
        c.SetLeftMargin(0.15)

        if ov2.IsA() == ROOT.TH1F.Class():
            ov2.Draw('e1')
            ov3.Draw('e1 same')

            ymax = 1.1 * max(ov2.GetMaximum(), ov3.GetMaximum())
            ov2.GetYaxis().SetRangeUser(0, ymax)

            l = ROOT.TLegend(0.5, 0.65, 0.88, 0.88)
            l.SetBorderSize(0)
            l.SetFillColor(0)
            l.SetFillStyle(0)
            l.AddEntry(ov2, 'GENIE v2.12.10')
            l.AddEntry(ov3, 'GENIE v3.00.01')
            l.Draw()

        elif ov2.IsA() == ROOT.TH2F.Class():
            c.SetRightMargin(0.17)
            ov3.Divide(ov2)
            ov3.GetZaxis().SetTitle('Ratio GENIEv3/GENIEv2')
            ov3.Draw('colz')
            ov3.GetZaxis().SetRangeUser(0, 2)
            ov3.GetZaxis().SetTitleOffset(1.15)

        plotname = '_'.join(['cmp'] + k.GetName().split('_')[1:]) + '.pdf'
        c.SaveAs(plotname)

        del c

