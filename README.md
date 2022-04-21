# DynamicArray
Basically ended up recreating the standard vector, but I assume it's worse because I can't outcode the standard lib.

Basic Info
----------
A basic array wrapper so I don't have to deal with malloc and calloc and whatnot by hand. Has similar functionality to ``std::vector``. 
Arrays will always be resized to powers of 2.

Empty init creates an array of size 0.

Initialization with a value will directly resize the array to the given value.

``push()`` and ``pop()`` work like a regular stack, ``append()`` will add a value or an array to the end of the existing array.

There are also a bunch of operator overloads you can check out.

Example
----------
```
dynArray<int> arrRegular(3);				//	Regular int[3], wrapped
dynArray<int> arrDefault;				//	Empty array
dynArray<int> arrList({ 10, 11, 12 });			//	Copy of the given array
										
arrDefault.push(1);					//	Add 1 to back
arrDefault.append({ 2, 3, 4, 5 });			//	Attach array to back
int i = arrDefault.pop();				//	Take value off back

arrRegular[0] = 4;
arrRegular[1] = i;												
arrRegular[2] = 6;						
										
arrDefault.resize(3);					//	Change apparent size of array
arrRegular += { 7, 8, 9 };				//	Add a primitive array to a dynArray
arrRegular += arrList;					//	Add 2 dynArrays together
										
dynArray<int> arrFill;					
arrFill = arrDefault;					//	Set a dynArray equal to another
arrFill.append(arrRegular);				//	Append a dynArray					
printDynArray(arrFill);					
										
arrFill.resize(5, true);				//	Resize and clamp array (new array is created)
printDynArray(arrFill);	        
```

Output
----------
```
  1 2 3 4 5 6 7 8 9 10 11 12
  1 2 3 4 5
```
