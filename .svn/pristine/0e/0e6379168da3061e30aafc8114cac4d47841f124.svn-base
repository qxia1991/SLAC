import ROOT
import math

# Following in mm
ROOT.gSystem.Load("libEXOROOT")

#################################
# Build the waveform
waveform = ROOT.EXODoubleWaveform()
mus = ROOT.CLHEP.microsecond

waveform.SetLength(2048)
waveform.SetSamplingPeriod(1*mus)

# 50 kHz
first_freq = 1000*ROOT.CLHEP.hertz;
# 500 kHz
second_freq = 25000*ROOT.CLHEP.hertz;

twopi = ROOT.TMath.TwoPi()
for i in range(waveform.GetLength()):
    pt = i*waveform.GetSamplingPeriod()
    waveform[i] = math.sin(twopi*pt*first_freq) + math.sin(twopi*pt*second_freq)
#################################

c1 = ROOT.TCanvas()
waveform.GimmeHist().Draw()
c1.Update()
print "Original waveform"
raw_input("E")

# Do a copy so we can look at both results
wf1 = ROOT.EXODoubleWaveform(waveform)
wf2 = ROOT.EXODoubleWaveform(waveform)

# Get bandpass
bandpass = ROOT.EXOBandpassFilter()

# First try to remove the upper components
bandpass.SetUpperBandpass(second_freq/2)
bandpass.Transform(wf1)

# Now remove the lower components
bandpass.SetLowerBandpass(second_freq/2)
bandpass.SetUpperBandpass(4*second_freq)
bandpass.Transform(wf2)

hist = wf1.GimmeHist("1")
hist.SetLineColor(ROOT.kRed)
hist.Draw()
c1.Update()
print "Lower frequencies"
raw_input("E")
wf2.GimmeHist("2").Draw("same")
c1.Update()
print "Upper frequencies"
raw_input("E")

