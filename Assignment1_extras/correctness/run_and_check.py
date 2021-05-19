import os
import csv
import numpy as np
import shutil

def getGlobalGridFromFolder(numProcs, numCells, timeSteps,  foldername):
    P = int((pow(numProcs, 0.5)))
    N = int((pow(numCells, 0.5)))
    timeGrid = np.zeros([timeSteps, P*N, P*N])
    for filename in os.listdir(foldername):
        filepath = os.path.join(foldername, filename)
        with open(filepath) as csvfile:
            entries = csv.reader(csvfile, delimiter=',')
            entries = list(entries)
            for entry in entries:
                t = int(entry[0])
                r = int(entry[1])
                c = int(entry[2])
                val = float(entry[3])
                timeGrid[t][r][c] = val
                
    return timeGrid

def make_the_new_input_file(temp_grid, numProcs, numCells, filename):
    P = int((pow(numProcs, 0.5)))
    N = int((pow(numCells, 0.5)))
    arrSize = P*N
    f = open(filename, "w")
    f.seek(0)
    for r in range(arrSize):
        for c in range(arrSize):
            f.write(str(temp_grid[0][r][c]) + "\n")
    f.close()
    return

numProcs = 64
numCells = 64*64
timeSteps = 3
mode = 2

P = int((pow(numProcs, 0.5)))
N = int((pow(numCells, 0.5)))

shutil.rmtree('all_ranks')
os.makedirs('all_ranks')
os.system("mpiexec -n " +str(numProcs) + " ./halo_with_print_all.o "+str(numCells) +" "+ str(timeSteps)+" "+ str(mode))
timeGrid = getGlobalGridFromFolder(numProcs, numCells, timeSteps, 'all_ranks')
make_the_new_input_file(timeGrid, numProcs, numCells, 'input_proc_1')
shutil.rmtree('single_proc')
os.makedirs('single_proc')
os.system("mpiexec -n 1 ./halo_check.o "+str(numProcs*numCells)+" "+ str(timeSteps)+" "+ str(mode))
timeGrid2 = getGlobalGridFromFolder(numProcs, numCells, timeSteps , 'single_proc')
print(np.sum(timeGrid-timeGrid2))
