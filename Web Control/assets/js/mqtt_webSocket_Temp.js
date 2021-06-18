var selectedClientID;
var sensors;
var hasil;

// Called after form input is processed
function startConnect() {
    // Generate a random client ID
    clientID = "clientID-" + "Control Panel";

    // Fetch the hostname/IP address and port number from the form
    host = "test.mosquitto.org";
    port = "8081";
    user = "";
    pass = "";

    // Print output for the user in the messages div
    //document.getElementById("messages").innerHTML += '<span>Connecting to: ' + host + ' on port: ' + port + '</span><br/>';
    //document.getElementById("messages").innerHTML += '<span>Using the following client value: ' + clientID + '</span><br/>';

    // Initialize new Paho client connection
    client = new Paho.MQTT.Client(host, Number(port), clientID);

    //initialize new sensor_listener object
    //sensors = new listener();

    // Set callback handlers
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    // Connect the client, if successful, call onConnect function
    client.connect({ 
        onSuccess: onConnect,
        userName : user,
	    password : pass
    });

}

// Called when the client connects
function onConnect() {
    // Redirect to the sensors info page
    //window.open("sensors.html");

    // Fetch the MQTT topic from the form
    //phSensor = "1710510160UlulAzmi/esp32/ph";
    temperatureSensor = "1710510160UlulAzmi/esp32/temperature";

    //hasil = client.subscribe(topic);

    // Print output for the user in the messages div
    //document.getElementById("pHmessages").innerHTML += '<h6 class="font-extrabold mb-0">' + hasil + '</h6>';

    // Subscribe to the requested topic
    //client.subscribe(phSensor);
    client.subscribe(temperatureSensor);
}

// Called when the client loses its connection
function onConnectionLost(responseObject) {
    console.log("onConnectionLost: Connection Lost");
    if (responseObject.errorCode !== 0) {
        console.log("onConnectionLost: " + responseObject.errorMessage);
    }
}

// Called when a message arrives
function onMessageArrived(message) {
    console.log("onMessageArrived: " + message.payloadString);
    document.getElementById("temp").innerHTML = '<h6 class="font-extrabold mb-0"> ' +message.payloadString + '</h6>';
    switch(message.destinationName){
        case "1710510160UlulAzmi/esp32/temperature":
            document.getElementById("temp").innerHTML = message.payloadString;
            break;
        case "1710510160UlulAzmi/esp32/voltase":
            document.getElementById("humidity").innerHTML = message.payloadString;
            break;
        case "1710510160UlulAzmi/esp32/temperature":
            document.getElementById("ph").innerHTML = message.payloadString;
            break;
        case "30:AE:A4:F5:88:6E/altitude":
            document.getElementById("altitude").innerHTML = message.payloadString;
            break;
        case "30:AE:A4:F5:88:6E/sos":
            
            if(message.payloadString == "ON"){
                alert("SOS DETECTED!" + '\n\n' +  "Click OK to view user's location ")
                //to be fixed
                window.open("http://www.google.com/maps/place/42.34,14.22");
            }
            break;
    }
    //updateScroll(); // Scroll to bottom of window
}

// Called when the disconnection button is pressed
function startDisconnect() {
    client.disconnect();
    document.getElementById("messages").innerHTML += '<span>Disconnected</span><br/>';
    updateScroll(); // Scroll to bottom of window
}

// Updates #messages div to auto-scroll
function updateScroll() {
    var element = document.getElementById("messages");
    element.scrollTop = element.scrollHeight;
}
