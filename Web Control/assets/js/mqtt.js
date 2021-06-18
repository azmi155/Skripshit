function onConnectionLost() {
    console.log("connection lost");
    document.getElementById("status").innerHTML = "Connection Lost";
    document.getElementById("messages").innerHTML = "Connection Lost";
    connected_flag = 0;
}
function onFailure(message) {
    console.log("Failed");
    document.getElementById("messages").innerHTML = "Connection Failed- Retrying";
    setTimeout(MQTTconnect, reconnectTimeout);
}
function onMessageArrived(r_message) {
    switch (r_message.destinationName) {
        case "1710510160UlulAzmi/esp32/temperature":
            document.getElementById("temp").innerHTML = '<h6 class="font-extrabold mb-0"> ' + r_message.payloadString + '</h6>';
            console.log(r_message.payloadString);
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
function onConnected(recon, url) {
    console.log(" in onConnected " + reconn);
}
function onConnect() {
    
    // Once a connection has been made, make a subscription and send a message.
    document.getElementById("messages").innerHTML = "Connected to " + host + "on port " + port;

    connected_flag = 1
    document.getElementById("status").innerHTML = "Connected";
    console.log("on Connect " + connected_flag);

    phSensor = "1710510160UlulAzmi/esp32/ph";
    temperatureSensor = "1710510160UlulAzmi/esp32/temperature";
    watherLevel = "1710510160UlulAzmi/esp32/whaterlevel";
    suhu = "1710510160UlulAzmi/esp32/suhu";

    mqtt.subscribe(phSensor);
    mqtt.subscribe(temperatureSensor);
    mqtt.subscribe(watherLevel);
    mqtt.subscribe(suhu);

}

function MQTTconnect() {
    clientID = "clientID-" + "Control Panel";
    //document.getElementById("messages").innerHTML ="";
    var s = "test.mosquitto.org";
    var p = "8081";
    user = "";
    pass = "";
    if (p != "") {
        console.log("ports");
        port = parseInt(p);
        console.log("port" + port);
    }
    if (s != "") {
        host = s;
        console.log("host");
    }
    console.log("connecting to " + host + " " + port);
    //var x=Math.floor(Math.random() * 10000); 
    //var cname="orderform-"+x;
    mqtt = new Paho.MQTT.Client(host, port, clientID);
    //document.write("connecting to "+ host);


    mqtt.onConnectionLost = onConnectionLost;
    mqtt.onMessageArrived = onMessageArrived;
    //mqtt.onConnected = onConnected;

    mqtt.connect({
        onSuccess: onConnect,
        userName: user,
        password: pass
    });


}


function send_message() {
    document.getElementById("messages").innerHTML = "";
    if (connected_flag == 0) {
        out_msg = "<b>Not Connected so can't send</b>"
        console.log(out_msg);
        document.getElementById("messages").innerHTML = out_msg;
        return false;
    }
    var msg = document.forms["smessage"]["message"].value;
    console.log(msg);

    var topic = document.forms["smessage"]["Ptopic"].value;
    message = new Paho.MQTT.Message(msg);
    if (topic == "")
        message.destinationName = "test-topic"
    else
        message.destinationName = topic;
    mqtt.send(message);
    return false;
}
