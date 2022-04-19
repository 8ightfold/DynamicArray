# DynamicArray
Basically ended up recreating the standard vector, but I assume it's worse because I can't outcode the standard lib.

Basic Info
----------
A basic array wrapper so I don't have to deal with malloc and calloc and whatnot by hand. Has similar functionality to ``std::vector``. 
Arrays will always be resized to powers of 2.

Empty init creates an array of size 0. Don't do anything with this, it won't work.

Initialization with a value will directly resize the array to the given value.

Initialization with a value and *true* will reserve space for values to be pushed back.

``push()`` and ``pop()`` work like a regular stack, ``append()`` will add an array to the end of the existing array.

There are also a bunch of operator overloads you can check out.
