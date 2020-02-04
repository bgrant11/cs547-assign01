#!/bin/bash
#SBATCH --job-name=project1
# Number of cores
#SBATCH --cpus-per-task=24
# Number of Nodes 
#SBATCH -N 1
# Assign memory usage in Megabytes 
#SBATCH --mem=8G

# Run for n trials for n threads
A=-1
B=1
iterations=100000000 # 8 zeros
n_threads=40
trials=5

for ((j=1;j<=$trials;j++));
do
   for ((i=1;i<=$n_threads;i++));
   do 
     (time ./integrate $A $B $iterations $i) &>> trial-$j.txt 
  done
done
