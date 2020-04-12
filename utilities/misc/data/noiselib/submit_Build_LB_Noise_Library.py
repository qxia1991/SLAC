# ************************************************************************************* #
#                                                                                       #
# Program Build_LB_Noise_Library.py                                                     #
# Description: program to build library of LB noise profiles from solicited LB events   #
#              1) Randomizes the ordering so selecting random number of traces from a   #  
#                  random times in each run to eliminate sampling a sustained burst of  #
#                  noise.                                                               #
#               2) Rejects pile up events above some threshold.  Threshold defined as   #
#                  a limit on the ratio of max_value/rms_noise.                         #
#                                                                                       #
#                                                                                       #
# Usage: python Build_LB_Noise_Libray.py                                                #
#                                                                                       #
#                                                                                       #
# Date Created: 1/20/2014                                                               #
# Author: Josiah Walton                                                                 #
# Institution: University of Illinois at Urbana-Champaign                               # 
#                                                                                       #
# 3/29/2014: Added 226 waveforms cut and a simple signal finder to search for pileup    #
# (the coincidence of a real signal with the solicited trigger).                        #           
#                                                                                       #
# 9/8/2016: Adapted to run on Phase 2 data (12kV, post upgrade)                         #                                                             
# ************************************************************************************* #
 
import os, re, sys, glob, math, random
import subprocess
import numpy as np
import ROOT
import time

running_jobs = 100

#Function to pause and hold while waiting for submitted jobs to finish up
def Hold(proc):
    print "Holding", len(proc)
    while len(proc) > running_jobs:
        map(lambda x: x[0].poll(), proc)
        finishedProcs = filter(lambda x: x[0].returncode != None, proc)
        for p in finishedProcs:
            proc.remove(p)
            if p[0].returncode != 0: print "Run %s %s failed" % (p[1],p[2])
            else: print  "Run  %s  %s sucess" % (p[1],p[2])
        time.sleep(20) #Need this or loops to quickly
    print "Done Holding", len(proc)
    return proc
#-----------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------


# Main function 
def main(): 
    #Define some constants up front
    threshold = 6.0
    nchannels = 226

    # LB run limits and file list
    num_solicited_triggers = 50000
    begin_run = 7103 #First 12kV 
    end_run = 7395   #9/8/2016 current most recent run
    #end_run = 7110
    #NoiseFile = "/nfs/slac/g/exo-userdata/users/mjjewell/NoiseLibrary/LB_noise_library_runs_%i_%i.root" % (begin_run, end_run) 
    filterStr = 'run>=%i&&run<=%i&&runType=="Data-Physics"&&quality=="GOOD"' % (begin_run, end_run)
    exe = "./run_Build_Noise_Library.sh"

    golden_masked_LB_files = []
    golden_masked_LB_rn = []

    # Gather file names for masked golden LB data
    ds_masked = ROOT.EXORunInfoManager.GetDataSet("Data/Processed/masked", filterStr)
    for run in ds_masked:
        golden_masked_LB_rn.append(run.GetRunNumber())
        for masked_LB_file in run.GetRunFiles():
            golden_masked_LB_files.append(masked_LB_file.GetFileLocation())

    # Chain masked files together
    masked_chain = ROOT.TChain("tree")
    for f in golden_masked_LB_files:
        print "Adding %s" % f
        masked_chain.Add(f)

    # Set length of the internal arrays, can't be longer than the total number
    masked_chain.SetEstimate(masked_chain.GetEntries())
  
    # Perform Draw command, the "goff" says not to plot.  This stores a list of run and event numbers for those events that satisfy the selection cut
    selection_string = "(!fEventHeader.fIndividualTriggerRequest) && (!fEventHeader.fSumTriggerRequest)" # Solicited trigger
    selection_string += " && (fEventHeader.fSampleCount==2047)" # 2048 samples in waveforms
    # Also require no charge clusters. This introduces some small bias against
    # large noise events, but primarily rejects pileup
    selection_string += " && (@fChargeClusters.size() == 0)"
  
    masked_chain.Draw("fRunNumber", selection_string, "goff")
    
    # The following is a list of tuples of (run, event)
    v1 = masked_chain.GetV1()
    run_solicited_events = np.array([int(v1[i]) for i in range(masked_chain.GetSelectedRows())])
    random.shuffle(run_solicited_events)
  
    bc = np.bincount(run_solicited_events[0:num_solicited_triggers])
    print "Size of counter is", len(bc)
    
    proc = []
    for rn in golden_masked_LB_rn:
        if  rn >= len(bc):
            #Run not used
            nevents = 0
        else:
            nevents = bc[int(rn)]
      
        print "Add", rn, nevents
      
        #Check and Remove Done ones
        proc = Hold(proc)

        #Submit and add to process list
        proc.append((subprocess.Popen(['bsub','-q','long','-W', '0:30', '-R', 'rhel60', '-K', 
                                       exe, str(rn), str(nevents)]), rn, nevents))


    #Submission is done now loop over the processes still running and wait for each one to finish up
    print "Done submit now just wait"
    for p in proc:
        print "Waiting on", p[1], p[2]
        p[0].wait()
        if p[0].returncode != 0: print "Run %s %s failed" % (p[1], p[2])
        else: print  "Run %s %s sucess" % (p[1],p[2])
    print "Done."

   
   
    

# Run script standalone
if __name__ == "__main__":

  if ROOT.gSystem.Load("libEXOROOT") < 0: sys.exit('Failed to load EXOROOT.')
  if ROOT.gSystem.Load("libEXOUtilities") < 0: sys.exit('Failed to load EXOUtilities')
  
  main()
