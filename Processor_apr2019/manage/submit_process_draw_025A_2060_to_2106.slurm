#!/bin/bash
#
#SBATCH --job-name=processing
#SBATCH --ntasks=1
#SBATCH --time=72:00:00

#SBATCH --mail-type=FAIL,END
#SBATCH --mail-user=alexis@nucleares.unam.mx

# setup system
source setup-system.sh

echo -e "\n## Slurm job started on $(date +'%d-%m-%Y at %T') #####################"

#-----------------------------------------------------------------------------
# raw input data directory, first and last Id's to process

RAW_DATA_DIR=/share/storage2/connie/sims/addnu_to_data/runs/025
FID=2060
LID=2106

# type of data: "data", "draw", "other" 
export TYPE="draw"

# base output directory (must be created manually)
BASE_OUT_DIR=/share/storage2/connie/nusims_analysis/processed02_draw/runs/025A

# Processing macros location
MACROS_DIR=/share/storage2/connie/data_analysis/Processor/macros

# damicTools options
export OPTS="-s 1 -s 2 -s 3 -s 4 -s 5 -s 6 -s 7 -s 8 -s 9 -s 10 -s 11 -s 12 -s 13 -s 14 -s 15 "

#-----------------------------------------------------------------------------
# output directory (will be created by makeList.py)
SUFFIX=$FID"_to_"$LID
OUT_DIR=$BASE_OUT_DIR/$TYPE"_"$SUFFIX

# Create lists
python $MACROS_DIR/makeList.py $RAW_DATA_DIR $BASE_OUT_DIR $FID $LID > listMB_$SUFFIX.log
cat listMB_$SUFFIX.log > $OUT_DIR/$TYPE"_"$SUFFIX.log; rm listMB_$SUFFIX.log

# doAll
cd $MACROS_DIR
srun ./doAll.sh $OUT_DIR >> $OUT_DIR/$TYPE"_"$SUFFIX.log 2>&1
cd -

echo -e "## Slurm job finished on $(date +'%d-%m-%Y at %T') ####################"

#-----------------------------------------------------------------------------
# ouptut to slurm log file

echo "## Job name:                         $SLURM_JOB_NAME"
echo "## Job ID:                           $SLURM_JOB_ID"

# append slurm log file to processing log file
echo -e "\n=================================================" >> $OUT_DIR/$TYPE"_"$SUFFIX.log
echo "slurm-$SLURM_JOB_ID.out:" >> $OUT_DIR/$TYPE"_"$SUFFIX.log
if [ -d "$OUT_DIR" ]; then
  cat slurm-$SLURM_JOB_ID.out >> $OUT_DIR/$TYPE"_"$SUFFIX.log
  rm slurm-$SLURM_JOB_ID.out
fi
