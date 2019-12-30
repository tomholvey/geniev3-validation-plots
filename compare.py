# Make comparison plots from input files
# Note: currently does up to 5 input files. Can be extended to do more if desired
#
# A. Mastbaum <mastbaum@uchicago.edu>, 2018/12/20

import sys
import ROOT

color = [ROOT.kRed,ROOT.kBlue,ROOT.kOrange-3,ROOT.kMagenta+2,ROOT.kGreen+2]
style = [1,9,2,3,4]
normalize = False

if __name__ == '__main__':
    if len(sys.argv) < 5:
        print('Usage: %s normalize (true/false) input1.root legendtitle1 input2.root legendtitle2 [input3.root legendtitle3 input4.root legendtitle4 input5.root legendtitle5]' % sys.argv[0])
        sys.exit(0)

    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPalette(ROOT.kBird)
    ROOT.gROOT.SetBatch(True)

    normalize = sys.argv[1]
    print(normalize)

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
            assert(ov[i])
            ov[i].SetLineColor(color[i])
            ov[i].SetLineWidth(2)
            ov[i].SetLineStyle(style[i])

            if normalize and ov[i].Integral()!=0:
                # print('normalizing')
                ov[i].Scale(1.0/ov[i].Integral())


        c = ROOT.TCanvas('c', '', 500, 500)
        c.SetLeftMargin(0.15)

        if ov[0].IsA() == ROOT.TH1F.Class():
            ov[0].Draw('e1')
            for i in range(len(ov)):
                ov[i].Draw('e1 same')

            ymax = ov[0].GetMaximum()
            for i in range(len(ov)):
                if ov[i].GetMaximum() > ymax:
                    ymax = ov[i].GetMaximum()
            ymax *= 1.1

            ov[0].GetYaxis().SetRangeUser(0, ymax)

            l = ROOT.TLegend(0.5, 0.65, 0.88, 0.88)
            l.SetBorderSize(0)
            l.SetFillColor(0)
            l.SetFillStyle(0)
            for i in range(len(ov)):
                l.AddEntry(ov[i],legendtitle[i])
            l.Draw()

            plotname = '_'.join(['cmp'] + k.GetName().split('_')[1:]) + '.pdf'
            c.SaveAs(plotname)

        elif ov[0].IsA() == ROOT.TH2F.Class():
            c.SetRightMargin(0.17)
            for i in range(len(ov)):
                ov[i].Divide(ov[0])
                ov[i].GetZaxis().SetTitle('Ratio'+legendtitle[i]+'/'+legendtitle[0])
                ov[i].Draw('colz')
                ov[i].GetZaxis().SetRangeUser(0,2)
                ov[i].GetZaxis().SetTitleOffset(1.15)

                plotname = '_'.join(['cmp'] + k.GetName().split('_')[1:]) + '.pdf'
                c.SaveAs(plotname)

        del c
