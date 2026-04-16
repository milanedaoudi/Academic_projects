This repository holds the final results of my project. 
In /piklab, there are 3 piklab projects to test 3 different assembly programs. 
In /simulations, you can mainly find the Makefile of each exercices and the makefile of the last simulation to verify the project works correctly. 
In /vhdl, there is the code for each exercices. /vhdl/project is the folder used by the top simulation, this is the code you should be reading if you think something is wrong for the last simulation.

To execute top simulation and verify the project, you can do " make TESTBENCH=PIC ". It executes the file tb\_project located in /vhdl/project. It let the simulation run for 100 clock cycles and executes assembly program of /home/mdaoudi/Documents/ELEC-E9540/PIC16F84A/testbenchX.hex. You can change the program executed by choosing X=1, 2 or 3. You can also change the total simulation time. 
