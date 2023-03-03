#!/bin/bash
Tune1_DIR="/eos/project-n/neutrino-generators/generatorOutput/GENIE/DIRT_II_Studies/groundStateValidations/fixed2p2h_params/GENIE_3_02_00_myFork/"
Tune2_DIR="/eos/project-n/neutrino-generators/generatorOutput/GENIE/DIRT_II_Studies/groundStateValidations/GENIEtune/GENIE_3_02_00_myFork/"

PLOT_DIR_Tune1="output/Plots_00_000_030323/"
PLOT_DIR_Tune2="output/Plots_02_11b_030323/"

vec="/flat_vec_"
TUNE_1_a="D22_22a_00_000"
TUNE_1_n="D22_22n_00_000"
TUNE_2_a="D22_22a_02_11b"
TUNE_2_n="D22_22n_02_11b"

GDIRT="_3_02_00_myFork_*.root"

TARGET="1000010010 1000060120 1000180400"
NU="12 -12 14 -14 16 -16"

mkdir $PLOT_DIR_Tune1
mkdir $PLOT_DIR_Tune2

for target in ${TARGET}; do
	for nu in ${NU}; do
		echo 
		./bin/plot_kinematics $PLOT_DIR_Tune1/Plots_DIRT_D22_22a_${target}_${nu}.root ${Tune1_DIR}${TUNE_1_a}_${target}${vec}${TUNE_1_a}_${nu}_${target}${GDIRT}
		./bin/plot_kinematics $PLOT_DIR_Tune1/Plots_DIRT_D22_22n_${target}_${nu}.root ${Tune1_DIR}${TUNE_1_n}_${target}${vec}${TUNE_1_n}_${nu}_${target}${GDIRT}
		
		./bin/plot_kinematics $PLOT_DIR_Tune2/Plots_02_11b_DIRT_D22_22a_${target}_${nu}.root ${Tune2_DIR}${TUNE_2_a}_${target}${vec}${TUNE_2_a}_${nu}_${target}${GDIRT}
		./bin/plot_kinematics $PLOT_DIR_Tune2/Plots_02_11b_DIRT_D22_22n_${target}_${nu}.root ${Tune2_DIR}${TUNE_2_n}_${target}${vec}${TUNE_2_n}_${nu}_${target}${GDIRT}
		
	done
done
