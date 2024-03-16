Design Notes
    - We utilized a recursive directory traversal to search for every .txt file within a given directory name. Before each recursive call, the program
        calls a method that checks the spelling within that file.



Commands to run code
    - Compile -> gcc -o spchk spchk.c
    - Run code -> ./spchk (dictionary).txt (directory name)
        For example: 
            ./spchk dict_test.txt test1
            Running this will give the program dict_test.txt as a dictionary to refer to and test1 as a directory to recursively search through
            