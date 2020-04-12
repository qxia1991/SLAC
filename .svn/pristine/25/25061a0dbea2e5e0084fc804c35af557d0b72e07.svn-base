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
    dirlist = ('exo_data2','exo_data3','exo_data4','exo_data5','exo_data6','exo_data7','exo_data8')

    for df in dirlist:
        crp = "/nfs/slac/g/" + df + "/exo_data/data/WIPP/root/" + file_num + "/run0000" + file_num + "-" + file_ext + ".root"
        if(os.path.isfile(crp)):
            raw_path = crp
            break

    return raw_path
    
def pad2png(wf, filename):

	c1 = ROOT.TCanvas()
	wfh = wf.GimmeHist().Draw()
	c1.SaveAs(filename)

def BaseLine(wf,avgover):
	return (wf.Sum(0,avgover))/avgover

def MakeGlitchTempArray(ch, evlist):
	#File = ROOT.TFile(file)
	#tree = File.Get("tree")
	
	glitchwfarray = np.empty(226, ROOT.EXODoubleWaveform)
	
	#loop over events piling the wf's into glitchwfarray by channel
	for ev in evlist:
		#print "start for " + str(ev)
		ch.GetEntryWithIndex(int(ev[0]),int(ev[1]))
		ed = ch.EventBranch
		wfdata = ed.GetWaveformData()
		wfdata.Decompress()
		
		if ev==evlist[0]:
			for i in range(226):
				wf = wfdata.GetWaveformWithChannelToEdit(i)
				dwf = ROOT.EXODoubleWaveform(wf)
				glitchwfarray[i] = dwf
				
			#pad2png(glitchwfarray[0],"origwf.png")
				
		else:
			for j in range(226):
				wf = wfdata.GetWaveformWithChannelToEdit(j)
				dwf = ROOT.EXODoubleWaveform(wf)
				glitchwfarray[j] += dwf
				
	for k in range(226):
		glitchwfarray[k] /= len(evlist)  #averaged over the num of events
		bl = BaseLine(glitchwfarray[k],500) #subtract the baseline for a "base template"
		#print "bl for " + str(k) + " is " + str(bl)
		glitchwfarray[k] -= bl
	
	#pad2png(glitchwfarray[0],"GlitchTempCheck.png")
	
	return glitchwfarray

def Integerize(dtwfarray):
	
	intglitchwfarray = np.empty(226, ROOT.EXOWaveform)
	
	randwfarray = [np.random.uniform(0,1,2048) for x in range(226)]
	
	for i in range(len(dtwfarray)):
		dtwf = dtwfarray[i]
		randwf = randwfarray[i]
		itwf = ROOT.EXOWaveform()
		h = dtwf.GimmeHist()
		l = h.GetNbinsX()
		itwf.SetLength(l)
		
		for k in range(dtwf.GetLength()):
			data = dtwf[k]
			fracpart, intpart = math.modf(data)
			r = randwf[k]
			#print "intpart = " + str(intpart) + ", fracpart = " + str(fracpart) + "\n random number = " +str(r)
			
			if r < math.fabs(fracpart) : itwf[k] = intpart-1 if data<0 else intpart+1
			else: itwf[k] = intpart
		
		intglitchwfarray[i] = itwf
		
	return intglitchwfarray

# Main function 
def main(rn, nevents, outdir): 
    #Define some constants up front
    threshold = 6.0
    nchannels = 226

    # LB run limits and file list
    NoiseFile = os.path.join(outdir, "LB_noise_library_run%i.root" % rn)

    golden_raw_LB_files = []
    golden_masked_LB_files = []

    ri_raw = ROOT.EXORunInfoManager.GetRunInfo(rn, "Data/Raw/root")
    ri_mask = ROOT.EXORunInfoManager.GetRunInfo(rn, "Data/Processed/masked")
    
    for raw_LB_file in ri_raw.GetRunFiles():
        #Don't Use the xrootd path use the real path 
		golden_raw_LB_files.append( get_local_path(raw_LB_file.GetFileLocation()) )
		print raw_LB_file.GetFileLocation()
  
    for masked_LB_file in ri_mask.GetRunFiles():
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
 
    # Build index event number
    raw_chain.BuildIndex("fRunNumber", "fEventNumber")
    
    # Make a Double WF Array for the noise glitch templates
    double_glitch_array = MakeGlitchTempArray(raw_chain, run_solicited_events)
    #print "made double glitch array"
	

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
        
                print("Event %i" % (entry[1]))
                print(" signal found on wavefrom %i with significance %f [rms]" % (i, max_height/noise))
                break

        if signal_found: continue
        
        events_for_use.append(entry)
        num_good+=1
        #if have enough than break
        if num_good >= nevents: break

    print("Found %i pileup events out of %i total solicited events." % (num_pileup_signals, len(run_solicited_events)))
    
    # Shuffle them around randomly
    random.shuffle(events_for_use)

    # Ignore errors
    old_err = ROOT.gErrorIgnoreLevel
    ROOT.gErrorIgnoreLevel = ROOT.kError + 1

    # This will only copy the selected entries
    new_tree = raw_chain.CloneTree(0)
    ROOT.gErrorIgnoreLevel = old_err

    # Now get all the entry numbers, this just calls el.Enter(x), for all x in the list)
    el = ROOT.TEventList()
    map(el.Enter, [raw_chain.GetEntryNumberWithIndex(int(entry[0]), int(entry[1])) for entry in events_for_use])
    raw_chain.SetEventList(el)
        
    # Open LB noise root file (libary)
    noise_library = ROOT.TFile(NoiseFile, "RECREATE")

 
    print("Number of solicited trigger events selected for storage: %i" % new_tree.GetEntries()) 
    #print("Number of solicited trigger events selected for storage: %i" % counter) 

    
    #Now that we have the noise events, subtract the glitch from each waveform
    for event in range(el.GetN()):
        raw_chain.GetEntry(el.GetEntry(event))
        ed = raw_chain.EventBranch
        new_tree.SetBranchAddress("EventBranch", ed)
        wfdata = ed.GetWaveformData()
        wfdata.Decompress()

        #Integerize the double_glitch_array for each event
        int_glitch_array = Integerize(double_glitch_array)
        #if event == 0: pad2png(int_glitch_array[0],"IntGlitchWfFirstEvent.png")
        
        #loop through each channel and subtract the glitch template from the corresponding channel in int_glitch_array
        for channel in range(wfdata.GetNumWaveforms()):
            wf = wfdata.GetWaveformWithChannelToEdit(channel)
            #if event == 0 and channel == 0: pad2png(wf,"WaveformBeforeSubtraction.png")
            #wf += 500  ## just test the saving with something obvious
            wf -= int_glitch_array[channel]
            #if event == 0 and channel == 0: pad2png(wf,"WaveformAfterSubtraction.png")
            
        wfdata.Compress()
        new_tree.Fill()

        #print "Subtraction for event " + str(event)
		
    #print "finished subtraction for all events"

    noise_library.cd() # shouldn't be necessary
    
    #re-access wf for check
    #new_tree.GetEntry(0)
    #ed = new_tree.EventBranch
    #wfdata = ed.GetWaveformData()
    #wfdata.Decompress()
    #wf = wfdata.GetWaveformWithChannel(0)
    #pad2png(wf,"ReAccessTreeForFinalWF.png")
    
    new_tree.Write()
 
    noise_library.Close()
    print "Create Noise File:", NoiseFile

# Run script standalone
if __name__ == "__main__":

    if ROOT.gSystem.Load("libEXOROOT") < 0: sys.exit('Failed to load EXOROOT.')
    if ROOT.gSystem.Load("libEXOUtilities") < 0: sys.exit('Failed to load EXOUtilities')
  
  
    if len(sys.argv) < 4:
        print "arguments: [run number] [events to use from run]"
        sys.exit(1)
  
    rn = int(sys.argv[1])
    nevents = int(sys.argv[2])
    outdir = sys.argv[3]
    print "Process %i events for  run %i " % (nevents, rn)
    main(rn, nevents, outdir)



