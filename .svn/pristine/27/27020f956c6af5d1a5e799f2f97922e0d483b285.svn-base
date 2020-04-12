# Script demonstrating how one might use FFT (e.g. for convolution, etc.)
# M. Marino 2011

import ROOT
import math
ROOT.gSystem.Load("libEXOROOT")
wfFT = ROOT.EXOWaveformFT()
wfTwoFT = ROOT.EXOWaveformFT()
wf = ROOT.EXODoubleWaveform()
wf.SetLength(2048)
fft = ROOT.EXOFastFourierTransformFFTW.GetFFT(wf.GetLength())

#Just set up a simple waveform, plot it, then plot its FFT
wf[0] = 1
wf[1] = 1
wf[2] = 1
wf[3] = 1
wf[4] = 1
c1 = ROOT.TCanvas()
fft.PerformFFT(wf, wfFT)
wfFT.GimmeHist().Draw()
c1.Update()
raw_input("E")
fft.PerformInverseFFT(wf, wfFT)
wf.GimmeHist().Draw()
c1.Update()
raw_input("E")

# demonstrate convolution
print "Doing convolution"
for i in range(int(wf.GetLength()/2.)):
    test = (i)/10
    wf[i] = math.exp(-test*test)
    wf[wf.GetLength()-i]= wf[i]

print "Gaussian, FFTs are always circularly symmetric: "
wf.GimmeHist().Draw()
c1.Update()
raw_input("E")
fft.PerformFFT(wf, wfFT)
wftwo = ROOT.EXODoubleWaveform()
wftwo.SetLength(wf.GetLength())
wftwo[wftwo.GetLength()/2] = 1.
print "Subject wf: "
hist = wftwo.GimmeHist()
hist.SetLineWidth(3)
hist.Draw("hist")
c1.Update()
raw_input("E")
fft.PerformFFT(wftwo, wfTwoFT)

print "Convolving"
wfTwoFT *= wfFT
fft.PerformInverseFFT(wftwo, wfTwoFT)

print "Subject wf: "
wftwo.GimmeHist().Draw()
c1.Update()
raw_input("E")

