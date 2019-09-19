/*  app.ino
 *  The embedded code of the NodeMCU ESP32-S for the Modular and Affordable
 * 	Crop Monitoring System resarch project.
 * 
 *  Author: Mehmet Eren Aldemir
 *  Date: June 2019 - July 2019
 *  
 *  This file includes the code that runs on the Main Control Unit of the system.
 * 	It contains the code to take measurements from the sensor modules, conduct output
 * 	tasks, upload data to and fetch data from database.
 * 
 * 	Functions:
 * 	-----------------------------------
 * 	setup() : This function decides the flow of the program and basically runs each time after device wakes up. It drives all the main tasks.
 * 	loop() : The loop() function is not used in this code as all the flow is implemented in the setup() with sleep functions.
 *  uploadRoutine() : This function calls the function to gather all the sensor data, initiates the Firebase connection, and then uploads all the measured data to the database.
 * 	fetchSettings() : This function reads all the user settings from the Firebase database and returns them.
 * 	uploadData(struct dataPacket dt) : This function takes a dataPacket as parameter and uploads the dataPacket passed to it to the Firebase database.
 * 	initConnection() : This function initiates the Wi-Fi connection of the device and then connects to the Firebase database.
 *	getAmbientData() : This function collects the ambient data from the Ambient Module and returns it.
 * 	getCropData() : This function collects the crop data from the Crop Module and returns it.
 * 	getMeteorologicalData() : This function collects the meteorological data from the Meteorological Module and returns it.
 * 	getAllData() : This function calls all the measurements functions and packs the received data into one structure.
 * 	getVariable(int outData) : This function converts the variable number for outputs fetched from the database into actual values of the variables. It takes the number as the parameter.
 *	updateOutputs() : This function updates the OM's outputs according to the actual data it is configured to depend on.
 *	averageAnalogRead(int pinToRead) : This function takes the average of several readings from a pin. It takes the pin number to be read as a parameter. Source: https://learn.sparkfun.com/tutorials/ml8511-uv-sensor-hookup-guide/all
 *	This function scales the argument x from the range in_min to in_max to the rane out_min to out_max. Source: http://forum.arduino.cc/index.php?topic=3922.0
 */

#include <analogWrite.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <DHT.h>
#include <HTTPClientESP32Ex.h>
#include <FirebaseESP32.h>
#include "Structures.h"
#include <Wire.h>

#define CD_SOIL_MOISTURE_PIN 27
#define CD_SOIL_TEMPERATURE_PIN 26
#define CD_LEAF_MOISTURE_PIN 14
#define REF_5V 33
#define AD_3V3_REF 34
#define AD_LDR_PIN 39
#define AD_UV_PIN 36
#define MD_DHT_PIN 15
#define OM_DO_PIN 0
#define OM_AO_PIN 2
#define OM_RELAY_PIN 18
#define OM_BUZZER_PIN 19
#define OFFPWR_SLEEP_TIME 720
#define DBG_SLEEP_TIME 10
#define uS_FACTOR 1000000

enum deviceState {					 	// Setting for storing whether the device draws power from the OM or not.
	ONPWR,
	OFFPWR
};

FirebaseData firebaseData;				// Variable for storing the data to be uploaded and the data fetched.
struct deviceSettings devcSettings;		// Variable for storing the up-to-date fetched device settings.
deviceState devcState = ONPWR; 			// Variable for storing the up-to-date fetched OM-connection state.
dataPacket data;						// Variable for storing all the sensor data.

/*	This function decides the flow of the program and basically runs each time after device wakes up. It drives all the main tasks.
 */
void setup() {
	pinMode(OM_DO_PIN, OUTPUT);			// Define the OM Digital Out pin as output.
	pinMode(OM_BUZZER_PIN, OUTPUT);		// Define the OM Buzzer pin as output.
	pinMode(OM_AO_PIN, OUTPUT);			// Define the OM Analog Out pin as output.
	pinMode(OM_RELAY_PIN, OUTPUT);		// Define the OM Relay Control pin as output.
	esp_sleep_enable_timer_wakeup(OFFPWR_SLEEP_TIME * uS_FACTOR);		// Set the system sleep duration in microseconds.
	uploadRoutine();					// Take measurements and upload all the data.
	while (devcState == ONPWR) {		// While device is getting power from the OM
		devcSettings = fetchSettings(); // Fetch the up-to-date settings from the database.
		if (devcSettings.omState) devcState = ONPWR;				// Set the device power state as ONPWR if the OM is connected.
		else {
			devcState = OFFPWR;			// Set it as OFFPWR otherwise.
			esp_deep_sleep_start();		// Sleep. (After waking up it starts from the beginning of the setup())
		}
		for (int i = 0; i < 6; i++) {	// For 6 times
			data = getAllData();		// Take measurements from sensors.
			updateOutputs();			// Update the OM outputs according to the measurements.
			delay(30000);				// Stop 30 seconds.
		}
		uploadRoutine();				// Take measurements and upload all the data.
	}
	esp_deep_sleep_start();				// Sleep. (After waking up it starts from the beginning of the setup())
}

/*	The loop() function is not used in this code as all the flow is implemented in the setup() with sleep functions.
 */
void loop() {
}

/*	This function calls the function to gather all the sensor data, initiates the Firebase connection,
 *	and then uploads all the measured data to the database.
 */
void uploadRoutine() {
	data = getAllData();	// Get all sensor data.
	initConnection();		// Init Wi-Fi and Firebase connections.
	uploadData(data);		// Upload all the data to Firebase.
}

/*	This function reads all the user settings from the Firebase database
 * 	and returns them.
 */
struct deviceSettings fetchSettings() {
	struct deviceSettings retSettings;									// Define variable for storing the read settings.
	Firebase.getInt(firebaseData, "/devices/0/buzzer-var");				// Fetch buzzer variable.
	retSettings.buzzerVar = firebaseData.intData();						// Read it and assign to retSettings.
	Firebase.getInt(firebaseData, "/devices/0/output-var");				// Fetch the output variable.
	retSettings.outVar = firebaseData.intData();						// Read it and assign to retSettings.
	Firebase.getInt(firebaseData, "/devices/0/relay-var");				// Fetch the relay variable.
	retSettings.relayVar = firebaseData.intData();						// Read it and assign to retSettings.
	Firebase.getBool(firebaseData, "/devices/0/buzzer-en");				// Fetch if the buzzer alarming feature is enabled.
	retSettings.isBuzzerEnabled = firebaseData.boolData();				// Read it and assign to retSettings.
	Firebase.getBool(firebaseData, "/devices/0/relay-en");				// Fetch if the relay feature is enabled.
	retSettings.isRelayThresholdEnabled = firebaseData.boolData();		// Read it and assign to retSettings.
	Firebase.getBool(firebaseData, "/devices/0/relay-state");			// Fetch if user has explicitly set a relay state.
	retSettings.relayState = firebaseData.boolData();					// Read it and assign to retSettings.
	Firebase.getBool(firebaseData, "/devices/0/om-state");				// Fetch if the OM is connected or not.
	retSettings.omState = firebaseData.boolData();						// Read it and assign to retSettings.
	Firebase.getFloat(firebaseData, "/devices/0/buzzer-threshold");		// Fetch the buzzer alarm threshold.
	retSettings.buzzerThreshold = firebaseData.floatData();				// Read it and assign to retSettings.
	Firebase.getFloat(firebaseData, "/devices/0/relay-threshold");		// Fetch the relay threshold.
	retSettings.relayThreshold = firebaseData.floatData();				// Read it and assign to retSettings.
	Firebase.getFloat(firebaseData, "/devices/0/output-threshold");		// Fetch the digital output threshold.
	retSettings.outThreshold = firebaseData.floatData();				// Read it and assign to retSettings.

	return retSettings;													// Return the up-to-date settings.
}

/*	This function takes a dataPacket as parameter and uploads
 *	the dataPacket passed to it to the Firebase database.
 */
void uploadData(struct dataPacket dt) {
	Firebase.pushFloat(firebaseData, "/devices/0/temperature", dt.meteorological.temperature);	// Upload the air temperature.
	Firebase.pushFloat(firebaseData, "/devices/0/pressure", dt.meteorological.pressure);		// Upload the air pressure.
	Firebase.pushFloat(firebaseData, "/devices/0/humidity", dt.meteorological.humidity);		// Upload the air humidity.

	Firebase.pushFloat(firebaseData, "/devices/0/light-intensity", dt.ambient.lightIntensity);	// Upload the ambient light level.
	Firebase.pushFloat(firebaseData, "/devices/0/uv-intensity", dt.ambient.uvRate);				// Upload the UV level.

	Firebase.pushFloat(firebaseData, "/devices/0/soil-temperature", dt.crop.soilTemperature);	// Upload the soil temperature.
	Firebase.pushFloat(firebaseData, "/devices/0/soil-moisture", dt.crop.soilMoisture);			// Upload the soil moisture.
	Firebase.pushFloat(firebaseData, "/devices/0/leaf-moisture", dt.crop.leafMoisture);			// Upload the leaf moisture.
}

/*	This function initiates the Wi-Fi connection of the device and then connects
 *	to the Firebase database.
 */
void initConnection() {
	WiFi.begin("yourSSID", "yourPassword");		// Initiate Wi-Fi connection to the network named yourSSID with yourPassword
	while (WiFi.status() != WL_CONNECTED) {		// If Wi-Fi connection is not successful
		delay(500);								// Try until it connects.
	}
	Firebase.begin("greenhouse-sys.firebaseio.com", "apiSecret");		// Connect to Firebase Realtime Database with the specified URL and apiSecret.
	Firebase.reconnectWiFi(true);										// Set Wi-Fi reconnection setting.
	Firebase.setMaxRetry(firebaseData, 3);								// Set maximum retries of uploading if anything fails.
	Firebase.setMaxErrorQueue(firebaseData, 30);						// Set maximum error queue.
	Firebase.enableClassicRequest(firebaseData, true);					// Enable classic requests.
}

/*	This function collects the ambient data from the Ambient Module and returns it.
 */
struct ambientData getAmbientData() {
	struct ambientData retData;			// Create the data structure to be returned.
	retData.uvRate = -404.0;			// Set to default error value.
	retData.lightIntensity = -404.0;	// Set to default error value.
	
	retData.uvRate = mapFloat(3.3 / averageAnalogRead(AD_3V3_REF) * averageAnalogRead(AD_UV_PIN), 0.99 * 4, 2.8 * 4, 0.0, 15.0);	// Read the input, compensate it with the system working voltage, and assign it to the return variable.
	retData.lightIntensity = analogRead(AD_LDR_PIN);	// Read the input and assign it to the return variable.

	return retData;		// Return the data.
}

/*	This function collects the crop data from the Crop Module and returns it.
 */
struct cropData getCropData() {
	struct cropData retData;            // Create the data structure to be returned.
	retData.soilMoisture = -404.0;      // Set to default error value.
	retData.soilTemperature = -404.0;   // Set to default error value.
	retData.leafMoisture = -404.0;      // Set to default error value.

	retData.soilMoisture = analogRead(CD_SOIL_MOISTURE_PIN);	// Measure the soil moisture.
	retData.leafMoisture = analogRead(CD_LEAF_MOISTURE_PIN);	// Measure the leaf moisture.

	float vin, val, temp, vout;							// Define variables for accurate voltage reading.
	vin = averageAnalogRead(REF_5V);					// Measure the actual voltage at 5V pin.
	val = averageAnalogRead(CD_SOIL_TEMPERATURE_PIN);	// Measure the value at the soil temperature pin.
	temp = vin * val;									// 
	vout = temp / 4096.0;								// Calculate the voltage by compensating the offset in the actual system voltage.
	temp = (vin / vout) - 1;							//
	retData.soilTemperature = (217.0 * temp);    		// Finally determine the soil temperature.
	return retData;     // Return the data.
}

/*	This function collects the meteorological data from the Meteorological Module and returns it.
 */
struct meteorologicalData getMeteorologicalData() {
	DHT_Unified dht(MD_DHT_PIN, DHT11);   	// Define DHT22 sensor.
	Adafruit_BMP280 bmp;					// Define the BMP280 sensor.

	struct meteorologicalData retData;      // Create the data structure to be returned.
	retData.temperature = -404.0;           // Set to default error value.
	retData.pressure = -404.0;              // Set to default error value.
	retData.humidity = -404.0;              // Set to default error value.

	dht.begin();							// Begin DHT sensor measurement phase.
	sensors_event_t event;					// Define a sensor event.
	dht.temperature().getEvent(&event);		// Get the temperature measurement into the defined event.
	if (!isnan(event.temperature))			// If it is not garbage (i.e. an actual number)
		retData.temperature = event.temperature;	// Assign it to the return structure.
	dht.humidity().getEvent(&event);		// Get the temperature measurement into the defined event.
	if (!isnan(event.relative_humidity))	// If it is not garbage (i.e. an actual number)
		retData.humidity = event.relative_humidity;	// Assign it to the return structure.

	bmp.begin();									// Begin DHT sensor measurement phase.
	retData.pressure = bmp.readPressure() / 100;	// Read the pressure and divide it by 100 to convert it to hPa.

	return retData;		// Return the data.
}

/*	This function calls all the measurements functions and packs the received data into one structure.
 */
struct dataPacket getAllData() {
	struct dataPacket dt;							// Create the data structure to be returned.
	dt.meteorological = getMeteorologicalData();	// Save the meteorological data into the structure.
	dt.ambient = getAmbientData();					// Save the ambient data into the structure.
	dt.crop = getCropData();						// Save the crop data into the structure.
	return dt;										// Return the data.
}

/*	This function converts the variable number for outputs fetched from the database into actual values of the variables.
 * 	It takes the number as the parameter.
 */
float getVariable(int outData) {
	float retVal = 0;		// Define the return variable.
	switch (outData) {		// Decide the value by a switch-case statement.
		case 0:
			break;
		case 1:
			retVal = data.meteorological.temperature;
			break;
		case 2:
			retVal = data.meteorological.humidity;
			break;
		case 3:
			retVal = data.meteorological.pressure;
			break;
		case 4:
			retVal = data.crop.soilTemperature;
			break;
		case 5:
			retVal = data.crop.soilMoisture;
			break;
		case 6:
			retVal = data.crop.leafMoisture;
			break;
		case 7:
			retVal = data.ambient.lightIntensity;
			break;
		case 8:
			retVal = data.ambient.uvRate;
			break;
	}
	return retVal;		// Return the data.
}

/*	This function updates the OM's outputs according to the actual data it is configured to depend on.
 */
void updateOutputs() {
	float oVar, bVar, rVar;						// Define the output variable, buzzer variable, and relay variable respectively.
	oVar = getVariable(devcSettings.outVar);	// Get the actual value of the output variable.
	bVar = getVariable(devcSettings.buzzerVar);	// Get the actual value of the buzzer variable.
	rVar = getVariable(devcSettings.relayVar);	// Get the actual value of the relay variable.

	analogWrite(OM_AO_PIN, oVar);								// Output the variable using analog output.
	digitalWrite(OM_DO_PIN, oVar > devcSettings.outThreshold);	// Output the threshold state using the digital output.

	if (devcSettings.isBuzzerEnabled && bVar > devcSettings.buzzerThreshold) {	// If the buzzer is enabled and the threshold is passed
		digitalWrite(OM_BUZZER_PIN, HIGH);										// Beep
		delay(8000);															// for 8 seconds.
	}
	digitalWrite(OM_BUZZER_PIN, LOW);											// Stop beeping.

	if ((devcSettings.isRelayThresholdEnabled && rVar > devcSettings.relayThreshold) || devcSettings.relayState)	// If the relay threshold is passed or if the user explicitly set a relay state
		digitalWrite(OM_RELAY_PIN, HIGH);	// Set relay on
	else									// Otherwise
		digitalWrite(OM_RELAY_PIN, LOW);	// Set it off.
}

/*	This function takes the average of several readings from a pin. It takes the pin number to be read as a parameter.
 *	Source: https://learn.sparkfun.com/tutorials/ml8511-uv-sensor-hookup-guide/all
 */
int averageAnalogRead(int pinToRead) {
	byte numberOfReadings = 8;		// Set the number of readings.
	unsigned int runningValue = 0; 	// Initialize the sum variable.
	for (int x = 0; x < numberOfReadings; x++)	// For numberOfReadings time
		runningValue += analogRead(pinToRead);	// Sum the readings.
	runningValue /= numberOfReadings;			// Divide it by numberOfReadings
	return(runningValue);  			// Return the result.
}

/*	This function scales the argument x from the range in_min to in_max to the rane out_min to out_max.
 *	Source: http://forum.arduino.cc/index.php?topic=3922.0
 */
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;	// Return the scaled x.
}