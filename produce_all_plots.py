# Script to make GENIE validation plots and upload to https://microboone-sim.fnal.gov/
# Takes as input:
# - up to 5 input NUISANCE GenericVectors tree files (can be extended if desired -- will also require editing compare.py to accept more than 5 inputs)
# - a "legend title" for each of those input files (to be used in plot legends and the webpage organisation)
#
# Example usage:
# python produce_all_plots.py /genie/app/users/kduffy/t2k_fit_CC0pi/NoSTV_Noifk/Nov14/uBCCincPlots/NuisTrees/uB_all_MCC8Tune1_vectors.root "MCC8 Tune 1" /genie/app/users/kduffy/t2k_fit_CC0pi/NoSTV_Noifk/Nov14/uBCCincPlots/NuisTrees/uB_all_MCC9Nominal_vectors.root "MCC9 Nominal" /genie/app/users/kduffy/t2k_fit_CC0pi/NoSTV_Noifk/Nov14/uBCCincPlots/NuisTrees/uB_all_MCC9TunedAverage_vectors.root "MCC9 Tuned"
#
#python produce_all_plots.py /genie/app/users/kduffy/t2k_fit_CC0pi/NoSTV_Noifk/Nov14/uBCCincPlots/NuisTrees/uB_all_MCC9Nominal_vectors.root "MCC9 Nominal" /genie/app/users/kduffy/t2k_fit_CC0pi/NoSTV_Noifk/Nov14/uBCCincPlots/NuisTrees/uB_all_MCC9TunedAverage_vectors.root "MCC9 Tuned" /genie/app/users/kduffy/t2k_fit_CC0pi/NoSTV_Noifk/Nov14/uBCCincPlots/NuisTrees/uB_all_LowBE_vectors.root "Low BE Nominal" /genie/app/users/kduffy/t2k_fit_CC0pi/NoSTV_Noifk/Nov14/uBCCincPlots/NuisTrees/uB_all_LowBE_uBTune_vectors.root "Low BE Tuned"

import argparse
import os
import glob
import shutil
import websitehelper

# Parse arguments
parser = argparse.ArgumentParser(description='Create and upload a set of validation plots to the https://microboone-sim.fnal.gov website')
parser.add_argument('inputs',nargs='*',help='Input files and legend titles, in the format "input1.root legendtitle1.root input2.root legendtitle2 [input3.root legendtitle3 input4.root legendtitle4 input5.root legendtitle5]". You must provide a legend title corresponding to every input file. You must provide at least two input files with legend titles.')
parser.add_argument('-norm','--normalize',action='store_true',help='Plot area-normalized distributions (rather than absolutely normalized)')
parser.add_argument('-f','--force',action='store_true',help='Use to force creation of plot directories when that will involve overwriting existing plots. Use with care!')
args = parser.parse_args()

# Check that we have the right number of inputs
assert len(args.inputs) >= 4, "Did not provide enough inputs! Provide at least two input NUISANCE files and two legend titles: %s" % inputs

# Get a list of just the input .root files
input_rootfiles = []
input_legendtitles = []
for input in args.inputs:
    if ".root" in input:
        input_rootfiles.append(input)
    else:
        input_legendtitles.append(input)

assert len(input_rootfiles)*2 == len(args.inputs), "Input root files: %s, All inputs: %s"%(input_rootfiles,args.inputs)

###################################################################
#                  Stage 1: Make the plots                        #
###################################################################

# Build the app to make the plots
os.system('make plot_kinematics_nuistr')

# Make histograms for each individual thing we want to compare
for i in range(len(input_rootfiles)):
    cmd = './plot_kinematics_nuistr tmpfile_%d.root '%i +input_rootfiles[i]
    os.system(cmd)

# The command above made a lot of .png output files that we don't want to keep - delete them to stop filling up directories
os.system('rm *.png')

# Now run compare.py to produce overlay plots
cmd = 'python compare.py '+str(args.normalize)
for i in range(len(input_rootfiles)):
    cmd += ' tmpfile_%d.root '%i + '"'+input_legendtitles[i]+'"'
os.system(cmd)

# Clean up by deleting the .root files we made
for i in range(len(input_rootfiles)):
    cmd = 'rm tmpfile_%d.root'%i
    os.system(cmd)

###################################################################
#                Stage 2: Upload to the webpage                   #
###################################################################

# The legendtitle labels define the directory where the plots will be copied to. Check if the directory already exists. If it doesn't, make it. If it does, report an error and exit (unless the -f argument has been called, in which case carry on and overwrite everything in that directory)
sample = ""
for i in range(len(input_legendtitles)):
    if (i > 0):
        sample += '-'
    sample += '_'.join(input_legendtitles[i].split(' ')[0:])
directory = "/web/sites/m/microboone-sim.fnal.gov/htdocs/data/" + sample
if not os.path.exists(directory):
    print "Making directory " + directory
    os.makedirs(directory)
else:
    if not os.listdir(directory):
        print "Directory " + directory + " exists and is empty so will be used"
    else:
        if (args.force==True):
            print "[WARNING] Directory " + directory + " exists and is not empty. Overwriting contents..."
        else:
            print "[ERROR] Directory " + directory + " exists and is not empty. Will not overwrite contents. Please either 1) change your legend title labels, 2) remove the contents of this directory, or 3) run this script again with the -f flag to force overwrite of the contents of this directory (caution!)"
            exit(1)


# Now copy the plots
# Make different folders for different interaction channels and neutrino flavours, to (hopefully) make it easier to navigate. These channels/flavours are hardcoded here so will need to be changed by hand if we want the website to separate out others.
flavours = ['num','nue']
channels = ['ccqe','ccmec','ccres']
all_subchannels = []

plots = sorted(glob.glob('*.png'))
copied_plots = []
for plot in plots:
    for flavour in flavours:
        for channel in channels:
            subchannel = flavour + '_' + channel
            all_subchannels.append(subchannel)
            if flavour in plot and channel in plot:
                if not os.path.exists(directory+'/'+subchannel+'/'):
                    os.makedirs(directory+'/'+subchannel+'/')
                #print plot
                shutil.copy2(plot,directory+'/'+subchannel+'/')
                copied_plots.append(plot)



# Add "other" category!
# Check which plots have not been copied in the above loop, and put them in an "other" category
for plot in plots:
    if plot not in copied_plots:
        all_subchannels.append('other')
        if not os.path.exists(directory+'/other/'):
            os.makedirs(directory+'/other/')
        shutil.copy2(plot,directory+'/other/')

# Ok, now we have the plots in the right place, we need to make some webpages to display them!

# Make list of flavour/channel subdirectories

flavour_channel_subdirs = []
for tmp in os.listdir(directory):
    flavour_channel_subdirs.append(os.path.basename(os.path.normpath(tmp)))

# For flavour_channel_subdir.php
#  - Make flavour_channel_subdir.php to upload all plots in that directory

for subdir in flavour_channel_subdirs:
    websitehelper.make_html_allplots(sample,subdir)

# For sample.html:
#  - For all sample subdirectories, add a link to flavour_channel_subdir.php to sample.html

websitehelper.make_sample_html(sample,flavour_channel_subdirs)

#  3) Edit localincludes/nav-left.html to add a link on the left hand navigation bar to sample.html

websitehelper.add_to_nav_left(sample)
