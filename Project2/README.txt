To compile the two c files, project2.c and student2.c, 
type "make" in the directory, from the command prompt 
to start the Makefile. To test, run ./project2 and enter 
the appropriate prompts for the simulation paramaters.
The simulation works perfectly when there is no corruption
or packet loss, and suffers some packet loss when they're
raised sufficiently high. It can also segmentation fault
when the final ack packet is corrupted, as it deals with 
ack corruption by relying on cumulative acknowledgements, 
so just run it on the random setting again.