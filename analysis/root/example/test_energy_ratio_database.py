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

ratio = calib.getCalib("energy-ratio", "vanilla", ROOT.EXOTimestamp()) 
print(ratio)
ROOT.EXOEnergyRatio.SetUserChargeValues(1,2,3,4,5,6)
ROOT.EXOEnergyRatio.SetUserScintValues(11,12,13,14,15,16)
ROOT.EXOEnergyRatio.SetUserRotatedValues(21,22,23,24,25,26)
ratio = calib.getCalib("energy-ratio", "vanilla", ROOT.EXOTimestamp()) 
print(ratio)
ROOT.EXOEnergyRatio.UseDatabaseForCharge()
ROOT.EXOEnergyRatio.UseDatabaseForScint()
ROOT.EXOEnergyRatio.UseDatabaseForRotated()
ratio = calib.getCalib("energy-ratio", "vanilla", ROOT.EXOTimestamp()) 
print(ratio)
