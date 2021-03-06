================================================
Falaise C++ Library for the SuperNEMO experiment
================================================

:Authors: Steven Calvez, Xavier Garrido, François Mauger, Ben Morgan, Guillaume Oliviero
:Date:    2017-05-04
:Contact: snemo-software-l@in2p3.fr
:Version: 3.0.0

.. contents::
   :depth: 3
..

.. raw:: pdf

   PageBreak oneColumn

Version Control
===============

As of the 4th May 2017, Falaise development migrated from SVN to Git.


About Falaise
=============

.. _Falaise: https://nemo.lpc-caen.in2p3.fr/wiki/Software/Falaise

Falaise_ provides the main computational environment for the simulation,
processing and analysis of data for the SuperNEMO double beta decay search
experiment.

The three main components are

falaise
  the core library.

flsimulate
  the main detector simulation application.

flreconstruct
  the main reconstruction application.

A pipeline architecture is used in which the pipeline stages
may be configured and added to at runtime via a plugin system.
Code for pipeline stages is stored in the ``modules`` subdirectory
with each module (or set of modules) having its own directory.
Note that if you are using a SVN checkout of Falaise that some of these
module directories may be imported via an svn:externals link.

Additional modules from external sources and individual contribution
can be used too.

TODO : other apps and APIs.


Naming
------
Falaise is named thus because Falaise_ is the `town in Normandy`_ where William
the Conqueror was born. Note this has nothing to do with SuperNEMO software!

.. _`town in Normandy`: http://en.wikipedia.org/wiki/Falaise,_Calvados

Licensing
---------
Please study the file ``LICENSE.txt`` for the distribution terms and
conditions of use of Falaise.

Getting Help
============

If you have problems, questions, ideas or suggestions on Falaise or
any of its submodules, please contact the SuperNEMO Software Working
Group via the main development website https://nemo.lpc-caen.in2p3.fr/.

Getting Falaise
===============

You can obtain the Falaise source code from the main LPC Caen Subversion
repository. To checkout the trunk, simply do:

.. code:: sh

   $ svn co https://nemo.lpc-caen.in2p3.fr/svn/Falaise/trunk Falaise/trunk
..

You can of course checkout to a directory of your choosing. Tagged versions
can be listed by doing:

.. code:: sh

   $ svn ls https://nemo.lpc-caen.in2p3.fr/svn/Falaise/tags
..

and checked out in similar fashion to the trunk. If you use a tag, you
should always take the newest version. However, Falaise is developed with
the aim of keeping the trunk stable, so you can use this with high
confidence.

.. raw:: pdf

   PageBreak oneColumn

Preparing Your System for Falaise Installation
==============================================

To install Falaise, you first need the following sofware:

Prerequisite Software
---------------------

UNIX Operating System
.....................

* Linux
* macOS 10.10 (Yosemite) or higher

We expect Falaise to work on most modern BSD based Unices or Linux
distributions derived from Fedora(RedHat) or Debian provided the
software listed below is installed. However, we can only fully support
and test the following at present:

* SUSE Enterprise Linux 11SP4
* Ubuntu 14.04LTS, 16.04LTS: http://www.ubuntu.com
* CentOS/RHEL/SL 5/6: http://www.centos.org
* macOS 10.10, 10.11, 10.12

If you have problems on systems other than these, please contact us,
though support will be on a best effort basis.

Core Software Required
......................

* Subversion (only for development)
* CMake 3.3 or higher: http://www.cmake.org
* C/C++ compiler supporting at least C++11 standard
  (GNU/Clang/Intel)

Optional Software
.................

Required for documentation development:

* Doxygen (for documentation development)
* Pandoc (for man page generation)

On Linux, you should install these through the package manager for your
distribution. Some older Linux systems (CentOS, especially on
clusters) may not provide CMake 3.3. If this is the case, then you
should download the latest Linux *binary .sh* file from:

  http://www.cmake.org/cmake/resources/software.html

and follow the instructions on that page to install it.

On mac OS, simply install CMake from the latest ``Darwin64`` dmg
bundle available from:

  http://www.cmake.org/cmake/resources/software.html

To obtain the other tools, simply install the latest version of Xcode
from the Mac App Store. After installation, you should also install the
Xcode command line tools via going to the menu Xcode->Preferences,
clicking on the Downloads tab, and then installing Command Line Tools.

Core Libraries Required
.......................

* Bayeux 3.0 or higher
* Boost 1.55.0 or higher: http://www.boost.org
  with serialization, filesystem, system... libraries.
* Camp 0.7.1 : https://github.com/tegesoft/camp
* GSL 1.15 or higher: http://www.gnu.org/s/gsl
* CLHEP 2.1.3.1 or higher: http://proj-clhep.web.cern.ch
* Geant4 9.6.4 or higher: http://geant4.cern.ch
  with GDML support enabled
* ROOT 5.34 or higher: http://root.cern.ch

For ease of use, LPC Caen provides the Cadfaelbrew SDK which bundles these
libraries for easy use by Bayeux and clients of Bayeux. You can use this
bundle, or, at your discretion, your own, or system, installs of these
packages.


Installing Falaise
==================
Falaise provides a CMake based build system. We'll assume for brevity
that you are using a UNIX system on the command line (i.e. macOS or Linux).
We'll also assume that you're going to use the Cadfaelbrew SDK to provide
the required third party packages.

To build Falaise, simply do, from the directory in which this file
resides:

.. code:: sh

   $ mkdir build
   $ cd build
   $ cmake -DCMAKE_INSTALL_PREFIX=<where you want to install> \
       -DCMAKE_PREFIX_PATH=<path to your Cadfael install> ..
   $ make
..

Binaries and documentation will be output under a directory named
``BuildProducts`` in the build directory. The layout of this file
follows POSIX conventions, so you can find, **and use**, the binaries,
libraries and documents present therein. For example, you may do

.. code:: sh

   $ ./BuildProducts/bin/flreconstruct --help
..

to run and get help on the ``flreconstruct`` application. Documentation,
when built, is stored in the ``BuildProducts/share`` subdirectory of
your Falaise build directory. Doxygen documentation may be viewed immediately
via the terminal by running, on macOS:

.. code:: sh

   $ open ./BuildProducts/share/Falaise-<VERSION>/Documentation/API/html/index.html
..

where ``<VERSION>`` is the current Falaise version (simply use tab-complete
if you are unsure of this)
which should open a new tab in your browser populated with the documentation
"home page". On Linux, you can generally do the same by running

.. code:: sh

   $ xdg-open ./BuildProducts/share/Falaise-<VERSION>/Documentation/API/html/index.html
..

though ``xdg-open`` may not always be present (``gnome-open`` may be used
instead, for example).

If you need to install Falaise, after building you can run

.. code:: sh

   $ make install
..

to install everything in a standard POSIX style hierarchy under the directory
passed as ``CMAKE_INSTALL_PREFIX``.


Troubleshooting
===============
WIP

Using Falaise
=============

To be done.

Developing Falaise
==================

To be done.
