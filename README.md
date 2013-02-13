jblas is a matrix library for Java which uses existing high
performance BLAS and LAPACK libraries like ATLAS.

* Version 1.2.3, February 13, 2013
* Version 1.2.2, December 17, 2012
* Version 1.2.1
* Version 1.2.0, January 7, 2011
* Version 1.1.1
* Version 1.1, August 16, 2010
* Version 1.0.2, February 26, 2010
* Version 1.0.1, January 14, 2010
* Version 1.0, December 22, 2009
* Version 0.3, September 17, 2009
* Version 0.2, May 8, 2009
* Version 0.1, March 28, 2009

see also the file RELEASE_NOTES

Homepage: http://jblas.org

![travis status](https://travis-ci.org/mikiobraun/jblas.png)
Travis Page: https://travis-ci.org/mikiobraun/jblas

INSTALL
-------

In principle, all you need is the jblas-1.2.0.jar in your
classpath. jblas-1.2.0.jar will then automagically extract your platform
dependent native library to a tempfile and load it from there. You can
also put that file somewhere in your load path ($LD_LIBRARY_PATH for
Linux, %PATH for Windows).


BUILDING
--------

If you only work on the java part, an ant build.xml is provided to
recompile the sources. In addition to that you need an installation of
ruby for some scripts which automaticall generate code. Then, you just
type "ant" on the command line.

If you want to build jblas from the sources including the native part,
you need to set up quite a few things:

You will need some implementation of blas and lapack. jblas is tested
with either plain lapack, or ATLAS
(http://math-atlas.sourceforge.net/). You also need the Fortran
sources for BLAS and LAPACK, available, for example from
http://www.netlib.org/lapack/lapack-lite-3.1.1.tgz.

If you still want to build the source your own, see INSTALL for
further details.


HOW TO GET STARTED
------------------

Have a look at javadoc/index.html and
javadoc/org/jblas/DoubleMatrix.html

If you want to validate your installation and get some performance
numbers, try "java -server -jar jblas-1.2.0.jar", or drop the server
in case, you only have the "client" JVM installed.


LICENSE
-------

jblas is distributed under a BSD-style license. See the file COPYING
for more information.


BUGS
----

If you encounter any bugs, feel free to go to http://jblas.org and
register a ticket for them. Make sure to include as much information
as possible. For configuration problems it would also be helpful to
include the file "configure.log".


CONTRIBUTORS
------------

see file AUTHORS