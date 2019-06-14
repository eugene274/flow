#!/bin/sh

inFile=$1

#build q-vectors

#no corrections
analysis/correct $inFile nothing
mv output.root output_0.root

#recentering
analysis/correct $inFile qn.root
mv output.root output_1.root

#twist and rescale
analysis/correct $inFile qn.root
mv output.root output_2.root

#correlate q-vecors from desired correction step
ls output_2.root > list
analysis/analysis list
mv corr.root corr_2.root 

#work with correlations
root -l '../macro/save_graphs.C ("corr_2.root")'