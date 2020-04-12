"""
 Example script for fitting to a waveform using pieces of reconstruction.
M. Marino Aug 2012
"""
import ROOT

ROOT.gSystem.Load("libEXOROOT")

class Waveform:
    def __init__(self, integrating_times=None, diff_times=None):  
        if not integrating_times: integrating_times = []
        if not diff_times: diff_times = []

        # Set up the transfer function
        tf = ROOT.EXOTransferFunction()
        for int_time in integrating_times: tf.AddIntegStageWithTime(int_time)
        for diff_time in diff_times: tf.AddDiffStageWithTime(diff_time)

        self.trans_function = tf

	# Set up the signal model, we are assuming it's a U-wire type signal
	self.signal_model = ROOT.EXOSignalModel()
        builder = ROOT.EXOUWireSignalModelBuilder(tf)
        builder.InitializeSignalModelIfNeeded(self.signal_model, 0)
        
    def get_signal_model(self):
        return self.signal_model

    def shape_wf(self, awf):
        # Shape the waveform
	# Conversion is a unfortunate aspect of doing this in a script
	temp_wf = awf.Convert('Double_t')() 
        self.trans_function.Transform(temp_wf)    
        int_wf = temp_wf.Convert('Int_t')()
        for i in range(awf.GetLength()): awf[i] = int_wf[i]

def main():

    c1 = ROOT.TCanvas()
    chi = ROOT.EXOSignalFitterChiSquare()

    # Define where we want the signal to be
    signal_at = 1024
    signal_length = 2048
    amplitude = 512
    mus = ROOT.CLHEP.microsecond

    # Define the Waveform object which builds signals models, etc.
    wf_gen = Waveform(integrating_times=[1*mus], 
                      diff_times=[60*mus, 100*mus])

    # Make the initial waveform
    exo_wf = ROOT.EXOWaveform()
    exo_wf.SetLength(signal_length)
    for i in range(signal_length):
        if i >= signal_at: exo_wf[i] = amplitude
        else : exo_wf[i] = 0

    # include waveform, this is to use the whole waveform.  The below is a hack
    # to be able to save something in the EXOBoolWaveform, which is a char
    # waveform. "\x01"*signal_length gives a const char* of 1's signal_length
    # long. 
    bool_wf = ROOT.EXOBoolWaveform("\x01"*signal_length, signal_length)

    # Shape the waveform
    wf_gen.shape_wf(exo_wf) 

    # Plot it
    exo_wf.GimmeHist().Draw()
    c1.Update()
    raw_input("E")

    # Begin fitting code
    # "found" signal
    signal = ROOT.EXOSignal()
    signal.fMagnitude = amplitude
    signal.fMagnitudeError = amplitude*0.1
    signal.fTime = signal_at*mus
    signal.fTimeError = signal_at*mus*0.1

    # "found" signals
    all_signals = ROOT.EXOChannelSignals()
    # Add the signal and set the waveform
    all_signals.AddSignal(signal)
    all_signals.SetWaveform(exo_wf)

    # Add the everything to the fit model
    
    chi.AddSignalsWithFitModel(all_signals, # all the found signals
                               wf_gen.get_signal_model(), # signal model
                               ROOT.EXOWireFitRanges(0, signal_length*mus), # fit ranges
                               bool_wf, # include waveform (which points are fit)
                               1.0, # calculated noise (should be non-zero)
                               0.0  # baseline
                              )

    # Do the fit
    chi.Minimize()

    # Print the results
    chi.GetFitChannelSignalsAt(0).Print()

    # Plot the results
    obj = ROOT.TList()
    chi.GetHistPlotOfResults(obj)

    for ahist in obj:
        ahist.Draw()
        c1.Update()
        raw_input("E")
     
if __name__ == '__main__': 
    main()
