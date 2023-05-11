"""   -- Start License block
src/head.py
Main header generator - Created out of pure laziness
version: 20.11.27
Copyright (C) 2020 Jeroen P. Broks
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
-- End License block   """
from glob import glob

out="#pragma once\n"

for k in glob("*.hpp"):
    if k!="june19.hpp" and k!="june19_jcr6.hpp":
        print(k)
        out += "#include \"%s\"\n"%k

text_file = open("june19.hpp", "w")
text_file.write(out)
text_file.close()    

# Hey I'm lazy and this Python script saves me tons of work, mind you?