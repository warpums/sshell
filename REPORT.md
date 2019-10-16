PHASE 1:
In Phase 1, we created an implementation similar to the code detailed in the
system calls lecture slides.

PHASE 2:
In Phase 2, we debated over various ways to obtain the line. Looking at the
manuals provided on the project instructions, we decided to use getline().
To handle error management, we looked at other phases of the project to get a
more holistic sense of how to implement them. We looked at each error case and
determined how each type can be detected.

PHASE 3:
In Phase 3, where we were challenged with handling multiple arguments, we
began to think of how to obtain them from the input line. We decided to try
strtok(), but when tried to execute, we had errors. We discovered that we
needed to remove the new line character from the last token. We used the
strtok() manual page and various Stack Overflow posts. We ended up using
strcspn() to remove the new line character. When using strtok(), we extracted
the first argument, and then used a while loop to get the remainding.

PHASE 4:
In Phase 4, we used strcmp() to determine if we were dealing with a builtin
command. For exit, we simply printed to stderr. For cd, we used getcwd() to
get the working directory, and then changed the destination using chdir().

PHASE 5 and PHASE 6:
Both input and output redirection were implemented in the child process. For
both processes, we first iterated throught the command line to determine if
the special characters (<, >) were present. If input redirection was present,
we replaced STDIN with the input file. Similarly, if output redirection was
present, we replaced STDOUT with the output file. We used the lecture slides
as resources to complete these two phases.

PHASE 7:
In Phase 7, we separated arguments between the pipes. We then opened the
appropriate number of pipes, replaced STDOUTs and STDINs with PIPEINs and
PIPEOUTS, and made sure they produced the expected output.

TESTING:
First we used examples from the project instruction page, then we developed
our own test cases. We ran commands on the terminal and then tried to
reproduce the results on our own shell. Then we finished by using the testing
script.
