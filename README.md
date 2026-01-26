Simple Geant4 simulation of a 241Am source and a CCD (back-illuminated).
Tested against geant4-v11.3.2

Parameters:
* CCD is 6k x 1.5k, 15um x 15 um pixels, 650 um thick. Fano factor = 0.115, ionization energy = 3.74 eV
* Deadlayer/PCC approximated by 2um of inactive Si
* 241Am source encapsulated in 1-mm plastic bead to block alphas
* Aluminum cover (2.5mm thick) is placed between the source and the CCD


To build/run:

[0] Make sure you've sourced geant4.sh (or equivalent)
[1] In the Am241_Sim directory: mkdir build; cd build
[2] cmake ..
[3] make -j4
[4] ./Am241_Sim ../run.mac

In run.mac, you can change the number of decays to simulate: 
/run/beamOn 100000

Will write output events to a .csv file. Simulation assumes F = 0.115 and
ionization energy = 3.74 eV. Number of electron/hole pairs produced has
a standard deviation of sqrt(F*Edep/3.74 eV).

Output .csv puts the energies (and number of e/h pairs) on a rectangular grid.
Each EventID corresponds to a single 241Am decay. A given EventID may have >1
energy depositions (e.g., from gamma/x-ray cascade in 241Am/237Np).

Conversion to source activity: a 1-uCi source has activity 3.7e4 Bq.
So simulating 1e6 Am-241 disintegration events --> 27 seconds of exposure time.




