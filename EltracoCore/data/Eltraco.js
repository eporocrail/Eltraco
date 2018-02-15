var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);
var decoderType="0";
connection.onopen = function () {
    connection.send('Connect at: ' + new Date());
};
connection.onerror = function (error) {
    console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {  
    console.log('Server: ', e.data);
};
connection.onclose = function(){
    console.log('WebSocket connection closed at: '+ new Date());
};

function linkShow(){
    document.getElementById("lnkServo").style.visibility = "visible";
}

function linkHideServo(){
    if (decoderType > 1){
        document.getElementById("lnkServo").style.visibility = "hidden";
    }else{
     document.getElementById("lnkServo").style.visibility = "visible";
    }
}

function debugNo(){
    connection.send("#02"+"1"+"0");
    coreOut();
}

function debugYes(){
    connection.send("#02"+"1"+"1");
    coreOut();
}

function wifiNo(){
    connection.send("#02"+"0"+"0");
    coreOut();
}

function wifiYes(){
    connection.send("#02"+"0"+"1");
    coreOut();
}

function typeDouble(){
    decoderType="0";
    connection.send("#01"+"0");
    coreOut();
    linkHideServo();
}

function typeSingle(){
    decoderType="1";
    connection.send("#01"+"1");
    coreOut();
    linkHideServo();
}

function typeSwitch(){
    decoderType="2";
    connection.send("#01"+"2");
    coreOut();
    linkHideServo();
}

function typeSensor(){
    decoderType="3";
    connection.send("#01"+"3");
    coreOut();
    linkHideServo();
}

function typeClock(){
    decoderType="11";
    connection.send("#01"+"11");
    accOut();
}

function typeGCA145(){
    decoderType="12";
    connection.send("#01"+"12");
    accOut();
}

function typeFyRfid(){
    decoderType="13";
    connection.send("#01"+"13");
    accOut();
}

function typeRcvr(){
    decoderType="14";
    connection.send("#01"+"14");
    accOut();
}

function accWfNo(){
    connection.send("#02"+"0"+"0");
    accOut();
}

function accWfYes(){
    connection.send("#02"+"0"+"1");
    accOut();
}

function accDebugNo(){
    connection.send("#02"+"1"+"0");
    accOut();
}

function accDebugYes(){
    connection.send("#02"+"1"+"1");
    accOut();
}

function coreOut(){
document.getElementById("decDouble").style.visibility = "hidden";
document.getElementById("decSingle").style.visibility = "hidden";
document.getElementById("decSwitch").style.visibility = "hidden";
document.getElementById("decSensor").style.visibility = "hidden";
document.getElementById("wfNo").style.visibility = "hidden";
document.getElementById("wfYes").style.visibility = "hidden";
document.getElementById("debNo").style.visibility = "hidden";
document.getElementById("debYes").style.visibility = "hidden";
document.getElementById("ipDec").style.visibility = "hidden";
document.getElementById("ipGat").style.visibility = "hidden";
document.getElementById("ipMos").style.visibility = "hidden";
}

function accOut(){
document.getElementById("decClock").style.visibility = "hidden";
document.getElementById("decGCA145").style.visibility = "hidden";
document.getElementById("decFyRfid").style.visibility = "hidden";
document.getElementById("decRcvr").style.visibility = "hidden";
document.getElementById("accWfNo").style.visibility = "hidden";
document.getElementById("accWfYes").style.visibility = "hidden";
document.getElementById("accDebNo").style.visibility = "hidden";
document.getElementById("accDebYes").style.visibility = "hidden";
}

function ipDecoder() {
    var x, text;
    // Get the value of the input field with id="ipAdrDecoder"
    x = document.getElementById("ipAdrDecoder").value;
    // If x is Not a Number or less than 2 or greater than 249
    if (isNaN(x) || x < 2 || x > 249) {
        text = "Input not valid";
    } else {
        text = "Input OK";
        connection.send("#00"+"0"+x);
        coreOut();
    }
    document.getElementById("resDecoder").innerHTML = text;
}

function ipGateway() {
    var x, text;
    x = document.getElementById("ipAdrGateway").value;
    if (isNaN(x) || x < 1 || x > 249) {
        text = "Input not valid";
    } else {
        text = "Input OK";
        connection.send("#00"+"1"+x);
        coreOut();
    }
    document.getElementById("resGateway").innerHTML = text;
}

function ipMosquitto() {
    var x, text;
    x = document.getElementById("ipAdrMosquitto").value;
    if (isNaN(x) || x < 1 || x > 249) {
        text = "Input not valid";
    } else {
        text = "Input OK";
        connection.send("#00"+"2"+x);
        coreOut();
    }
    document.getElementById("resMosquitto").innerHTML = text;
}

function sendStraight1(){
    var s1 = document.getElementById('straight1').value;
    if (s1 >0 && s1<10){
        connection.send("201");
    }
    connection.send(s1);
}

function sendTurnout1() {
    var t1 = document.getElementById('turnout1').value;
        if (t1 >0 && t1<10){
    connection.send("202");
    }
    connection.send(t1);
}

function AcknowledgeS1(){
        connection.send("205");
        document.getElementById('acknowledgeS1').style.backgroundColor = 'lightsalmon';
        document.getElementById('straight1').className = 'disabled';
        document.getElementById('straight1').disabled = true;
        
        document.getElementById('acknowledgeT1').style.backgroundColor = '#99ffcc';
        document.getElementById('turnout1').className = 'disabled';
        document.getElementById('turnout1').disabled = false;
}

function AcknowledgeT1(){
        connection.send("206");
        document.getElementById('acknowledgeT1').style.backgroundColor = 'lightsalmon';
        document.getElementById('turnout1').className = 'disabled';
        document.getElementById('turnout1').disabled = true;
        document.getElementById("servo1").style.visibility = "hidden";
        if (single==false){
            document.getElementById('acknowledgeS2').style.backgroundColor = '#99ffcc';
            document.getElementById('straight2').className = 'disabled';
            document.getElementById('straight2').disabled = false;
        }
}

function sendStraight2(){
    var s2 = document.getElementById('straight2').value;
    if (s2 >0 && s2<10){
        connection.send("203");
    }
    connection.send(s2);
 }

function sendTurnout2() {
    var t2 = document.getElementById('turnout2').value;
    if (t2 >0 && t2<10){
        connection.send("204");
    }
    connection.send(t2);
}

function AcknowledgeS2(){
        connection.send("207");
        document.getElementById('acknowledgeS2').style.backgroundColor = 'lightsalmon';
        document.getElementById('straight2').className = 'disabled';
        document.getElementById('straight2').disabled = true;
        
        document.getElementById('acknowledgeT2').style.backgroundColor = '#99ffcc';
        document.getElementById('turnout2').className = 'disabled';
        document.getElementById('turnout2').disabled = false;
}

function AcknowledgeT2(){
        connection.send("208");
        document.getElementById('acknowledgeT2').style.backgroundColor = 'lightsalmon';
        document.getElementById('turnout2').className = 'disabled';
        document.getElementById('turnout2').disabled = true;
        document.getElementById("servo2").style.visibility = "hidden";
}

function servo1(){
        connection.send("209");
        document.getElementById('servo1').style.backgroundColor = '#7FFF00';
        document.getElementById('servo2').style.backgroundColor = '#99ffcc';
        document.getElementById('acknowledgeT2').style.backgroundColor = 'lightsalmon';
        document.getElementById('turnout2').className = 'disabled';
        document.getElementById('turnout2').disabled = true;
        document.getElementById('acknowledgeS2').style.backgroundColor = 'lightsalmon';
        document.getElementById('straight2').className = 'disabled';
        document.getElementById('straight2').disabled = true;
        
        document.getElementById('acknowledgeT1').style.backgroundColor = '#99ffcc';
        document.getElementById('turnout1').className = 'disabled';
        document.getElementById('turnout1').disabled = false;
        document.getElementById('acknowledgeS1').style.backgroundColor = '#99ffcc';
        document.getElementById('straight1').className = 'disabled';
        document.getElementById('straight1').disabled = false;
        
        document.getElementById('acknowledgeT1').style.backgroundColor = 'lightsalmon';
        document.getElementById('turnout1').className = 'disabled';
        document.getElementById('turnout1').disabled = true;
}

function servo2(){
        connection.send("210");
        document.getElementById('servo2').style.backgroundColor = '#7FFF00';
        document.getElementById('servo1').style.backgroundColor = '#99ffcc';
        document.getElementById('acknowledgeT2').style.backgroundColor = 'lightsalmon';
        document.getElementById('turnout2').className = 'disabled';
        document.getElementById('turnout2').disabled = false;
        document.getElementById('acknowledgeS2').style.backgroundColor = '#99ffcc';
        document.getElementById('straight2').className = 'disabled';
        document.getElementById('straight2').disabled = false;
        
        document.getElementById('acknowledgeT1').style.backgroundColor = 'lightsalmon';
        document.getElementById('turnout1').className = 'disabled';
        document.getElementById('turnout1').disabled = true;
        document.getElementById('acknowledgeS1').style.backgroundColor = 'lightsalmon';
        document.getElementById('straight1').className = 'disabled';
        document.getElementById('straight1').disabled = true;
        
        document.getElementById('acknowledgeT1').style.backgroundColor = 'lightsalmon';
        document.getElementById('turnout1').className = 'disabled';
        document.getElementById('turnout1').disabled = true;
        
        document.getElementById('acknowledgeS2').style.backgroundColor = '#99ffcc';
        document.getElementById('acknowledgeT2').style.backgroundColor = 'lightsalmon';
        document.getElementById('straight2').className = 'disabled';
        document.getElementById('straight2').disabled = false;
        document.getElementById('turnout2').className = 'disabled';
        document.getElementById('turnout2').disabled = true;
}

function hideAll(){
    document.getElementById('turnout2').className = 'disabled';
    document.getElementById('turnout2').disabled = true;
    document.getElementById('straight2').className = 'disabled';
    document.getElementById('straight2').disabled = true;
    document.getElementById('turnout1').className = 'disabled';
    document.getElementById('turnout1').disabled = true;  
    document.getElementById('straight1').className = 'disabled';
    document.getElementById('straight1').disabled = true;
    document.getElementById('acknowledgeS1').style.backgroundColor = 'lightsalmon';
    document.getElementById('acknowledgeS2').style.backgroundColor = 'lightsalmon';
    document.getElementById('acknowledgeT1').style.backgroundColor = 'lightsalmon';
    document.getElementById('acknowledgeT2').style.backgroundColor = 'lightsalmon';
}