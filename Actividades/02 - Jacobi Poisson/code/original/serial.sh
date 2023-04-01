#!/bin/bash
#SBATCH --partition=all  	#Seleccione los nodos para el trabajo de todos el conjunto de nodos de cómputo del cluster
#SBATCH -o Serial.%j.out    	#Nombre del archivo de salida
#SBATCH -J Serial	        #Nombre del trabajo

N=100000
NSTEPS=1000
./jacobi1d $N $NSTEPS jacobiPoisson.out > timing_serial.out
