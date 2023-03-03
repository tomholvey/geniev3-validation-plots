#!/bin/bash

DIR_TUNE_1_a="/afs/cern.ch/user/t/tholvey/public/GENIE_Plots/geniev3-validation-plots/output/Plots_00_000_030323/Plots_DIRT_D22_22a_"
DIR_TUNE_1_n="/afs/cern.ch/user/t/tholvey/public/GENIE_Plots/geniev3-validation-plots/output/Plots_00_000_030323/Plots_DIRT_D22_22n_"
DIR_TUNE_2_a="/afs/cern.ch/user/t/tholvey/public/GENIE_Plots/geniev3-validation-plots/output/Plots_02_11b_030323/Plots_02_11b_DIRT_D22_22a_"
DIR_TUNE_2_n="/afs/cern.ch/user/t/tholvey/public/GENIE_Plots/geniev3-validation-plots/output/Plots_02_11b_030323/Plots_02_11b_DIRT_D22_22n_"

# DIR_* + TARGET + "_" + flavour 

TARGET="1000010010 1000060120 1000180400"
NUFLAVOUR="_12 _-12 _14 _-14 _16 _-16"
root=".root"
OUTDIR="Validations_Final_030323"
wc="*"

for targ in ${TARGET}; do
	mkdir ${OUTDIR}_${targ}
	mkdir ${OUTDIR}_${targ}/ROOT/
	for flav in ${NUFLAVOUR}; do
		python utils/compare_with_withoutFSI.py ${DIR_TUNE_1_a}${targ}${flav}${root} "D22_22a_00_000" ${DIR_TUNE_1_n}${targ}${flav}${root} "D22_22n_00_000" ${DIR_TUNE_2_a}${targ}${flav}${root} "D22_22a_02_11b" ${DIR_TUNE_2_n}${targ}${flav}${root} "D22_22n_02_11b"
		
		mv 1DComp_${wc}${root} ${OUTDIR}_${targ}/ROOT
		mv 2D_${wc}${root} ${OUTDIR}_${targ}/ROOT

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
	
		mv ${targ}${flav}${wc} ${OUTDIR}_${targ}
		
		rm 1D$wc 2D$wc 

	done
done
