# Script to make GENIE validation plots and upload to https://microboone-sim.fnal.gov/
# Takes as input:
# - up to 5 input NUISANCE GenericVectors tree files (can be extended if desired -- will also require editing compare.py to accept more than 5 inputs)
# - a "legend title" for each of those input files (to be used in plot legends and the webpage organisation)

import argparse
import os
import glob
import shutil
import validationwebhelper

# Parse arguments
parser = argparse.ArgumentParser(description='Create and upload a set of validation plots to the https://microboone-sim.fnal.gov website')
parser.add_argument('inputs',nargs='*',help='Input files and legend titles, in the format "input1.root legendtitle1.root input2.root legendtitle2 [input3.root legendtitle3 input4.root legendtitle4 input5.root legendtitle5]". You must provide a legend title corresponding to every input file. You must provide at least two input files with legend titles.')
parser.add_argument('-norm','--normalize',action='store_true',help='Plot area-normalized distributions (rather than absolutely normalized)')
args = parser.parse_args()

# Check that we have the right number of inputs
assert len(args.inputs) >= 4, "Did not provide enough inputs! Provide at least two input NUISANCE files and two legend titles: %s" % inputs

# Get a list of just the input .root files
input_rootfiles = []
for input in args.inputs:
    if ".root" in args.inputs:
        input_rootfiles.append(input)

assert len(input_rootfiles)*2 = len(args.inputs)

###################################################################
#                  Stage 1: Make the plots                        #
###################################################################

os.system('make plot_kinematics_nuistr')

for i in range(len(input_rootfiles)):
    outname = 'tmpfile_'+i+'.root'
    cmd = './plot_kinematics_nuistr tmpfile_'+i+'.root '+input_rootfiles[i]
    os.system(cmd)


###################################################################
#                Stage 2: Upload to the webpage                   #
###################################################################

# The version and sample labels define the directory where the plots will be copied to. Check if the directory already exists. If it doesn't, make it. If it does, report an error and exit (unless the -f argument has been called, in which case carry on and overwrite everything in that directory)
# directory = "/web/sites/m/microboone-sim.fnal.gov/data/" + args.version + "/" + args.sample
# if not os.path.exists(directory):
#     print "Making directory " + directory
#     os.makedirs(directory)
# else:
#     if not os.listdir(directory):
#         print "Directory " + directory + " exists and is empty so will be used"
#     else:
#         if (args.force==True):
#             print "[WARNING] Directory " + directory + " exists and is not empty. Overwriting contents..."
#         else:
#             print "[ERROR] Directory " + directory + " exists and is not empty. Will not overwrite contents. Please either 1) change your version or sample labels, 2) remove the contents of this directory, or 3) run this script again with the -f flag to force overwrite of the contents of this directory (caution!)"
#             exit(1)
#
#
# # Now copy the plots
# # Make different folders for different reconstruction algorithms, to (hopefully) make it easier to navigate. These algorithms are hardcoded here so will need to be changed by hand if we want the CI to monitor others. Note that order matters (because of the breaks)!
# algoNames = ['pandora_pandoraNu','pandora_pandoraCosmic','pmtrack', 'showerrecopandora', 'pandoraNuKHit', 'pandoraNuKalmanTrack', 'pandoraNuKalmanShower', 'pandoraKalmanTrack', 'pandoraKalmanShower', 'pandoraTrack', 'pandoraShower', 'pandoraNuPMA', 'pandoraNu', 'pandoraCosmicKHit', 'pandoraCosmicKalmanTrack', 'pandoraCosmic', 'pandora', 'simpleFlashBeam', 'simpleFlashCosmic', 'opflashBeam', 'opflashCosmic', 'hit', 'other']
#
# plots = sorted(glob.glob(args.plotdir+'/*.jpg')) + sorted(glob.glob(args.plotdir+'/*.png')) + sorted(glob.glob(args.plotdir+'/*.gif')) + sorted(glob.glob(args.plotdir+'/*.pdf'))
# for plot in plots:
#     for algoName in algoNames:
#         if algoName in plot:
#             if not os.path.exists(directory+'/'+algoName+'/'):
#                 os.makedirs(directory+'/'+algoName+'/')
#             #print plot
#             shutil.copy2(plot,directory+'/'+algoName+'/')
#             break
#         # Add "other" category!
#         else:
#             if algoName=='other':
#                 # If it makes it this far in the loop, the plot does not match any of the given algo names. Put it in an "other" directory
#                 if not os.path.exists(directory+'/'+algoName+'/'):
#                     os.makedirs(directory+'/'+algoName+'/')
#                 shutil.copy2(plot,directory+'/'+algoName+'/')
#             continue
#         break
#
#
# #print (os.listdir(directory))
#
# # Ok, now we have the plots in the right place, we need to make some webpages to display them!
#
# # 1) Make list of version subdirectories (i.e. samples)
# # 2) Make list of sample subdirectories
#
# version_dir = os.path.dirname(directory)
# version_subdirs_fullpath = sorted(glob.glob(version_dir+'/*/'))
#
# version_sample_dirs = []
# version_sample_subdirs = []
# for tmp in version_subdirs_fullpath:
#     sample_subdirs = []
#     sample_subdirs_fullpath = sorted(glob.glob(tmp+'/*/'))
#     for tmp2 in sample_subdirs_fullpath:
#         sample_subdirs.append(os.path.basename(os.path.normpath(tmp2)))
#     version_sample_dirs.append(os.path.basename(os.path.normpath(tmp)))
#     version_sample_subdirs.append(sample_subdirs)
#
# #print version_sample_dirs
# #print version_sample_subdirs
#
# # For version_sample_subdir.php
# #  1) Make version_sample_subdir.php to upload all plots in that directory
#
# for i_sample in range(0,len(version_sample_dirs)):
#     for i_subdir in range(0,len(version_sample_subdirs[i_sample])):
#         sample = version_sample_dirs[i_sample]
#         subdir = version_sample_subdirs[i_sample][i_subdir]
#         validationwebhelper.make_html_allplots(version_dir+'/'+sample+'/'+subdir, args.version, sample, subdir)
#
# # For version.html:
# #  1) For all samples, add a sample header to version.html
# #  2) For all sample subdirectories, add a link to version_sample_subdir.php to version.html
#
# validationwebhelper.make_version_html(args.version, version_sample_dirs, version_sample_subdirs)
#
# #  3) Edit localincludes/nav-left.html to add a link on the left hand navigation bar to version.html
#
# validationwebhelper.add_to_nav_left(args.version)
