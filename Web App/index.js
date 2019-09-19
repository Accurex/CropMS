/*	index.js
 *	Main script that makes the web dashboard functioning.
 *	
 * 	Author: Mehmet Eren Aldemir
 * 	Date: July 2019
 * 
 * 	saveOutVar(): This function saves the user's output variable choice to the database.
 * 	saveOMState(): This function saves the user's output module state choice to the database.
 * 	saveOutVarThreshold(): This function saves the user's output variable threshold choice to the database.
 * 	saveBuzzerEnabled(): This function saves the user's buzzer feature choice to the database.
 * 	saveBuzzerVar(): This function saves the user's buzzer variable choice to the database.
 * 	saveBuzzerVarThreshold(): This function saves the user's buzzer variable threshold choice to the database.
 * 	saveRelayEnabled(): This function saves the user's relay feature choice to the database.
 * 	saveRelayVar(): This function saves the user's relay variable choice to the database.
 * 	saveRelayVarThreshold(): This function saves the user's relay threshold choice to the database.
 * 	relayOn(): This function saves the user's explicit turn on the relay action to the database.
 * 	relayOff(): This function saves the user's explicit turn off the relay action to the database.
 * 	login(): This function logs the user in.
 * 	logout(): This function logs the user out.
 */

/*	This section shows or hides the main dashboard according to whether the user is signed in or not.
 */
firebase.auth().onAuthStateChanged(function (user) {
	if (user) {
		document.getElementById("user_div").style.display = "block";
		document.getElementById("login_div").style.display = "none";
		var user = firebase.auth().currentUser;
		if (user != null) {
			var email_id = user.email;
			document.getElementById("user_para").innerHTML = "Current user: " + email_id;
		}
	} else {
		document.getElementById("user_div").style.display = "none";
		document.getElementById("login_div").style.display = "block";
	}
});

/*	This function saves the user's output variable choice to the database.
 */
function saveOutVar() {
	var selection = document.querySelector('input[name="out-var"]:checked').value;
	var database = firebase.database();
	var ao = database.ref('devices/0/output-var');
	ao.set(parseInt(selection));
}

/*	This function saves the user's output module state choice to the database.
 */
function saveOMState() {
	var selection = document.querySelector('input[name="om-state"]:checked').value;
	if (selection <= 1 && selection >= 0) {
		var database = firebase.database();
		var ao = database.ref('devices/0/om-state');
		ao.set(selection == "1" ? false : true);
	}
}

/*	This function saves the user's output variable threshold choice to the database.
 */
function saveOutVarThreshold() {
	var selection = document.querySelector('input[name="outvar-threshold"]').value;
	var database = firebase.database();
	var ao = database.ref('devices/0/output-threshold');
	ao.set(parseInt(selection));
}

/*	This function saves the user's buzzer feature choice to the database.
 */
function saveBuzzerEnabled() {
	var selection = document.querySelector('input[name="is-buzzer-enabled"]:checked').value;
	var database = firebase.database();
	var ao = database.ref('devices/0/buzzer-en');
	ao.set(parseInt(selection) == 1 ? true : false);
}

/*	This function saves the user's buzzer variable choice to the database.
 */
function saveBuzzerVar() {
	var selection = document.querySelector('input[name="buzzer-var"]:checked').value;
	var database = firebase.database();
	var ao = database.ref('devices/0/buzzer-var');
	ao.set(parseInt(selection));
}

/*	This function saves the user's buzzer variable threshold choice to the database.
 */
function saveBuzzerVarThreshold() {
	var selection = document.querySelector('input[name="buzzer-threshold"]').value;
	var database = firebase.database();
	var ao = database.ref('devices/0/buzzer-threshold');
	ao.set(parseInt(selection));
}

/*	This function saves the user's relay feature choice to the database.
 */
function saveRelayEnabled() {
	var selection = document.querySelector('input[name="is-relay-enabled"]:checked').value;
	var database = firebase.database();
	var ao = database.ref('devices/0/relay-en');
	ao.set(parseInt(selection) == 1 ? true : false);
}

/*	This function saves the user's relay variable choice to the database.
 */
function saveRelayVar() {
	var selection = document.querySelector('input[name="relay-var"]:checked').value;
	var database = firebase.database();
	var ao = database.ref('devices/0/relay-var');
	ao.set(parseInt(selection));
}

/*	This function saves the user's relay threshold choice to the database.
 */
function saveRelayVarThreshold() {
	var selection = document.querySelector('input[name="relay-threshold"]').value;
	var database = firebase.database();
	var ao = database.ref('devices/0/relay-threshold');
	ao.set(parseInt(selection));
}

/*	This function saves the user's explicit turn on the relay action to the database.
 */
function relayOn() {
	var database = firebase.database();
	var ao = database.ref('devices/0/relay-state');
	var dd = database.ref('devices/0/relay-en');
	ao.set(true);
	dd.set(false);
}

/*	This function saves the user's explicit turn off the relay action to the database.
 */
function relayOff() {
	var database = firebase.database();
	var ao = database.ref('devices/0/relay-state');
	var dd = database.ref('devices/0/relay-en');
	ao.set(false);
	dd.set(false);
}

/*	This function logs the user in.
 */
function login() {
	var userEmail = document.getElementById("email_field").value;
	var userPass = document.getElementById("password_field").value;
	firebase.auth().signInWithEmailAndPassword(userEmail, userPass).catch(function (error) {
		var errorCode = error.code;
		var errorMessage = error.message;
		window.alert("Error : " + errorMessage);
	});
}

/*	This function logs the user out.
 */
function logout() {
	firebase.auth().signOut();
}
