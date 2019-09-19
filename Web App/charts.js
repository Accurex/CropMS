/*	charts.js
 *	The script that generates charts on the dashboard.
 *	
 * 	Author: Mehmet Eren Aldemir
 * 	Date: July 2019
 * 
 * 	generateCharts(): This function is a driver function to call other chart generation functions.
 * 	fetchAirTemp(): This function fetches the last 10 air temperature values from the database and charts them.
 * 	fetchAirHumidity(): This function fetches the last 10 air humidity values from the database and charts them.
 * 	fetchAirPressure(): This function fetches the last 10 air pressure values from the database and charts them.
 * 	fetchSoilTemperature(): This function fetches the last 10 soil tenperature values from the database and charts them.
 * 	fetchSoilMoisture(): This function fetches the last 10 soil moisture values from the database and charts them.
 * 	fetchLeafMoisture(): This function fetches the last 10 leaf moisture values from the database and charts them.
 * 	fetchLightIntensity(): This function fetches the last 10 light intensity values from the database and charts them.
 * 	fetchUVrate(): This function fetches the last 10 UV rate values from the database and charts them.
 * 	decode(id): This function takes a Firebase timestamp as parameter id and converts it to actual time data and returns it.
 */

/*	This function is a driver function to call other chart generation functions.
 */
function generateCharts() {
	firebase.auth().onAuthStateChanged(function (user) {
		fetchAirTemp();
		fetchAirHumidity();
		fetchAirPressure();
		fetchSoilTemperature();
		fetchSoilMoisture();
		fetchLeafMoisture();
		fetchLightIntensity();
		fetchUVrate();
	});
}

/*	This function fetches the last 10 air temperature values from the database and charts them.
 */
function fetchAirTemp() {
	var seriesArray = new Array();
	var labelsArray = new Array();
	var database = firebase.database();
	var ref = firebase.database().ref('devices/0/temperature').limitToLast(10).once('value').then(
		function (snapshot) {
			for (var key in snapshot.val()) {
				labelsArray.push(decode(key));
				var val = snapshot.val()[key];
				seriesArray.push(val < 60 && val > -30 ? val : null);
			}
			new Chartist.Line('#chart1', {
				labels: labelsArray,
				series: [seriesArray]
			});
		});
}

/*	This function fetches the last 10 air humidity values from the database and charts them.
 */
function fetchAirHumidity() {
	var seriesArray = new Array();
	var labelsArray = new Array();
	var database = firebase.database();
	var ref = firebase.database().ref('devices/0/humidity').limitToLast(10).once('value').then(
		function (snapshot) {
			for (var key in snapshot.val()) {
				labelsArray.push(decode(key));
				var val = snapshot.val()[key];
				seriesArray.push(val <= 100 && val >= 0 ? val : null);
			}
			new Chartist.Line('#chart2', {
				labels: labelsArray,
				series: [seriesArray]
			}, {
					low: 0,
					high: 100
				});
		});
}

/*	This function fetches the last 10 air pressure values from the database and charts them.
 */
function fetchAirPressure() {
	var seriesArray = new Array();
	var labelsArray = new Array();
	var database = firebase.database();
	var ref = firebase.database().ref('devices/0/pressure').limitToLast(10).once('value').then(
		function (snapshot) {
			for (var key in snapshot.val()) {
				labelsArray.push(decode(key));
				var val = snapshot.val()[key];
				seriesArray.push(val <= 1800 && val >= 500 ? val : null);
			}
			new Chartist.Line('#chart3', {
				labels: labelsArray,
				series: [seriesArray]
			});
		});
}

/*	This function fetches the last 10 soil tenperature values from the database and charts them.
 */
function fetchSoilTemperature() {
	var seriesArray = new Array();
	var labelsArray = new Array();
	var database = firebase.database();
	var ref = firebase.database().ref('devices/0/soil-temperature').limitToLast(10).once('value').then(
		function (snapshot) {
			for (var key in snapshot.val()) {
				labelsArray.push(decode(key));
				var val = snapshot.val()[key];
				seriesArray.push(val <= 60 && val >= -5 ? val : null);
			}
			new Chartist.Line('#chart4', {
				labels: labelsArray,
				series: [seriesArray]
			});
		});
}

/*	This function fetches the last 10 soil moisture values from the database and charts them.
 */
function fetchSoilMoisture() {
	var seriesArray = new Array();
	var labelsArray = new Array();
	var database = firebase.database();
	var ref = firebase.database().ref('devices/0/soil-moisture').limitToLast(10).once('value').then(
		function (snapshot) {
			for (var key in snapshot.val()) {
				labelsArray.push(decode(key));
				var val = snapshot.val()[key];
				seriesArray.push(val <= 4096 && val >= 0 ? val : null);
			}
			new Chartist.Line('#chart5', {
				labels: labelsArray,
				series: [seriesArray]
			});
		});
}

/*	This function fetches the last 10 leaf moisture values from the database and charts them.
 */
function fetchLeafMoisture() {
	var seriesArray = new Array();
	var labelsArray = new Array();
	var database = firebase.database();
	var ref = firebase.database().ref('devices/0/leaf-moisture').limitToLast(10).once('value').then(
		function (snapshot) {
			for (var key in snapshot.val()) {
				labelsArray.push(decode(key));
				var val = snapshot.val()[key];
				seriesArray.push(val <= 4096 && val >= 0 ? val : null);
			}
			new Chartist.Line('#chart6', {
				labels: labelsArray,
				series: [seriesArray]
			});
		});
}

/*	This function fetches the last 10 light intensity values from the database and charts them.
 */
function fetchLightIntensity() {
	var seriesArray = new Array();
	var labelsArray = new Array();
	var database = firebase.database();
	var ref = firebase.database().ref('devices/0/light-intensity').limitToLast(10).once('value').then(
		function (snapshot) {
			for (var key in snapshot.val()) {
				labelsArray.push(decode(key));
				var val = snapshot.val()[key];
				seriesArray.push(val <= 4096 && val >= 0 ? val : null);
			}
			new Chartist.Line('#chart7', {
				labels: labelsArray,
				series: [seriesArray]
			});
		});
}

/*	This function fetches the last 10 UV rate values from the database and charts them.
 */
function fetchUVrate() {
	var seriesArray = new Array();
	var labelsArray = new Array();
	var database = firebase.database();
	var ref = firebase.database().ref('devices/0/uv-intensity').limitToLast(10).once('value').then(
		function (snapshot) {
			for (var key in snapshot.val()) {
				labelsArray.push(decode(key));
				var val = snapshot.val()[key];
				seriesArray.push(val <= 25 && val >= 0 ? val : null);
			}
			new Chartist.Line('#chart8', {
				labels: labelsArray,
				series: [seriesArray]
			});
		});
}

/*	This function takes a Firebase timestamp as parameter id and converts it to actual time data and returns it.
 */
function decode(id) {
	var PUSH_CHARS = "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";
	id = id.substring(0, 8);
	var timestamp = 0;
	for (var i = 0; i < id.length; i++) {
		var c = id.charAt(i);
		timestamp = timestamp * 64 + PUSH_CHARS.indexOf(c);
	}
	var dd = new Date(timestamp);
	retval = dd.getDate() + "/" + dd.getMonth() + " " + dd.getHours() + ":" + dd.getMinutes();
	return retval;
}