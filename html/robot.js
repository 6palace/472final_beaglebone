
//----Accelerometer and Touch----//

window.addEventListener("devicemotion",onDeviceMotion,false);

var click = ('ontouchstart' in document.documentElement)  ? 'touchstart' : 'mousedown';
var upclick = ('ontouchend' in document.documentElement)  ? 'touchend' : 'mouseup';

var sendTilt = false;

$("#butRev").on(click,function(){
	console.log("rev");
	sendMessage('TANKREV');
}); 
$("#butFw").on(click,function(){
	sendTilt = true;
	sendMessage('TANKFW');
}); 
$("#butMode").on(click,function(){
	sendMessage('TANKMODE');
}); 	
$("#butRev").on(upclick,function(){
	sendTilt = false;
	sendMessage('TANKSTOP');
}); 
$("#butFw").on(upclick,function(){
	sendTilt = false;
	sendMessage('TANKSTOP');
}); 


function onDeviceMotion(event){
	if (sendTilt) {
		var accelGround;

		if (window.orientation != 0)
			accelGround = event.accelerationIncludingGravity.y;
		else
			accelGround = event.accelerationIncludingGravity.x;

		var turn = Math.floor(accelGround * 10);

		document.getElementById("fdData").innerHTML = turn;
		sendMessage('TANKTURN:' + turn);
	}
}

//----Robot Websockets----//

setInterval(function(){
	sendMessage('ROBOTEMP');
}, 3000);



var ws = new WebSocket('ws://' + window.location.hostname + ':9000', 'robot-cmd-protocol');
ws.onmessage = function(evt) { 
	document.getElementById("temperatureData").innerHTML = evt.data;
};

ws.onopen = function(e) {
	document.getElementById("connectData").innerHTML = "Yes";
};

ws.onerror = function(e) {
	document.getElementById("connectData").innerHTML = "No";
};

ws.onclose = function(e) {
	document.getElementById("connectData").innerHTML = "No";
};

function sendMessage(msg){
	waitForSocketConnection(ws, function(){
		ws.send(msg);
	});
}

function waitForSocketConnection(socket, callback){
	setTimeout(
		function () {
			if (socket.readyState === 1) {
				if(callback != null){
					callback();
				}
				return;
			} else {
				waitForSocketConnection(socket, callback);
			}

		}, 5);
}

//myWebSocket.close();