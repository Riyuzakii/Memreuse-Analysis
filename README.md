# Memreuse-Analysis
Analysis of memory reuse and sharing profiles of parallel programs

### Setup and Build Instructions
Clone the repository in `source/tools/CS622Assignment` path of `pin-3.11` tool.

``` bash
$ git clone git@github.com:Riyuzakii/Memreuse-Analysis.git <Path-to-Pin>/source/tools/CS622Assignment
```

Compile the target programs, i.e `prog{1, 2, 3, 4}.c` using `./compile.sh`


**Number of machine accesses**
```
addrtrace1.out   128990938
addrtrace2.out   2508113
addrtrace3.out   9531558
addrtrace4.out   1064400
```


  128988038 output/addtrace1.1.out
  128988149 output/addtrace1.2.out
  128987956 output/addtrace1.3.out
  128988046 output/addtrace1.4.out
  128987901 output/addtrace1.5.out

  2528955 output/addtrace2.1.out
  2513452 output/addtrace2.2.out
  2521172 output/addtrace2.3.out
  2524574 output/addtrace2.4.out
  2532314 output/addtrace2.5.out

  9508261 output/addtrace3.1.out
  9510696 output/addtrace3.2.out
  9501049 output/addtrace3.3.out
  9497081 output/addtrace3.4.out
  9521463 output/addtrace3.5.out

  1061544 output/addtrace4.1.out
  1061507 output/addtrace4.2.out
  1061492 output/addtrace4.3.out
  1061525 output/addtrace4.4.out
  1061515 output/addtrace4.5.out
