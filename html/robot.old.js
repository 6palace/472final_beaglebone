
//----Accelerometer and Touch----//

window.addEventListener("devicemotion",onDeviceMotion,false);

var click = ('ontouchstart' in document.documentElement)  ? 'touchstart' : 'mousedown';
var upclick = ('ontouchend' in document.documentElement)  ? 'touchend' : 'mouseup';

$("#butRev").on(click,function(){
	console.log("rev");
	sendMessage('TANKREV');
}); 
$("#butFw").on(click,function(){
	sendMessage('TANKFW');
}); 	
$("#butRev").on(upclick,function(){
	sendMessage('TANKSTOP');
}); 
$("#butFw").on(upclick,function(){
	sendMessage('TANKSTOP');
}); 

function onDeviceMotion(event){

	var accelGround;

	if (window.orientation != 0)
		accelGround = event.accelerationIncludingGravity.y;
	else
		accelGround = event.accelerationIncludingGravity.x;

	var turn = Math.floor(accelGround * 10);

	document.getElementById("fdData").innerHTML = turn;
}

//----Robot Websockets----//

var ws = new WebSocket('ws://' + window.location.hostname + ':9000', 'robot-cmd-protocol');
ws.onmessage = function(evt) { 
	//alert( "Received Message: " + evt.data); 
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