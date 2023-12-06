# msh



## 1. Introduction

This is a linux shell in C that supports:

1. parse user input strings
2. fork for child process and use ``execve`` to evaluate and execute a command
3. handle keyboard interruption and child process termination using signals
4. handle parent preserve user history in a given file



## 2. How to run basic msh 



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
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh
msh> /usr/bin/echo hello
hello
msh> /usr/bin/ls -la .
total 84
drwxrwx--- 11 runqingc runqingc    16 Dec  1 16:38 .
drwxrwx---  3 runqingc runqingc     3 Nov  4 19:31 ..
drwxrwx---  2 runqingc runqingc     9 Dec  6 08:59 bin
drwxrwx---  2 runqingc runqingc     5 Dec  1 16:38 data
drwxrwx---  8 runqingc runqingc    15 Dec  2 21:29 .git
-rw-rw----  1 runqingc runqingc   430 Nov  4 19:31 .gitignore
msh> history
    1   jobs
    2   kill 16 %0
    3   history
    4   /usr/bin/seq 1 10;  /usr/bin/sleep 10 & /usr/bin/seq 1 5 ; /usr/bin/sleep 5; jobs;
    5   history
    6   /usr/bin/seq 1 10;  /usr/bin/sleep 10 & /usr/bin/seq 1 5 ; /usr/bin/sleep 5; jobs; 
    7   /usr/bin/ls -la .
    8   /usr/bin/echo hello
    9   /usr/bin/ls -la .
   10   history
msh> /usr/bin/echo Counting: ; /usr/bin/seq 1 5
Counting:
1
2
3
4
5
msh> exit  # use exit to quit the shell
```



### 2.3 Msh with optional arguments

 You can use this syntax to customize msh settings:

```sh
msg [-s NUMBER] [-j NUMBER] [-l NUMBER]
```

The `-s NUMBER` represents the maximum number of command lines to store in the shell history. `-j NUMBER` represents the maximum number of jobs that can be in existence at any point in time and `-l NUMBER` represents the maximum number of characters that can be entered in for a single command line. 

Test examples:

```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh -s 200 -l 20 -j 100
msh> /usr/bin/echo Counting:                   
error: reached the maximum line limit
msh> /usr/bin/seq 1 5                              
1
2
3
4
5
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh -l runqingc -s 45
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
/usr/bin/seq 1 10;  /usr/bin/sleep 8 & 

/usr/bin/seq 1 5 ; /usr/bin/sleep 5; jobs;
```

Then you can use:
```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh < ./data/test001.in 
1
2
3
4
5
6
7
8
9
10
1
2
3
4
5
[0]      2328686 RUNNNING          /usr/bin/sleep 8 
```



## 3. Advanced msh testing



In hw7 we tested 

### 3.1 History Module

The history records will be preserved in ``./data/.msh_history`` . Everytime user restarts the shell, it will load history from this file. User and use history to see their records.

```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh
msh> history
    1   kill 19 1475614
    2   jobs
    3   kill 18 %0
    4   jobs
    5   kill 9 1475614
    6   jobs
    7   /usr/bin/sleep 35&
    8   jobs
    9   kill 16 %0
   10   history
```

```sh
msh> /usr/bin/seq 1 10;  /usr/bin/sleep 10 & /usr/bin/seq 1 5 ; /usr/bin/sleep 5; jobs;
1
2
3
4
5
6
7
8
9
10
1
2
3
4
5
[0]     1480503 RUNNNING          /usr/bin/sleep 10 
msh> history
    1   kill 18 %0
    2   jobs
    3   kill 9 1475614
    4   jobs
    5   /usr/bin/sleep 35&
    6   jobs
    7   kill 16 %0
    8   history
    9   /usr/bin/seq 1 10;  /usr/bin/sleep 10 & /usr/bin/seq 1 5 ; /usr/bin/sleep 5; jobs;
   10   history
msh> 
```

The user can use ``!N`` to re-execute a command. ``!N`` and ``exit`` will not be preserved in history. See 3.3.2 and 3.3.3 for example. 



### 3.2 Keyboard Interruption

#### 3.2.1 ^C

``^C`` will send a ``SIGINT`` to the currently running foreground job and kill it. E.g.

```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh
msh> jobs
msh>  /usr/bin/sleep 60
^Cmsh> jobs
```

#### 3.2.3 ^Z

``^Z`` will send a ``SIGTSTP`` to the currently running foreground jobs and suspend it. E.g.

```sh
msh>  /usr/bin/sleep 60
^Zmsh> jobs
[0]     1454742 SUSPENDED        /usr/bin/sleep 60
```



### 3.3 Built in Commands



#### 3.3.1 jobs

The `jobs` command lists all the jobs currently active. E. g.

```sh
msh>  /usr/bin/sleep 30   
^Zmsh> /usr/bin/sleep 5&
msh> jobs
[0]     1455796 RUNNNING        /usr/bin/sleep 5
[1]     1455682 SUSPENDED        /usr/bin/sleep 30
msh> jobs
[1]     1455682 SUSPENDED        /usr/bin/sleep 30
```



#### 3.3.2 history

Calls `print_history` to print the current shell history. E. g.

```sh
msh> history
    1    /usr/bin/sleep 60
    2   jobs
    3    /usr/bin/sleep 60
    4   jobs
    5    /usr/bin/sleep 20  &
    6    /usr/bin/sleep 30
    7   /usr/bin/sleep 5&
    8   jobs
    9   jobs
   10   history
```



#### 3.3.3 !N

This command is the history expansion command where `N` is a line number from the `history` command.

```sh
msh> history
    1    /usr/bin/sleep 60
    2   jobs
    3    /usr/bin/sleep 20  &
    4    /usr/bin/sleep 30
    5   /usr/bin/sleep 5&
    6   jobs
    7   jobs
    8   history
    9   /usr/bin/seq 1 5
   10   history
msh> !9
1
2
3
4
5
```

Note that ``!N`` and ``exit`` will **not** be added to the history list. 

```sh
msh> /usr/bin/seq 11 15; exit
11
12
13
14
15
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh
msh> history
    1    /usr/bin/sleep 30
    2   /usr/bin/sleep 5&
    3   jobs
    4   jobs
    5   history
    6   /usr/bin/seq 1 5
    7   history
    8   /usr/bin/seq 1 5
    9   /usr/bin/seq 11 15; 
   10   history
```



#### 3.3.4 bg \<job>

Restarts `<job>` by sending it a `SIGCONT` signal, and then runs it in the **background**. Note that this job only applies to suspended jobs.

**Usage1**:  ``bg PID `` 

```sh
msh>  /usr/bin/sleep 300
^Z
msh> jobs
[0]     1462885 SUSPENDED        /usr/bin/sleep 300
msh> bg 1462885
msh> jobs
[0]     1462885 RUNNNING         /usr/bin/sleep 300
```

**Usage2**:  ``bg %JID `` 

```sh
msh> /usr/bin/sleep 310
^Z
msh> jobs
[0]     1465125 SUSPENDED       /usr/bin/sleep 310
msh> bg %0
msh> jobs
[0]     1465125 RUNNNING        /usr/bin/sleep 310
```



#### 3.3.5 fg \<job>

Restarts `<job>` by sending it a `SIGCONT` signal, and then runs it in the **foreground**. Note that this job applies to suspended jobs or background jobs.

**Usage1**:  ``bg PID `` 

**Usage2**:  ``bg %JID `` 

```sh
msh> /usr/bin/sleep 320&
msh> jobs
[0]     1471741 RUNNNING        /usr/bin/sleep 320
msh> fg %0   #then it will run in foreground and I can suspend it
^Z
msh> jobs
[0]     1471741 SUSPENDED       /usr/bin/sleep 320
msh> fg 1471741  #then it will run in foreground and I can suspend it
^Z
msh> jobs
[0]     1471741 SUSPENDED       /usr/bin/sleep 320
```



#### 3.3.6 kill

``kill SIG_NUM PID`` command - Calls the ``kill`` function, with ``SIG_NUM`` equal to either ``2`` (``SIGINT``), ``9`` (``SIGKILL``), ``18`` ( ``SIGCONT`` ), or ``19``(`SIGSTOP`).

**Usage1**:  ``kill SIG_NUM PID `` 

**Usage2**:  ``kill SIG_NUM %JID `` 

```sh
runqingc@linux5:~/Intro_to_Unix/msh/msh-runqingc$ msh
msh> /usr/bin/sleep 32&
msh> jobs
[0]     1474591 RUNNNING        /usr/bin/sleep 32
msh> kill 2 %0
msh> jobs
msh> /usr/bin/sleep 350&
msh> jobs
[0]     1475614 RUNNNING        /usr/bin/sleep 350
msh> kill 19 1475614
msh> jobs
[0]     1475614 SUSPENDED       /usr/bin/sleep 350
msh> kill 18 %0
msh> jobs
[0]     1475614 RUNNNING        /usr/bin/sleep 350
msh> kill 9 1475614
msh> jobs
msh> 
```

Any other `SIG_NUM` number must produce the error message: `error: invalid signal number` to the user.

```sh
msh> jobs
[0]     1477845 RUNNNING        /usr/bin/sleep 35
msh> kill 16 %0
error: invalid signal number. 
```





