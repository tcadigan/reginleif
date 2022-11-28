# Terrain

Date of origin: 2006

Original source: http://www.shamusyoung.com/files/terrain_source.zip

[![Build Status](https://travis-ci.org/tcadigan/snobol_III.svg?branch=master)](https://travis-ci.org/tcadigan/snobol_III)

Goals:

1) Re-type all source code [DONE]

2) Port the Windows specific parts to Linux [WIP]

3) Get it to compile [TBD]

4) Likely the above won't work right, reimplement so it does [TBD]

Notes:

This is OpenGL but it is developed for Windows. Thus, all the window and mouse handling will need to be replaced with SDL. Shouldn't be too bad.

Intrusive linked lists are used for the entities, which I am not fond of. This isn't without precendent: http://fabiensanglard.net/doom3_documentation/DOOM-3-BFG-Technical-Note.pdf.
