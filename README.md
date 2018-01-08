# PFC(Personal Food Computer) by KgwangHee Han.


![Capture Shots](https://github.com/housekorea/pfc/blob/master/doc/s_capture_4.jpeg) 

This repository for purpose distribute to others who have a interests about the PFC. It first developed by MIT Media Lab Open Inititiative.Thanks to their efforts, i can started this exciting projects(Along with i believe that we have a possibility to beacame #NerdFarmer!).If you want make a PFC, absolutely you can fork this repsitory.(I hope that this repository helpful!) 

It is different some hardware/software between my personal PFC prototype and MIT original prototype. But it is challengable work to localizing in S.Korea for generalizng on here(S.Korea) 

This repository have a four types of directory.(Probably It would expanding now, whenever i have a time to writing about the more detail guide for PFC makers, i will summarize about the PFC and their philosophy and mechanism(with Vision too). 

I believe that PFC project is one of stepping stones for solving problem which we are facing about rapidly urbanziation and food. Our NerdFarmer, Farming using with a Information Technology than shoval and pick.

<hr>

### first, Sensor directory.

 This directory have a sourcecode to commnuicate with Rapsberry PI ans Arduion through Serial Communication. It means that, this codes more familiar with ordering to the arudino.(It is not include any directely relate code for arduino sensors. If you want check a code which relate in Arduino, check the Arduino Directory. 
 
**Sensor List**

1. DHT11 (Air temperature, Air Humidity)
2. DS18B20 Water Proof sensor
3. CO2 Sensor
4. PhotoResistor(LDR)
5. EC(Electricity Conductivity) Sensor
6. PH Probe
7. Samsung USB camera
8. Water level sensor (Comming Soon)
9. Water Flow sensor (Comming Soon)

### second, Actuator directory.
 
 This directory have a sourcecode related in "Actuators", In the my PFC project context, this word "Actuators" means machine or any instrument for making Enviroment of growing plants. Let me give an example, PFC simulate natural wind to using FAN cooler, and for giving light like a sun light, we usually using LED bulb. We can call this, "Actuators" for FAN,LED or anything which verifying purpose i mentioned. 
 
**Acutator List**

1. LED
2. Air circulration FAN
3. Ventilation FAN
4. Air pump
5. Peristaltic Pump For Solution A
6. Peristaltic Pump For Solution B
7. Peristaltic Pump For Water
8. Peristaltic Pump For PH(-) (Comming Soon)
9. Peristaltic Pump For PH(+) (Comming Soon)
10. Solenoid Valve for each Peristaltic Pump (Considerable)
11. Chiler Unit (Considerable)

### thrid, Computer Vision directory.

 I have a great expectation about the role of **"Computer Vision"**, it is give a opportunity on the various ragne from  recognize about the any symptom for disease to measuring crops size,status too. In this time i used openCV3 library with python2 programing language. You can check my sourcecode relate this image processing for detecting plant and measuring their size using openCV library. At the same time, our openAG forum opend a specific thread to discuss for how to making this image processing to important role of our PFC model. You can check various and great ideas of usecase "Computer Vision". 
 Probably, on my PFC's computer vision headed to the next stage for Machine Learning more specialize Image Processing. I hope that a number of smart guys joined this Computer Vision thread, and sharing their knowledge and experience through cooperate.

Computer Vision Thread on openAG forum : http://forum.openag.media.mit.edu/t/cv-and-image-processing-for-the-pfc/273/18

### fourth, Arduino Code directory.

 If you want find arduino codes for making PFC? Exploring this "Arduino" Directory, this directory is only space for mananging Arduino Sourcecodes. I warned to you about the hardcode on my sourcodes. Until now, i used "Arduino Mega 2560" microcontroller board Model, this sourcode only specialized this specific microcontroller board. A number of GPIO(General Purpose Input/Output Pin) managed by "Constant Variable" on my Arduino Sourcecode. 
 
 After that, I would write about the electronics design circuit about my PFC codes : ). Absolutely i have to fight my inner laziness too!

### fifth, Database ERD

Data is key of PFC project, what kind of any version or custom of your PFC, absoltuely next stage(we are headed) is Machine Learning through "Data". For desing of Database, i have to consider some chracteristic about this project. It mean that, i have to aussme that parrell multiple PFC operate at the same time. But remote database server have a one address which other Prototype can be reach to this address, and it have a flexible and scalibility(i intended to make a "Meta Table of each sensor/actuator/particpant/models ...), this meta table have a key/value architecture, it would give us to flexsible but hard to using a wild joinning or exploring between meta tables.(I hope that a new "Search Technology" solving this problem. Such as ElasticSearch,Titan DB graph ...) 

**This ERD not finished for archiving PFC sensor data, it is continuing now, i would welcome any idea or suggestion!**

![Initial Database Schema ERD](https://github.com/housekorea/pfc/blob/master/doc/pfc_erd.svg) 

Any question or suggestion welcomed on my repository.

<hr>
#NerdFarmer,

> KgwangHee Han(House)

> E-mail : house9737@gmail.com

Enjoying Farming guys!

**P.s 
If you are live in S.Korea, and you have a intereset in making a PFC in S.korea. Join Us through this Thread. I want contribute with others about the belows issue on our location(South Korea.)**

1. How to sourcing alternative parts in S.kora local market.
2. How to break the language barrier of the documentation of MIT PFC's repository and fourm.(Until now, about any PFC documentation not translated in Korean language. Probably it is one of the valuable activity for our Elementary,Middle schoole students to joining PFC project.)
3. Solving problem origined by Local issue which such as "Government Rule", "Local Traditional Culture", "Recogintion of Farmers". 

Doesn't matter any issue which we are facing, i have a optimistic about the "We can solving this issue, and developing our agriculture on S.korea! Absolutely, we can helping others on the global too".


http://forum.openag.media.mit.edu/t/openag-in-south-korea/2801/16

(Above, Url link is a S.Korea Thread to talking about the any issue or communication in the S.Korea)
