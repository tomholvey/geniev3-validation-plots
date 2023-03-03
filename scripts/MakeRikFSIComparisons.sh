#!/bin/bash

DIR_3_02_00_10a="/afs/cern.ch/user/t/tholvey/public/GENIE_Plots/geniev3-validation-plots/Plots_3_02_00_RikFSI/Plots_3_02_00_G18_10a_"
DIR_3_02_00_10n="/afs/cern.ch/user/t/tholvey/public/GENIE_Plots/geniev3-validation-plots/Plots_3_02_00_RikFSI/Plots_3_02_00_G18_10n_"
DIR_DIRT_22a="/afs/cern.ch/user/t/tholvey/public/GENIE_Plots/geniev3-validation-plots/Plots_DIRT_RikFSI/Plots_DIRT_G18_10a_"
DIR_DIRT_22n="/afs/cern.ch/user/t/tholvey/public/GENIE_Plots/geniev3-validation-plots/Plots_DIRT_RikFSI/Plots_DIRT_G18_10n_"

# DIR_* + TARGET + "_" + flavour 

TARGET="1000010010 1000060120 1000180400"
NUFLAVOUR="_12 _-12 _14 _-14 _16 _-16"
root=".root"
OUTDIR="Validations_RikFSI_withAndWithoutFSI_20.01.23"
wc="*"

for targ in ${TARGET}; do
	mkdir ${OUTDIR}${targ}
	for flav in ${NUFLAVOUR}; do
		python compare_with_withoutFSI.py ${DIR_3_02_00_10a}${targ}${flav}${root} "3.02.00 G18_10a" ${DIR_3_02_00_10n}${targ}${flav}${root} "3.02.00 G18_10n" ${DIR_DIRT_22a}${targ}${flav}${root} "Rik's FSI Changes" ${DIR_DIRT_22n}${targ}${flav}${root} "Rik's FSI Changes (with no FSI)"
		#python compare.py ${DIR_3_02_00_10a}${targ}${flav}${root} "3.02.00 G18_10a" ${DIR_DIRT_22a}${targ}${flav}${root} "Rik's FSI Changes"
		
		pdfunite 1DComp_${wc}${flav}_ccqe${wc}.pdf ${targ}${flav}_1D_CCQE_AllTunes.pdf
		pdfunite 1DComp_${wc}${flav}_ccmec${wc}.pdf ${targ}${flav}_1D_CCMEC_AllTunes.pdf
		pdfunite 1DComp_${wc}${flav}_ccres${wc}.pdf ${targ}${flav}_1D_CCRES_AllTunes.pdf
		pdfunite 1DComp_${wc}${flav}_ccdis${wc}.pdf ${targ}${flav}_1D_CCDIS_AllTunes.pdf
		pdfunite 1DComp_${wc}${flav}_nc${wc}.pdf ${targ}${flav}_1D_NC_AllTunes.pdf

		pdfunite 2D_${wc}${flav}_ccqe${wc}.pdf ${targ}${flav}_2D_CCQE_AllTunes.pdf
		pdfunite 2D_${wc}${flav}_ccmec${wc}.pdf ${targ}${flav}_2D_CCMEC_AllTunes.pdf
		pdfunite 2D_${wc}${flav}_ccres${wc}.pdf ${targ}${flav}_2D_CCRES_AllTunes.pdf
		pdfunite 2D_${wc}${flav}_ccdis${wc}.pdf ${targ}${flav}_2D_CCDIS_AllTunes.pdf
		pdfunite 2D_${wc}${flav}_nc${wc}.pdf ${targ}${flav}_2D_NC_AllTunes.pdf
	
		mv ${targ}${flav}${wc} ${OUTDIR}${targ}
		
		rm 1D$wc 2D$wc 

	done
done
