#!/bin/csh
# August 20, 2010
# N. Ackerman and S. Herrin
#
###############################
# Set up the variables to be used later
###############################

# What file should EXOAnalysis run on?
# !!! Make sure that it specifies to use !!!
# !!!  whatever file you have specified  !!!
# !!!  in $TRIGFILE.                     !!!   
set EXOFILE="macros/trignoise.exo"

# The first two are just output files
set TRIGFILE="tmptrig.trg"
set ANSOUT="./noiseresults.txt"

# These parameters control the trigger loop
set TRIGLOW=75
set TRIGINC=2
set TRIGHI=85

###############################
# Begin trigger stuff
###############################

set CURTRIG=$TRIGLOW

# Create and write column headings
# Can remove for simplified processing later
# You may want to record what files/date/etc this was made
echo 'Trig Pass' > $ANSOUT

# Begin loop
while ( $CURTRIG != $TRIGHI+1 )

# Write the trigger config file

echo 'APD i 5 0,0 0,0 0,0 0,0' > $TRIGFILE

# Create the part of the line not being changed
set tmpline1='APD s 5 '
set tmpline2=',0 600,0 700,0 800,0'

# Write the line that is changing
echo $tmpline1${CURTRIG}$tmpline2 >> $TRIGFILE
echo '' >> $TRIGFILE
echo 'VG i 15 0,0 0,0 0,0 0,0' >> $TRIGFILE
echo 'VG s 15 0,0 0,0 0,0 0,0 ' >> $TRIGFILE
echo '' >> $TRIGFILE
echo 'HV i 15 0,0 0,0 0,0 0,0' >> $TRIGFILE
echo 'HV s 15 0,0 0,0 0,0 0,0' >> $TRIGFILE

# Begin EXOAnalysis

echo 'Beginning EXOAnalysis'

# Pass will hold the number passed
# The output of EXOAnalysis is piped to grep
# grep searching for the title of the SUM TRIGGERS table
# grep returns the 6 lines following SUM TRIGGERS
# sed and awk then produce a nice little table of trigger values:
#*==============================*
#*        SUM TRIGGERS          *
#*==============================*
#| THRESH |   APD    VG     HV  |
#|    0   | [0,0]  [0,1]  [0,2] |
#|    1   | [1,0]  [1,1]  [1,2] |
#|    2   | [2,0]  [2,1]  [2,2] |
#|    3   | [3,0]  [3,1]  [3,2] |
#*==============================*
# The above table shows what index you should put in the printf("%d", mat[0,0]) part
# If you want an individual trigger, use 'INDIVIDUAL TRIGGERS' in place of 'SUM TRIGGERS'

set PASS=`$EXOLIB/bin/EXOAnalysis $EXOFILE | grep -A 6 'SUM TRIGGERS' | grep -v '\*' | sed 's/|/ /g' | awk '{ for (x=1; x <= NF; x++) mat[x-2, NR-2] = $x } END { printf("%d", mat[0,0]) }'`

# Now write out the number passed to a file
# If you want to have the values comma separated (or something) modify the following line
echo $CURTRIG $PASS >> $ANSOUT

@ CURTRIG = $CURTRIG + $TRIGINC
if ( $CURTRIG > $TRIGHI ) then
    #  You are all done!
    echo 'Have gone high enough'
    exit 0
endif

end
