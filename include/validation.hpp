#pragma once

#include <mpi.h>
#include <iostream>

void validateNumberOfProcesses(int numberOfProcesses, int expectedNumberOfProcesses) {
    if (numberOfProcesses != expectedNumberOfProcesses) {
        std::cerr << "Wrong number of processes: expected " << expectedNumberOfProcesses
                  << ", got " << numberOfProcesses << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
}
