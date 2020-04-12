import ROOT
import numpy as np

#tfile = ROOT.TFile("/nfs/slac/g/exo-userdata/users/mjjewell/NoiseLibrary/LB_noise_library_runs_7103_7120.root")
tree = ROOT.TChain("tree")
#tree.Add("/nfs/slac/g/exo-userdata/users/mjjewell/NoiseLibrary_12k/LB_noise_library_run*.root")
tree.Add("/nfs/slac/g/exo-userdata/users/mjjewell/NoiseLibrary_50k/NoiseLibrary_Phase2_7106_7393_50k.root")
#tree = tfile.Get("tree")

ch_list = [int(20),int(220)]

c1 = ROOT.TCanvas("c1")

print "Total Entries", tree.GetEntries()
raw_input("Enter")

noise = [0,0]
run = []

for i in np.arange(tree.GetEntries()):
    tree.GetEntry(int(i))
    ed = tree.EventBranch
    
    print ed.fRunNumber, ed.fEventNumber
    run.append(ed.fRunNumber)

    wf_data = ed.GetWaveformData()
    wf_data.Decompress()
    for j,ch in enumerate(ch_list): 
        wf = wf_data.GetWaveformWithChannel(ch)
    
        wf_array = np.array([wf.At(k) for k in range(wf.GetLength())])
        #print ch, np.mean(wf_array), np.std(wf_array)
        
        noise[j]+=np.std(wf_array)
        if np.std(wf_array) > 7.0:
            print "Event = ", i, np.std(wf_array)
            wfg = ROOT.TGraph(wf.GimmeHist())
            wfg.Draw('AL')
            c1.Update()
            raw_input("Enter")

print noise[0]/tree.GetEntries(), noise[1]/tree.GetEntries()


