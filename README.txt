Lukas Chang - ldc105
Yousef Naam - yn164

Design Notes
    - We utilized a recursive directory traversal to search for every .txt file within a given directory name. Before each recursive call, the program
        calls a method that checks the spelling within that file.

Testing


Commands
    To run normally:
        - Compile -> gcc -o spchk spchk.c
        - Run code -> ./spchk (dictionary).txt (directory name)
            For example: 
                ./spchk dict_test.txt test1
                Running this will give the program dict_test.txt as a dictionary to refer to and test1 as a directory to recursively search through
    To run with make file:
        - Compile and make .o file -> make
        - Run test -> make test
        - Input files and run -> make run (dictionary).txt (diretory name)
        - Delete spchk.o -> make clean




Current state of code:
    Program works great. Passes cases where words are spelt in correctly as in they do not match with characters every time. Does not pass the 
    capitalization cases though. I made a few test cases in the /file/file2/testcases.txt. The desired results should be self explanatory, anything
    after the "misspelled:" should be considered misspelled. Yousef try to figure out the capitalization issue, I tried fixing it in the compareWords() function
    but it didnt work.

    useful rules for capitalization
        1. If the dictionary word starts with an uppercase letter, it allows for an exact match, an all-uppercase match, or an exact match with the initial letter capitalized.
        2. If the dictionary word starts with a lowercase letter, it allows for a all-uppercase match, or a match with the initial letter capitalized.

