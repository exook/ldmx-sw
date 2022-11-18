#! usr/bin/bash

# Example command:
# bash run.sh max_events=10000 input=data/20220424/pedestal_DPM1_20220424_220542.raw mode=all

cd ../../
source ldmx-sw/scripts/ldmx-env.sh
cd ldmx-sw/pedestals/

for ARGUMENT in "$@"
do
   KEY=$(echo $ARGUMENT | cut -f1 -d=)

   KEY_LENGTH=${#KEY}
   VALUE="${ARGUMENT:$KEY_LENGTH+1}"

   export "$KEY"="$VALUE"
done

# use here your expected variables
echo "input = $input"
echo "max_events = $max_events"
echo "mode = $mode"


if [ "$mode" == "decode" ]; then
    ldmx fire decode.py --max_events=$max_events --input=$input
elif [ "$mode" == "unpack" ]; then
    ldmx fire unpack.py --input=$input
elif [ "$mode" == "plot" ]; then
    python3 plot.py --input=$input
elif [ "$mode" == "all" ]; then
    ldmx fire decode.py --max_events=$max_events --input=$input
    ldmx fire unpack.py --input=$input
    python3 plot.py --input=$input
fi
