import ROOT

ROOT.gSystem.Load("libEXOROOT")

rec = ROOT.EXOROOT.EXOReconstructionModule()
rec.set_ATeam_YMatch_flag(True)
rec.set_pattern_recognition_flag(4)
rec.SetVerbose(True)

afile = ROOT.TFile("test.root")
tree = afile.Get("tree")

tree.GetEntry(0)
event = tree.EventBranch

event.GetWaveformData().Decompress()

rec.Initialize()
rec.BeginOfRun(event)
rec.BeginOfEvent(event)
rec.EndOfEvent(event)
rec.EndOfRun(event)
rec.ShutDown()
