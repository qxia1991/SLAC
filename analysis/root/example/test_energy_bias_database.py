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

bias = calib.getCalib("energy-bias", "vanilla", ROOT.EXOTimestamp()) 
print(bias)
ROOT.EXOEnergyBias.SetUserValuesForCharge(1,2,3,4,5,6,7,8)
ROOT.EXOEnergyBias.SetUserValuesForRotated(11,12,13,14,15,16,17,18)
bias = calib.getCalib("energy-bias", "vanilla", ROOT.EXOTimestamp()) 
print(bias)
ROOT.EXOEnergyBias.UseDatabaseForCharge()
ROOT.EXOEnergyBias.UseDatabaseForRotated()
bias = calib.getCalib("energy-bias", "vanilla", ROOT.EXOTimestamp()) 
print(bias)
