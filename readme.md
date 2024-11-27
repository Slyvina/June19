
# June 19

June 19 is a simple interface library written in C++ based on SDL. My mere objective was to quickly set things up, and that is basically all there's to it. Please note that this library heavily depends on my TQSG, TQSE and my Tricky Units, so all need to be present in order to get anything to work.

Please note that the VS2019 project files were only meant to get the test program to run, as the library itself will exist in source code only, as my own use is only to have it all in one .exe file. I am heavily opposed against dependencies as all they do is undermine stability, and the saving on hard disk space is (especially with the disk space we got today) totally NOT worth it. Unfortunately you will still need SDL2 and SDL2_Image to be installed or to be present with any program in which these libraries are used, that is how SDL2 was delivered to me, and I don't like to mess with 3rd party stuff, so I decided not to.

# Name

Well, the name is derived from my birthday, which is by the way also the birthday of Paula Abdul, the cat Garfield, and Blaise Pascal, and probably more great names. I guess I should have written these in Pascal in stead now :-P


# Use

In order to use these libraries, you simply need to make sure the compiler sees the files in the src folder and that the compiler also can find the header files here. Of course, you need SDL present and the compiler to be able to find their header files (and make sure SDL2_Main is able to replace your main function, in order to get stuff done), and of course the compiler must see the required .cpp files in TQSG, TQSE and TrickyUnits and know where to find their respective headers.
Please note that I coded all this in VS2019, and since Microsoft has a bit of a funny way of "standard" and blocks the standards and substitute their own, I cannot fully check if these libraries are fully operational in Linux, Mac and other systems, especially since I have no active systems like that running now. I can only hope it all works.

Now if you just go for 
~~~c++
#include <june19.hpp>
~~~
You will have all parts of june19 present if you do not want to use all the features June 19 has, ignore that file and just use 
~~~c++
#include <june19_core.hpp>
#include <june19_.......> // Well the stuff you want there
~~~

Only june19_jcr6.hpp is not present in june19 as I am not sure if people are really into using JCR6 or being forced to use it.
