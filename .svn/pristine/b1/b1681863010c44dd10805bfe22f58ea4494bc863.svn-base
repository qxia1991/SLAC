# This basic trigger only uses APD sum signals
# all bb0n events should trigger, though only ~20% of bb2n
##########
#FORMAT IS:
#SYSTEM type hyst thresh1,pre1 thresh2,pre2 thresh3,pre3 thresh4,pre4
#SYSTEM must be APD, VG, or HV
#type is i or s
#Each system has 2 lines, one of each type
#hyst is the hysterisis value of that type
#The threshold value is the level at which a trigger occurs
#The pre is the prescale counter value for representational triggers
#If a threshold is set to 0 it is disabled
##########

APD i 5 0,0 0,0 0,0 0,0
APD s 5 400,0 600,0 700,0 800,0

VG i 15 0,0 0,0 0,0 0,0
VG s 15 0,0 0,0 0,0 0,0

HV i 15 0,0 0,0 0,0 0,0
HV s 15 0,0 0,0 0,0 0,0

