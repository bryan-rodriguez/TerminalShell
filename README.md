# TerminalShell
C code used to create the Terminal Shell

some code was used from my Professor, Trevor Bakker at the University of Texas at Arlington, hence the MIT License.

Utilized the fork() function to create a seperate child pid to run execvp() which allows the user to run linux command.

to build this, go to your linux OS and then open the terminal and type 'gcc -Wall msh.c -o msh —std=c99' and run with './msh'
