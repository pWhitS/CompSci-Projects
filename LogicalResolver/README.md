LogicalResolver
===============
This project is an automated theorem prover, written in C++ using standard libraries. It utilizes the resolution technique to determine if a propositional statement in conjunctive normal form is satisfiable, i.e. if the statement can be true. 

The program is executed using the command line by invoking the executable name along with 2 additional arguments.
  
    ./resolver.x clauses.in solution.out
    
The file clauses.in must be a text file. The file must contains propositions named by a number, starting at 0, and seporated by a comma. A negative number indicates a 'not' proposition. The propositions appearing on the same line are disjuncts, and the clauses on seporate lines are conjuncts. Together, they form a propositional statement in conjunctive normal form. For Example:

    clauses.in
    0, 1, -2
    3, 6
    -7
    4, 5, -6, 8

This file represents the conjunctive normal form: (0 or 1 or not2) and (3 or 6) and (not7) and (4 or 5 or not6 or 8)

The file solution.out also is a text file. The format of this file is hard-coded into the output routine in the program. The top of the file shows the given clauses as the are in clauses.in. After this, the file shows the program's progress whenever a new clause is created or a clause is removed. Finally, the last few lines show a satisfiable solution for the propositional statement if at least one exists. An example output file has been uploaded to this repository. 

The resolver can be easily comlpied after modification by using the make file. 

    make -f resolver.mak
    
