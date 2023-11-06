# msh

The starter code for the msh shell 



## How to build and run

use the following command:

```sh
runqingc@linux7:~/Intro_to_Unix/msh/msh-runqingc$ . scripts/build.sh 
```



## Sample test results:

```sh
runqingc@linux7:~/Intro_to_Unix/msh/msh-runqingc$ . scripts/build.sh 
gcc -o ./bin/msh bin/msh.o bin/shell.o -I./include
runqingc@linux7:~/Intro_to_Unix/msh/msh-runqingc$ msh
msh> ls -la .
argv[0]=ls 
argv[1]=-la 
argv[2]=.
argc=3
msh> 
msh> ls -la /mpcs/; cd .. & echo hello
argv[0]=ls 
argv[1]=-la 
argv[2]=/mpcs/
argc=3
argv[0]=cd 
argv[1]=.. 
argc=2
argv[0]=echo 
argv[1]=hello
argc=2
msh> exit
runqingc@linux7:~/Intro_to_Unix/msh/msh-runqingc$ msh -l lamont -s 45
usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]
runqingc@linux7:~/Intro_to_Unix/msh/msh-runqingc$ msh -s 45 -j 0
usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]
runqingc@linux7:~/Intro_to_Unix/msh/msh-runqingc$ msh -l -j
usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]
runqingc@linux7:~/Intro_to_Unix/msh/msh-runqingc$ msh -s 5
msh> ls -la /mpcs;
argv[0]=ls 
argv[1]=-la 
argv[2]=/mpcs
argc=3
msh> exit
runqingc@linux7:~/Intro_to_Unix/msh/msh-runqingc$ msh -l 5
msh> ls -la /mpcs;
error: reached the maximum line limit
msh> ls
argv[0]=ls
argc=1
msh> exit
```

