ESMS
====

**ESMS** = Electronic Soccer Management Simulator.

This project is `no longer maintained
<http://eli.thegreenplace.net/2007/06/29/the-end-of-the-road-for-esms/>`_. The
code is posted publicly mostly for historical value, but also because I still
occasionally receive email requests about it.

About this source code
----------------------

ESMS was initially written together with a friend as our first real software
project. We learned C while writing it and this shows on parts of the code. So,
the initial version was quite a crude lump of C - which nevertheless worked and
did its job admirably. In later incarnations, when I rewrote large parts of the
code and added features, I switched to C++ and tried to make the code cleaner,
adhering to real programming practices. So the source code is a mash-up of older
C code and newer C++ code (older being from 1999 and newer from 2004 or so).

Documentation
-------------

ESMS comes with detailed documentation. It's available in ``html_docs``. The
source of the documentation is Perl-ish POD files that need to be translated to
HTML with ``pod2html``. However, unless you want to modify the documentation
this isn't required since the output HTML is also checked into the repository.

Compiling
---------

All the source code compiles with any standard C++ compiler. Take a look at
``src/Makefile`` for compiling it.

License
-------

LGPL

----

Eli Bendersky (eliben@gmail.com)
