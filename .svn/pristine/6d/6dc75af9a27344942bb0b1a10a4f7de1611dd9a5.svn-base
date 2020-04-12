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

thresh = calib.getCalib("position-threshold", "vanilla", ROOT.EXOTimestamp()) 
print(thresh)
ROOT.EXO2DRateThreshold.SetUserValues(1,2,3,4,5,6)
print(thresh)
ROOT.EXO2DRateThreshold.UseDatabase()
print(thresh)
