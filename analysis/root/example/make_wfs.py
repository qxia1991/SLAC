# Example python script for using the EXODigitizeSignalModule to generate wire signals
# M. Marino June 2011

import ROOT
import random
import math
import time

# Following in mm
lx_radius = 185.7248 
z_height  = 180.00
voxel_size = 0.5

ROOT.gSystem.Load("libEXOROOT")

def make_wfs():

    # Load the module.  It's a plugin, so we have to load it specially.
    exodig = ROOT.EXODigitizeSignalModule() 
    digitizer = ROOT.EXODigitizeModule() 
    
    digitizer.SetDigitizationTime(2048) # in mus
    
    # Get randome positions for the x and y coordinates
    r = lx_radius*math.sqrt(random.random())
    theta = ROOT.TMath.TwoPi()*random.random()
    x = int(r*math.cos(theta)/voxel_size)
    y = int(r*math.sin(theta)/voxel_size)

    # Add two charge deposits, changing the time (in ns) 
    exodig.AddChargePoint(x, y, 50, 5e5, 1000)
    exodig.AddChargePoint(x, y, 50, 1e6, 1000)
    exodig.AddAPDHit(20, 100, 1000)

    # Add two charge deposits, changing the z position
    exodig.NextEvent()
    exodig.AddChargePoint(x, y, 170, 5e5, 1000)
    exodig.AddChargePoint(x, y, 10, 5e5, 1000)
    exodig.AddAPDHit(20, 100, 1000)
    
    # Now display the events that were made
    c1 = ROOT.TCanvas()
    while 1:
        event = exodig.GetNextEvent()
        if not event: break
        # Set the time to now
        event.fEventHeader.fTriggerSeconds = int(time.time())
        digitizer.ProcessEvent(event)
        
        channels_affected = set() 
        channels_deposited = set() 
        for pcd in event.fMonteCarloData.GetPixelatedChargeDepositsArray():
            for j in pcd.fWireChannelsAffected: channels_affected.add(j)
            channels_deposited.add(pcd.fDepositChannel)
        
        print "Channels affected:",channels_affected
        print "Deposit channels:",channels_deposited
        
        wfs = event.GetWaveformData()
        for chan in channels_affected:
            wf = wfs.GetWaveformWithChannel(chan)
            wf.GimmeHist().Draw()
            c1.Update()
            if chan in channels_deposited: print "Deposit channel", wf.fChannel
            else: print "No deposit channel", wf.fChannel
            raw_input("E")

if __name__ == '__main__':
    make_wfs() 
