"""
Python script to test interaction with the database
"""
import ROOT
ROOT.gSystem.Load("libEXOROOT")
calib = ROOT.EXOCalibManager.GetCalibManager()
calib.SetMetadataAccessType("mysql")
#calib.SetMetadataAccessType("default")
calib.SetPort(3306)
calib.SetUser("rd_exo_cond_ro")
calib.SetHost("mysql-node03.slac.stanford.edu")

vec = calib.GetRegisteredHandlers()
print "Available database entry types: "
for name in vec: 
    print "  ", name

res = calib.getCalib("energy-resolution", "vanilla", ROOT.EXOTimestamp()) 
print(res)
ROOT.EXOEnergyResCalib.SetUserValuesForCharge(1,2,3,4,5,6)
ROOT.EXOEnergyResCalib.SetUserValuesForScint(11,12,13,14,15,16)
ROOT.EXOEnergyResCalib.SetUserValuesForRotated(21,22,23,24,25,26)
print(res)
ROOT.EXOEnergyResCalib.UseDatabaseForCharge()
ROOT.EXOEnergyResCalib.UseDatabaseForScint()
ROOT.EXOEnergyResCalib.UseDatabaseForRotated()
print(res)
