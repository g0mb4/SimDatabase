# SimDatabase
A MySQL extension for Simulink. 

## extlib - External Library
Dynamic Link Library. I wrote this to test the capabilities of this method.

## sfun - CMEX S-Function
More elegant and useable soultion with proper error handling. **Use this!**

## Online mode

![online](imgs/online.png)

The Online database block uses a MySQL server to generate the desired output data, this is achieved by a custom dynamic link library (dll) file. This library was written in C using the **MySQL Connector C API version 6.1.11**.

The block can interpret a predefined format of SQL entry:

| date        | time     | data                 |
|:-----------:|:--------:|:--------------------:|
| YYYY-MM-DD  | hh:mm:ss | N1=v1;N2=v2;N3=v3; … |
| ...  		    |  ...     | ...                  |       

This way a specified number of *vs* can be extracted from the database for the given *Ns* based on the time interval.

The precision required for the simulation is, currently, achieved by linear interpolation, which can be turned off.

The connection and the query take a long time compared to the other operations. To solve this problem the block saves the results in a unique file and only uses the database when this file does not exist, otherwise reads the data from the file, which takes significantly less time.

Example: https://www.youtube.com/watch?v=yDe3t1PS36U

## Offline mode

![offline](imgs/offline.png)

The Offline database block uses a file to output the measured data. The Simulink program has no easy built-in support for custom text file handling, so this is achieved by the same .dll file used in the Online database block.

The block can interpret a predefined format of file entry:
```
Date;Time;N1;N2;N3;N4...
YYYY-MM-DD;hh:mm:ss;v1;v2;v3;v4...
```
This format can be easily exported from an Excel table using CSV export.

The precision required for the simulation is, currently, achieved by linear interpolation, which can be turned off.

## Requirements
- MATLAB R2018b + Simulink (trial can do the trick)
- Visual Studio 2017 Community Edition (free)

## Compilation
Setup ```mex``` with the ```mex -setup``` command in MATLAB.
- sfun : Run ```compile.m```.
- extlib : Run ```compile_and_test.m```.

## Usage
- sfun : Copy ```sfun_simdatabase.mexwXX``` to the **Working Directory** and see ```sim_test.slx```.
- extlib : Copy ```simdatabase.dll``` and ```resolve_pointer.m``` to the **Working Directory** and follow the example ```.SLX``` files.

## Notes
- The ```simdatabase_step_x()``` functions have **10** outputs.
- Compiled with MS Visual Studio 2017 Community.
- Tested on Windows 10 x64.
- Can be ported to Linux (```mex()``` will be different).

## TODO

## License
Use, modify as you wish, just mention me in your README ;) 
