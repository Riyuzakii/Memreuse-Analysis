# Memory Reuse and Sharing Profile Analysis
Discrete analysis of memory reuse and sharing profiles of a set of parallel programs.
Memory accesses are obtained through PIN's `IARG_MEMORYOP_EA` macro, which in turn
is broken down into individual x86 machine accesses according to specified rules.

The analysis tool is developed as a part of the Assignment-2 for the course
Advanced Computer Architecture **(CS622A)**,<br> Fall Semester - 2019, 
instructed by [Prof. Mainak Chaudhury](https://www.cse.iitk.ac.in/users/mainakc/).

The results of the simulation are compiled [here](./result/README.md).
And the project report can be found [here](./docs/report.pdf).

### Group Members (G16)

| __Name__ | __Email__ | __Roll__ |
|-------------|------------|------------|
| Aditya Rohan | [raditya@iitk.ac.in](mailto:raditya@iitk.ac.in) | 160053 |
| Aniket Pandey | [aniketp@iitk.ac.in](mailto:aniketp@iitk.ac.in) | 160113 |

### Setup and Build Instructions
Clone the repository in `source/tools/CS622Assignment` path of `pin-3.11` tool.

``` bash
$ git clone git@github.com:Riyuzakii/Memreuse-Analysis.git <Path-to-Pin>/source/tools/CS622Assignment
```

Compiling and running the tool is easy. Like, really easy!

The root folder of the project contains a Makefile which is not something we're
allowed to touch! As a result, each of the following commands need to be executed
from within the `scripts` directory.

``` bash
 > cd scripts
 > make setup          # Compiles instrumentation, analysis and target programs
```
<hr>

**Part 1**: Collecting machine access trace

``` bash
 > make run PART=1 PROG=x                   # Collects access trace for progx.c
```
where `x ∈  {1, 2, 3, 4}`. This will create `addrtrace.out` inside the `result`
directory. **Note**: All subsequent parts will use this trace and running the above
command with `PROG=y` where `y ≠ x` will overwrite the trace.

**Part 2, 3, 4**: Analysis of the obtained trace

``` bash
 > make run PART=x
```
where `x ∈  {2, 3, 4}`. This will use the trace generated above.
