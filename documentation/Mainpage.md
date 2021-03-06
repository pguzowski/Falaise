Welcome to the Falaise Documentation {#mainpage}
====================================
Falaise is the software system for the SuperNEMO experiment. These
pages document the usage of the core applications, together with guides
on extending their functionality and the full C++ API of the Falaise
extension system.

Getting Started
===============
If you're reading this online and don't yet have Falaise installed, an
[Installation Guide](@ref flinstallguide) is available. Note that
at present Falaise is only supported on POSIX platforms, and even here
not all variants are guaranteed to work!

Core Applications and Libraries
===============================
Once you have an install of Falaise, the following pages provide
introductions to using and extending the core applications:

- [FLSimulate](@ref usingflsimulate): guide for using the Geant4 based simulations of the SuperNEMO Demonstrator and Commissioning detector setups.
- [FLReconstruct](@ref usingflreconstruct) guide for using the pipeline application for reading, reconstructing and outputting data generated by the FLSimulate detector simulation.
  - [Data Structures output by FLReconstruct](@ref flreconstructpipelineoutput)
  - [Converting FLReconstruct BRIO Format to ROOT](@ref usingflptd2root)
  - [Customizing the pipeline](@ref writingflreconstructpipelinescripts) for `flreconstruct`.
  - [Writing plugin modules](@ref writingflreconstructmodules) for `flreconstruct`.
  - [Working with Run/Event Data in plugin modules](@ref workingwitheventrecords).
  - [Using Services to access metadata](@ref usingservices).
- [FLVisualize](@ref usingflvisualize) guide for viewing simulated and reconstructed data.

Developing Falaise
==================
Contributions to the core of Falaise are welcome. To begin developing,
you should familiarize yourself with the [tools, languages and concepts](@ref developingfalaise) of its architecture and development model.

User Guides
===========
Documents detailing the use and design of Falaise may be found on
under the Related Pages tab if you are reading the HTML documentation,
or in the following sections in the printed manual.
