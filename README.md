# MM1 Simulation

This is a simulation of a M/M/1 queueing system. The simulation is written in C++ and uses the simlib library to simulate the queueing system. The simulation is based on the following parameters:

- Interarrival time: 7 minutes
- Service time: 8 minutes
- Number of customers: User defined

# Build

To build the simulation, run the following command:

- `cp param.example.txt param.txt`
- `g++ 'mm1sm.cpp' -o program`
- `./program`

# Reference

This code is based on the mm1 simulation provided by professor Jorge Ortiz at UNAL. Part of the code was modified following this example code:

https://github.com/yusufRahmatullah/simlib/blob/master/mm1smlb.c

"lcgrand.cpp" was taken from
http://www.sju.edu/~sforman/courses/2000F_CSC_5835/, (Random number generator based on simlib.c)
