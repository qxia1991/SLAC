#!/bin/bash

#./gen_LB_diff.sh events file number

#Event not used so just make dummy
RNUM=${1}
NEVENTS=${2}

#ulimit -c 1024
finalRC=1
#set -e

NAME=${OUTNAME}

export SCRATCHDIR=/tmp/exo_tmp/NOISE_${OUTNAME}
mkdir -p ${SCRATCHDIR}
cd ${SCRATCHDIR}

gotEXIT()
{
 rm -rf ${SCRATCHDIR}
 exit $finalRC
}
trap gotEXIT EXIT

source /nfs/slac/g/exo/mjewell/2D_Digitizer_Offline/EXOOUT/setup.sh
echo $EXOLIB/

python /nfs/slac/g/exo/mjewell/2D_Digitizer_Offline/utilities/misc/data/noiselib/Build_LB_Noise_Library.py ${RNUM} ${NEVENTS} &> /nfs/slac/g/exo-userdata/users/mjjewell/NoiseLibrary_50k/${RNUM}.log
rm -rf ${SCRATCHDIR}


finalRC=0
