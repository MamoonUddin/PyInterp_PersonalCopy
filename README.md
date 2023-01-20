COSC 4315 HW 2

A python mini-interpreter programmed in C++

By Muhammad Mamoon Uddin , Raymundo Torres

Date : 11/30/2022


Files :

    minipython.py : the main file to call
    Classes.hpp : hpp file that holds all the classes
    README.md : write up to everything related

Binding C++ data structure/class :

    The SymTab class keeps track of all the variables read from the python file and the access time is O(1) because the variables alongside their types/value is stored inside a hash table called symTabs.

Token Class:

    The token class helps us determine if a given token is a (print, if, else, def, operand, etc).

Tokenizer Class:

    The class keeps track of Tokens by storing them inside a vector of Tokens. It also houses token related functions such as getToken and ungetToken.

