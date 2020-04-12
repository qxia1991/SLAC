import ROOT

ROOT.gSystem.Load("libEXOROOT")

class signalmodel:
    def __init__(self,RC_tau_1,RC_tau_2,CR_tau_1,CR_tau_2,CR_tau_3,lower_range,upper_range,param):
        transfer_func = ROOT.EXOTransferFunction()
        transfer_func.AddIntegStageWithTime(RC_tau_1)
        transfer_func.AddIntegStageWithTime(RC_tau_2)
        transfer_func.AddDiffStageWithTime(CR_tau_1)
        transfer_func.AddDiffStageWithTime(CR_tau_2)
        transfer_func.AddDiffStageWithTime(CR_tau_3)
        self.model = ROOT.EXOSignalModel()
        ROOT.EXOUWireSignalModelBuilder(transfer_func).InitializeSignalModelIfNeeded(self.model, 0)
        self.function = self.model.GetFunction(lower_range, upper_range, len(param))
        for i in range(0, len(param)): self.function.SetParameter(i,param[i])

    def GetTF1(self):
        return self.function

microsecond = 1000.0
RC_tau_1 = 3.0*microsecond
RC_tau_2 = 3.0*microsecond
CR_tau_1 = 40.0*microsecond
CR_tau_2 = 40.0*microsecond
CR_tau_3 = 300.0*microsecond
par = [1.0,400.0,200.0*microsecond]
sm = signalmodel(RC_tau_1,RC_tau_2,CR_tau_1,CR_tau_2,CR_tau_3,0.0,2000.0*microsecond,par)
function = sm.GetTF1()
function.Draw("c")
raw_input('press [enter] to quit: ')
