var http = require("http");
var url = require("url");
var io = require('socket.io');
var net = require('net');
var $ = require('jquery');

var client_server;
var client_web_socket;

var arduino_server;
var arduino_tcp_socket;

var cli_port = 7000;
var ardu_port = 8000;
var asocket;
var ardu_conn = false;
function start(route, handle) {
  function onRequest(request, response) {
    var pathname = url.parse(request.url).pathname;
    console.log("Request for " + pathname + " received.");
	 route(handle, pathname, response, request);

  }

  client_server = http.createServer(onRequest).listen(cli_port);
  arduino_server = net.createServer().listen(ardu_port);
 

  client_web_socket = io.listen(client_server);
  // arduino_tcp_socket = io.listen(arduino_server);

	client_web_socket.sockets.on('connection', function(socket){
    //send data to client
    setInterval(function(){
        socket.emit('date', {'date': new Date()});
    }, 1000);

    socket.write("connected to the tcp server\r\n");

    socket.on('client_data', function(data){
      process.stdout.write(data.letter);
    });
    
    socket.on("music_value_change", function(data) {
      var jstring = JSON.stringify({name : data.name, value : data.value});
      if(ardu_conn == true) {
        sendMsgToUno(jstring);
      } else {
        console.log("it doesn't have receiver.");
      }  
      process.stdout.write(data.name + " " + data.value + " " +
                      jstring + "\n\r");
    });
	});

  arduino_server.on('connection', function(socket) {
    asocket = socket;
    ardu_conn = true;
    socket.write("this is new arduino socket communication");
    console.log("connection is made on port " + ardu_port + "#" + arduino_server.connections);

    // recevie data from arduino and print it
    socket.on('data', function(data) {
      console.log('received on tcp socket : ' + data);
    });

    socket.on('error', function(e) {
      console.log('error event ocurred\n\rarduino might reconnect to this host.');
      socket.write("Reconnecting...");
    });
  });

  //recieve client data

  console.log("Server has started.");
}
function sendMsgToUno(str) {
  asocket.write(str);
}
exports.start = start;