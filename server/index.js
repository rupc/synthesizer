var server = require("./server");
var router = require('./router');
var requestHandlers = require("./requestHandlers");

var handle = {};
// client port number : 7000
// localhost:7000/inscontrol
handle["/"] = requestHandlers.start;
handle["start"] = requestHandlers.start;
handle["/upload"] = requestHandlers.upload;
handle["/show"] = requestHandlers.show;
handle["/dontdelete"] = requestHandlers.dondelete;
handle["/inscontrol"] = requestHandlers.inscontrol;
handle["/play.png"] = requestHandlers.play;
handle["/stop.png"] = requestHandlers.stop;
handle["/note_off.png"] = requestHandlers.note_off;
handle["/tstop.png"] = requestHandlers.tstop;
handle["/segment.min.js"] = requestHandlers.sixteensegment;
server.start(router.route, handle);
	