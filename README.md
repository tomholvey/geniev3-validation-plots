GENIE v2/v3 Comparison Plots
============================
A. Mastbaum <mastbaum@uchicago.edu>, 2018/12

These scripts generate plots of kinematic distributions from art ROOT files
and overlay them, for validation of generator and reweighting output in
moving from GENIE v2.12.10 to v3.00.02.

Building
--------
To build, set up an environment for `uboonecode` `v08_00_00`, and then run

    $ make

This will build the `plot_kinematics` binary.

Plotter
-------
The main script is `plot_kinematics`, which reads MCTruth information from
an art ROOT file and builds many plots. The plots are output to PDFs and also
written to a ROOT file.

Usage:

    $ ./plot_kinematics OUTPUT.root INPUT1.root [INPUT2.root ...]

### Extending the Plotter

There are two main objects used in plot generation: *filters* and
*distributions*. A filter takes a `simb::MCTruth` and returns a boolean
indicating whether the event meets its criteria. For example, a filter
might select numu CCQE events with one proton above 30 MeV. A distribution
sets up a histogram and has a `Fill` function that knows how to populate
it given a `simb::MCTruth`. An example might be a Q^2 distribution, which
reads `MCTruth::GetNeutrino().QSqr()`.

In `plot_kinematics`, a set of distributions is constructed subject to
different filters. For instance, a q0/q3 plot with a numuCCQE filter
applied and one with a numuCCMEC filter applied. In this way, we build up the
set of plots relevant for each interaction mode.

Overlays
--------
A Python3 script, `compare.py` reads in the histograms from the ROOT files
output by `plot_kinematics` and draws them for comparison. In the case of
1D histograms, they are plotted together in different colors. For 2D
histograms, the ratio is plotted. The script will output a comparison plot
for every plot it finds within the first input file.

