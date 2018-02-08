# arduino-reactor
This Arduino program controls various parameters during the production of Single Walled Carbon Nanotubes (SWNT). It has a simplification for the Modbus Protocol for a EUROTHERM 2416 PID controller. The readout of pressure and flowmeters is done with analog input values.
The output and input of the arduino is simple ASCII plain text at the USB port.

Readout can be directed to a python based visual terminal manged by [python-swnt-reactor](https://github.com/ukos-git/python-swnt-reactor)

# setup
![photos/2017-05-10 17-17-13-1.JPG](https://github.com/ukos-git/arduino-reactor/blob/master/photos/2017-05-10%2017-17-13-1.JPG)
The setup consists of an oven with a centered quarz tube. Inside the quarz pipe the sample is placed. The oven can be moved so that the sample is in the center of the oven during reaction. Reaction (carbon nanotube formation) takes place at 725-900°C with a pressure of around 15mbar.

<img src="https://github.com/ukos-git/arduino-reactor/blob/master/photos/2017-05-10%2017-17-13-4.JPG" alt="sample in quartz tube" width="500px">

The process gas for the Carbon Source is Ethanol, supplied in liquid form and evaporated at 15mbar. Flow is controlled by a Bronkhorst EL-Flow FlowMeter. Digital control of the flowmeter is done in [another project](https://github.com/ukos-git/python-flowmeter).

Reduction of the Catalyst is done via Ar/H<sub>2</sub> gas flow during heat-up.

The Setup can be evacuated to <1mbar and flooded with Argon after reaction.

![process gas wiring](https://github.com/ukos-git/arduino-reactor/blob/master/photos/2017-05-10%2017-17-13-3.JPG)

# arduino wiring.

bread board planning|  final setup
:-------------------------:|:-------------------------:
<img src="https://github.com/ukos-git/arduino-reactor/blob/master/fritzing/swnt-reactor-image.png" alt="wiring fritzing" width="500px">  | <img src="https://github.com/ukos-git/arduino-reactor/blob/master/photos/2017-05-10%2017-17-13-2.JPG" alt="wiring photo" width="500px">



![arduino technical](https://github.com/ukos-git/arduino-reactor/blob/master/fritzing/swnt-reactor-schematic.png)

 
