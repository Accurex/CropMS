/*  Structures.h
 *  The header file that contains the data structures for the embedded
 *  code of the NodeMCU ESP32-S for the Modular and Affordable Crop 
 *  Monitoring System resarch project.
 * 
 *  Author: Mehmet Eren Aldemir
 *  Date: June 2019 - July 2019
 *  
 *  This file includes the code that runs on the Main Control Unit of the system.
 * 	It contains the code to take measurements from the sensor modules, conduct output
 * 	tasks, upload data to and fetch data from database.
 */

#ifndef A_H
#define A_H

struct meteorologicalData {		// Structure for meteorological data.
    float temperature;
    float pressure;
    float humidity;
};

struct ambientData {			// Structure for ambient data.
    float uvRate;
    float lightIntensity;
};

struct cropData {				// Structure for crop data.
    float soilMoisture;
    float soilTemperature;
    float leafMoisture;
};

struct dataPacket {				// Structure for combined and packed sensor data.
    struct meteorologicalData meteorological;
    struct ambientData ambient;
    struct cropData crop;
};

enum outputData {				// Setting structure for output variables.
    NONE,
    MTEMP,
    MHUMID,
    MPRESSURE,
    CTEMP,
    CMOIST,
    CLEAF,
    ALIT,
    AUV
};

struct deviceSettings {			// Structure for storing the device settings.
    int buzzerVar;
    int outVar;
    int relayVar;
    bool isBuzzerEnabled;
    bool isRelayThresholdEnabled;
    bool relayState;
    bool omState;
    float buzzerThreshold;
    float relayThreshold;
    float outThreshold;
};

#endif