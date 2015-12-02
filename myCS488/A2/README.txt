Compilation:
Compile by running the "make" command
This program was tested on the gl14.student.cs computer

Manual:

I did not implement clipping or view port mode. 


Code Credit Request:
1. The code that didn't work is the method clip, on line 281-302.

2. It does not clip the cube against the near plane or any plane because both values (cA & cB) 
   return a positive number or negative number but not a mix when the cube crosses the near plane (or any other plane).
   The only time when the clipping is applied is when the cube is pressed right up against the window.

3. I believe that the probable cause is that I have not converted into view space correctly.

4. I have tried to run the algorithm and looked at what the values of cA and cB , while translating
   the  cube towards and away from the near plane and seeing what values can be supplied for the
   point and normal to get it to clip properly.