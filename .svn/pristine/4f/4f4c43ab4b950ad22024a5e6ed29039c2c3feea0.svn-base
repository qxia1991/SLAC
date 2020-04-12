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

diag = calib.getCalib("diagonal-cut", "vanilla", ROOT.EXOTimestamp()) 
print(diag)
ROOT.EXODiagonalCut.SetUserValues(1,2,3,4)
diag = calib.getCalib("diagonal-cut", "vanilla", ROOT.EXOTimestamp()) 
print(diag)
ROOT.EXODiagonalCut.UseDatabase()
diag = calib.getCalib("diagonal-cut", "vanilla", ROOT.EXOTimestamp()) 
print(diag)
