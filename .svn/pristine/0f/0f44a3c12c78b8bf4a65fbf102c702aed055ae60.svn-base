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
print("Available database entry types: ")
for name in vec: 
  print("  "+str(name))

cal = calib.getCalib("energy-calib-mc", "vanilla", ROOT.EXOTimestamp()) 
print(cal)
ROOT.EXOMCEnergyCalib.SetUserChargeValues(1,2,3,4)
cal = calib.getCalib("energy-calib-mc", "vanilla", ROOT.EXOTimestamp()) 
print(cal)
ROOT.EXOMCEnergyCalib.UseDatabaseForCharge()
cal = calib.getCalib("energy-calib-mc", "vanilla", ROOT.EXOTimestamp()) 
print(cal)
