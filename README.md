# ILOC_Allocator

School project written for a Compilers class.

- Allocates branchless code to fit on hardware utilizing *k* registers
- Scans and parses input code blocks into an intermediate representation
- Optimizes allocated code's cycle count using rematerialization, memory checking, and clean value tracking



## Usage
*From -h flag*

usage: tipc [-lpthd] file\
&emsp;&emsp; -a, --alloc          &emsp;&emsp;&emsp;&emsp;               run the allocator algorithm and pretty-print the results\
&emsp;&emsp; -k NUM               &emsp;&emsp;&emsp;&emsp;&ensp;&nbsp;   use NUM registers when allocating the code\
&emsp;&emsp; -l, --lexer          &emsp;&emsp;&emsp;&emsp;&nbsp;         print a list of tokens, one per line\
&emsp;&emsp; -p, --pretty-print   &ensp;&nbsp;                           print legal ILOC code\
&emsp;&emsp; -t, --table-print    &emsp;&ensp;                           print the intermediate representation in tabular form\
&emsp;&emsp; -h, --help           &emsp;&emsp;&emsp;&emsp;               print this menu\
&emsp;&emsp; -d, --debug          &emsp;&emsp;&emsp;                     print debug information\

## Write Up
[Compiler Report.pdf](https://github.com/APTanner/ILOC_Allocator/files/12911058/Compiler.Report.pdf)
