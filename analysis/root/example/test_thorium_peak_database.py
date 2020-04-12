"""
Python script to test interaction with the database
"""
import ROOT
ROOT.gSystem.Load("libEXOROOT")
calib = ROOT.EXOCalibManager.GetCalibManager()
calib.SetMetadataAccessType("mysql")
calib.SetPort(3306)
calib.SetUser("rd_exo_cond_ro")
calib.SetHost("mysql-node03.slac.stanford.edu")

vec = calib.GetRegisteredHandlers()
print "Available database entry types: "
for name in vec: 
    print "  ", name

peak = calib.getCalib("thorium-peak", "vanilla", ROOT.EXOTimestamp()) 
print(peak)
ROOT.EXOThoriumPeak.SetUserValues(1,2,3,4,5,6,7,8)
peak = calib.getCalib("thorium-peak", "vanilla", ROOT.EXOTimestamp()) 
print(peak)
ROOT.EXOThoriumPeak.UseDatabase()
peak = calib.getCalib("thorium-peak", "vanilla", ROOT.EXOTimestamp()) 
print(peak)
