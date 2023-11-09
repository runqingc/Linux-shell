# msh

The starter code for the msh shell 



## 1. How to build and run tests



### 1.1 Run tests for parse_tok

Please navigate to ``./test``, then use the following commands to see the test result of test_parse_tok:
```sh 
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ cd tests/
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc/tests$ gcc -o test_parse_tok test_parse_tok.c ../src/shell.c -I../include -Wall
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc/tests$ ./test_parse_tok
```



### 1.2 Run tests for separate_args

Please navigate to ``./test``, then use the following commands to see the test result of test_separate_args:

```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ cd tests/
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc/tests$ gcc -o test_separate_args test_separate_args.c ../src/shell.c -I../include -Wall
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc/tests$ ./test_separate_args
```



## 2. How to run msh



### 2.1 Run msh:

Please navigate to the root directory of this project, then use the following command to build:

```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ . scripts/build.sh 
```



### 2.2 Msh default settings

Then you can start msh. To start default msh, you can simply type ``msh`` to use default shell settings: 

```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh
```

Test example:

```sh
msh> ls -la .
argv[0]=ls
argv[1]=-la
argv[2]=.
argc=3
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
```



### 2.3 Msh with optional arguments

 You can use this syntax to customize msh settings:

```sh
msg [-s NUMBER] [-j NUMBER] [-l NUMBER]
```

The `-s NUMBER` represents the maximum number of command lines to store in the shell history. `-j NUMBER` represents the maximum number of jobs that can be in existence at any point in time and `-l NUMBER` represents the maximum number of characters that can be entered in for a single command line. 

Test examples:

```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh -s 200 -l 5 -j 100
msh> git push
error: reached the maximum line limit
msh> ls .  
argv[0]=ls
argv[1]=.
argc=2
msh> exit
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh -l lamont -s 45
usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh -s 45 -j 0
usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh -l -j
usage: msg [-s NUMBER] [-j NUMBER] [-l NUMBER]
```



### 2.4 Msh with file redirection

Msh can work with file redirection. 

For example, given a file: ``test001.in`` which is in the ``./data`` directory with the following contents: 

```sh
ls -la .
cd .. & cat file; 
```

Then you can use:
```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh < ./data/test001.in 
msh> argv[0]=ls
argv[1]=-la
argv[2]=.
argc=3
msh> argv[0]=cd
argv[1]=..
argc=2
argv[0]=cat
argv[1]=file
argc=2
```

