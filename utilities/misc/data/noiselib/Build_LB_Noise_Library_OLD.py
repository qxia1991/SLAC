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
import numpy as np
import ROOT

def get_local_path(xpath):
    path, file_name = os.path.split( xpath ) #break into dir and basename
    file_tag = re.findall("\d{4}-\d{3}", file_name) #runs aved as rnum-filenum
    file_num = file_tag[0][:4] #runnumber (assume < run 10000)
    file_ext = file_tag[0][5:] #filenumber in run
    
    raw_path = ''
    file_num = str(file_num)
    #List of directories where data is stored
    dirlist = ('exo_data2','exo_data3','exo_data4','exo_data5','exo_data6')

    for df in dirlist:
        crp = "/nfs/slac/g/" + df + "/exo_data/data/WIPP/root/" + file_num + "/run0000" + file_num + "-" + file_ext + ".root"
        if(os.path.isfile(crp)):
            raw_path = crp
            break

    return raw_path

# Main function 
def main(): 
  #Define some constants up front
  threshold = 6.0
  nchannels = 226

  # LB run limits and file list
  num_solicited_triggers = 50000
  begin_run = 7103 #First 12kV 
  #end_run = 7120
  end_run = 7395   #9/8/2016 current most recent run
  NoiseFile = "/nfs/slac/g/exo-userdata/users/mjjewell/NoiseLibrary/LB_noise_library_runs_%i_%i.root" % (begin_run, end_run) 
  filterStr = 'run>=%i&&run<=%i&&runType=="Data-Physics"&&quality=="GOOD"' % (begin_run, end_run)

  golden_raw_LB_files = []
  golden_masked_LB_files = []

  # Gather file names for raw and masked golden LB data
  # need raw files for the WFs and masked for the parameters 
  # from proecessing.

  ds_raw = ROOT.EXORunInfoManager.GetDataSet("Data/Raw/root", filterStr)
  ds_masked = ROOT.EXORunInfoManager.GetDataSet("Data/Processed/masked", filterStr)
  
  for run in ds_raw:
    for raw_LB_file in run.GetRunFiles():
      #Don't Use the xrootd path use the real path 
      golden_raw_LB_files.append( get_local_path(raw_LB_file.GetFileLocation()) )
  
  for run in ds_masked:
    for masked_LB_file in run.GetRunFiles():
      #Could use same function but the dset should give real path for masked
      #golden_masked_LB_files.append( get_local_path(masked_LB_file.GetFileLocation()))
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
  
  masked_chain.Draw("fRunNumber:fEventNumber", selection_string, "goff")
    
  # The following is a list of tuples of (run, event)
  v1 = masked_chain.GetV1()
  v2 = masked_chain.GetV2()
  run_solicited_events = [(v1[i], v2[i]) for i in range(masked_chain.GetSelectedRows())]

  # Chain raw files together
  raw_chain = ROOT.TChain("tree")
  map(lambda x: raw_chain.Add(x), golden_raw_LB_files)
 
  # Build index versus run, event number
  raw_chain.BuildIndex("fRunNumber", "fEventNumber")

  # Shuffle them around randomly
  random.shuffle(run_solicited_events)
  
  # Check waveforms
  events_for_use = [] #Good events
  num_pileup_signals = 0 
  num_good = 0 
  # Loop over events and reject any event where
  # the maximum is greater than 6*RMS_noise which will cut
  # large noise pulses as well as actual "events"
  for entry in run_solicited_events:
    signal_found = False

    #Get solicited trigger event WF data from raw file
    raw_chain.GetEntryWithIndex(int(entry[0]), int(entry[1])) 
    raw_ED = raw_chain.EventBranch
    wf_data = raw_ED.GetWaveformData()
    wf_data.Decompress()
    
    if int(wf_data.GetNumWaveforms()) != nchannels: 
        #All 226 channel need to exist
        continue
    
    for i in range(wf_data.GetNumWaveforms()):
      wf = wf_data.GetWaveform(i)
      wf_arr = np.array(wf)
      baseline = np.median(wf_arr) 
      wf_arr = np.subtract(wf_arr, baseline) #Baseline subtract WF
      noise = np.std(wf_arr)
      max_height = np.max(wf_arr) #Max value

      if max_height/noise > threshold: 
        #Very large max reject as "pileup"
        num_pileup_signals += 1
        signal_found = True
        print("Run %i, event %i" % (entry[0], entry[1]))
        print(" signal found on wavefrom %i with significance %f [rms]" % (i, max_height/noise))
        break

    if signal_found: continue
    
    events_for_use.append(entry)
    num_good+=1
    #if have enough than break
    if num_good >= num_solicited_triggers: break
    if num_good%50==0: print "--------------------------------%i Events found------------------------------------------"%num_good

  print("Found %i pileup events out of %i total solicited events." % (num_pileup_signals, len(run_solicited_events)))

  # Shuffle them around randomly
  random.shuffle(events_for_use)

  # Now get all the entry numbers, this just calls el.Enter(x), for all x in the list)
  el = ROOT.TEventList()
  map(el.Enter, [raw_chain.GetEntryNumberWithIndex(int(entry[0]), int(entry[1])) for entry in events_for_use])
  raw_chain.SetEventList(el)
  
  # Open LB noise root file (libary)
  noise_library = ROOT.TFile(NoiseFile, "RECREATE")

  # Ignore errors
  old_err = ROOT.gErrorIgnoreLevel
  ROOT.gErrorIgnoreLevel = ROOT.kError + 1
 
  # This will only copy the selected entries
  new_tree = raw_chain.CopyTree("")
  ROOT.gErrorIgnoreLevel = old_err

  print("Number of solicited trigger events selected for storage: %i" % new_tree.GetEntries()) 
  #print("Number of solicited trigger events selected for storage: %i" % counter) 

  noise_library.cd() # shouldn't be necessary
  new_tree.Write()
 
  noise_library.Close()

# Run script standalone
if __name__ == "__main__":

  if ROOT.gSystem.Load("libEXOROOT") < 0: sys.exit('Failed to load EXOROOT.')
  if ROOT.gSystem.Load("libEXOUtilities") < 0: sys.exit('Failed to load EXOUtilities')
  
  main()
