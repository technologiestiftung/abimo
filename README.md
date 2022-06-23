## ABIMO 3.3: Water Balance Model for Urban Areas

This software is a derivative of "Wasserhaushaltsmodell Berlin ABIMO 3.2", as 
being published at:
https://github.com/umweltatlas/abimo

For further information (in German) on that original version of ABIMO and on its 
history of development, see there.

## Using the Model with the Programming Language R

If you are using the programming language [R](https://www.r-project.org/) you 
may be interested in our R package 
[kwb.abimo](https://github.com/KWB-R/kwb.abimo). It is a wrapper around the 
Windows executable of ABIMO 3.3 and allows to run the model from within the
R environment.

## Change Log

The main changes so far (June 2022) are:

- make runnable on Qt 5.15.2 (MSVC 2019, 64 bit)
- clean and refactor the code
- allow to run in batch mode (specify input/config file on the command line)
