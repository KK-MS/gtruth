# src
Source code of each module.

* Module is **independent** in terms of compilation and execution.
* Module's input and output is listed below.
* Module's **unit test** is available in test/ tool/ script/

## src structure
**src/ structure**

     src/ contains source code of the modules.
     |- README    | Readme file in .md format |
     |- imu/      | IN: SatGPS, IMU, localized GPS | OUT: Timestamp, IMU, SatGPS |
     |- stereo/   | IN: Timestamp, IMU, SatGPS | OUT: Timestamp, IMU, SatGPS, Stereo |
     |- localize/ | IN: Timestamp, IMU, SatGPS, Stereo | OUT: Timestamp, LoGPS |
     |- gtmap/    | IN: IMU, SatGPS | OUT: list of objects <desc, GtGps> |
     Note: SaGPS: Satellite GPS coordinates
           LoGPS: Localized GPS coordinates
           GtGPS: Ground truth map GPS coordinates 

