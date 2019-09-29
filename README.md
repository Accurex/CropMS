# CropMS :herb: <sub><sup> Modular, affordable, and internet-connected crop &amp; greenhouse monitoring system. </sup></sub>

This is a device I made using a NodeMCU ESP-32S to actively monitor the stats which can affect crop development. To keep it affordable for hobbyists, I designed it to be consist of different modules so that one can purchase only the modules he/she considers important. Currently, the overall cost of the system including each sensor is about $30.

Things that system logs are as the following:
* Air temperature
* Air humidity
* Air pressure
* Light intensity
* UV intensity
* Soil temperature
* Soil moisture
* Leaf moisture

In addition to these features, the system can also perform outputting actions such as controlling its integrated plug, triggering its internal alarm, or output digital and analog signals. The system can also be set to do these actions on certain conditions. For example, an irrigation system can be automatically triggered when the soil moisture is low.

The system displays the data it logged on a nicely designed, simple user interface which can be accessed using a web browser. The interface also gives users options to set the triggering events. All the data are read from and written to a Firebase realtime database.

After developing and manufacturing the system, I presented it to hobbyists and farmers and asked them to rate the feasibility of the device, and received pretty positive feedbacks. Although feedbacks are positive, I think that there are still things which can be improved such as the device's form factor, or the manufacturing quality (by the use of PCBs). So this a **_work in progress_**.

I also wrote a research paper covering all the details, from implementation to testing and results (including the ratings of the testers). You can find it [here](https://www.academia.edu/40470846/Modular_and_Affordable_Crop_Monitoring_System).
