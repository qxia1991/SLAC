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

fid = calib.getCalib("fiducial-volume", "vanilla", ROOT.EXOTimestamp()) 
print(fid)
ROOT.EXOFiducialVolume.SetUserRadialCut(100,0,50)
fid = calib.getCalib("fiducial-volume", "vanilla", ROOT.EXOTimestamp()) 
print(fid)
ROOT.EXOFiducialVolume.SetUserHexCut(120,5,180)
fid = calib.getCalib("fiducial-volume", "vanilla", ROOT.EXOTimestamp()) 
print(fid)
ROOT.EXOFiducialVolume.UseDatabase()
fid = calib.getCalib("fiducial-volume", "vanilla", ROOT.EXOTimestamp()) 
print(fid)
