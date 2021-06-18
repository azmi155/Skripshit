var selectedClientID;
var sensors;
var hasil;
var topic;
var StatusPompa = true;
var mqtt;

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
        userName: user,
        password: pass
    });

}

// Called when the client connects
function onConnect() {
    // Redirect to the sensors info page
    //window.open("sensors.html");

    // Fetch the MQTT topic from the form
    phSensor = "1710510160UlulAzmi/esp32/ph";
    temperatureSensor = "1710510160UlulAzmi/esp32/temperature";
    watherLevel = "1710510160UlulAzmi/esp32/whaterlevel";
    suhu = "1710510160UlulAzmi/esp32/suhu";

    //hasil = client.subscribe(topic);

    // Print output for the user in the messages div
    //document.getElementById("pHmessages").innerHTML += '<h6 class="font-extrabold mb-0">' + hasil + '</h6>';

    // Subscribe to the requested topic
    client.subscribe(phSensor);
    client.subscribe(temperatureSensor);
    client.subscribe(watherLevel);
    client.subscribe(suhu);


}

// function tblPompa(message){
//     topic = "1710510160UlulAzmi/esp32/controll/pompa";
//     if (StatusPompa = true){
//         client.publish(topic,"0");
//         pompatTEMP = false;
//     }else if(StatusPompa = false){
//         client.publish(topic,"1");
//         pompatTEMP = true;
//     }

// }




// Called when the client loses its connection
function onConnectionLost(responseObject) {
    console.log("onConnectionLost: Connection Lost");
    if (responseObject.errorCode !== 0) {
        console.log("onConnectionLost: " + responseObject.errorMessage);
    }
}

// Called when a message arrives
function onMessageArrived(message) {
    //console.log("onMessageArrived: " + message.payloadString);
    //document.getElementById("pHmessages").innerHTML = '<h6 class="font-extrabold mb-0"> ' +message.payloadString + '</h6>';

    // if(message.destinationName = "1710510160UlulAzmi/esp32/temperature"){
    //     document.getElementById("temp").innerHTML = '<h6 class="font-extrabold mb-0"> ' + message.payloadString + '</h6>';
    //         temp = message;
    // }else if(message.destinationName = "1710510160UlulAzmi/esp32/whaterlevel"){
    //     document.getElementById("Watherlevel").innerHTML = '<h6 class="font-extrabold mb-0"> ' + message.payloadString + '</h6>';
    //         level = message.payloadString;
    // } else if(message.destinationName = "1710510160UlulAzmi/esp32/ph"){
    //     document.getElementById("pHmessages").innerHTML = '<h6 class="font-extrabold mb-0"> ' + message.payloadString + '</h6>';
    //         ph = message.payloadString;
    // }


    switch (message.destinationName) {
        case "1710510160UlulAzmi/esp32/temperature":
            document.getElementById("temp").innerHTML = '<h6 class="font-extrabold mb-0"> ' + message.payloadString + '</h6>';
            //temp = message;
            break;
        case "1710510160UlulAzmi/esp32/whaterlevel":
            document.getElementById("Watherlevel").innerHTML = '<h6 class="font-extrabold mb-0"> ' + message.payloadString + '</h6>';
            // level = message.payloadString;
            break;
        case "1710510160UlulAzmi/esp32/ph":
            document.getElementById("pHmessages").innerHTML = '<h6 class="font-extrabold mb-0"> ' + message.payloadString + '</h6>';
            //  ph = message.payloadString;
            break;
        case "1710510160UlulAzmi/esp32/suhu":
            document.getElementById("SuhuSekitar").innerHTML = '<h6 class="font-extrabold mb-0"> ' + message.payloadString + ' C</h6>';
            //  ph = message.payloadString;
            break;

    }

}

// Called when the disconnection button is pressed
function startDisconnect() {
    client.disconnect();
    document.getElementById("messages").innerHTML += '<span>Disconnected</span><br/>';

}

function tblPompa() {
    
    var btn = document.getElementById("tblPompa").value;
    
    var topic = "1710510160UlulAzmi/esp32/controll/pompa";
    tblmessage = new Paho.MQTT.Message(btn);
    if (btn == "0") {
        tblmessage.destinationName = topic;
        document.getElementById("tblPompa").value = "1";
        
        console.log("Pompa OFF");
    }

    else {
        tblmessage.destinationName = topic;
        document.getElementById("tblPompa").value= "0";
       
        console.log("Pompa ON");
    }

    client.send(tblmessage);
    return false;
}