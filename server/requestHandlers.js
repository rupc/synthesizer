var querystring = require("querystring"),
fs = require("fs"),
formidable = require("formidable");

function start(response) {
	console.log("Request handler 'start' was called.");

	var body = '<html>'+
	'<head>'+
	'<meta http-equiv="Content-Type" content="text/html; '+
	'charset=UTF-8" />'+
	'</head>'+
	'<body>'+
	'<form action="/upload" enctype="multipart/form-data" '+
	'method="post">'+
	'<input type="file" name="upload" multiple="multiple">'+
	'<input type="submit" value="Upload file" />'+
	'</form>'+
	'</body>'+
	'</html>';

	response.writeHead(200, {"Content-Type": "text/html"});
	response.write(body);
	response.end();
}

function upload(response, request) {
	console.log("Request handler 'upload' was called.");

	var form = new formidable.IncomingForm();
	console.log("about to parse");
	form.parse(request, function(error, fields, files) {
		console.log("parsing done");
		fs.renameSync(files.upload.path, "/tmp/test.png");
		response.writeHead(200, {"Content-Type": "text/html"});
		response.write("received image:<br/>");
		response.write("<img src='/show' />");
		response.end();
	});
}

function show(response) {
	console.log("Request handler 'show' was called.");
	fs.readFile("tmp/penrosetile1.jpg", "binary", function(error, file) {
		if(error) {
			response.writeHead(500, {"Content-Type": "text/plain"});
			response.write(error + "\n");
			response.end();
		} else {
			response.writeHead(200, {"Content-Type": "image/png"});
			response.write(file, "binary");
			response.end();
		}
	});
}

function dondelete(response) {
	console.log("someone request");
	fs.readFile("tmp/dondelete.txt", "binary", function(error, file) {
		if(error) {
			response.writeHead(500, {"Content-Type": "text/plain"});
			response.write(error + "\n");
			response.end();
		} else {
			response.writeHead(200, {"Content-Type": "text/plain"});
			response.write(file, "utf8");
			response.end();
		}
	});
}

function instrument_contorl(response) {
	console.log("socket io has started.");
	fs.readFile("tmp/socketio.html", function(error, file) {
		if(error) {
			response.writeHead(404);
			response.write("socket io file this dones't exist - 404");
			response.end();
		} else {
			response.writeHead(200, {"Content-Type" : "text/html"});
			response.write(file, "utf8");
			response.end();
		}
	});

}
function sixteensegment(response) {
	fs.readFile("tmp/sixteensegment/segment.min.js", function(error, file) {
		if(error) {
			response.writeHead(404);
			response.write("segment.min.js doesn't exist");
			response.end();
		} else {
			response.writeHead(200, {"Content-Type" : "text/javascript"});
		    response.write(file, "utf8");
			console.log(file);
		    response.end();
		}
	});
}
function play(response) {
	fs.readFile("tmp/play.png", function(error, file) {
		if(error) {
			response.writeHead(404);
			response.write("play.png doesn't exist");
			response.end();			
		} else {
			response.writeHead(200, {'Content-Type' : 'image/jpeg'});
			response.end(file);
		}
	});
}
function stop(response) {
	fs.readFile("tmp/stop.jpg", function(error, file) {
		if(error) {
			response.writeHead(404);
			response.write("stop.png doesn't exist");
			response.end();			
		} else {
			response.writeHead(200, {'Content-Type' : 'image/jpeg'});
			response.end(file);
		}
	});

}
function note_off(response) {
	fs.readFile("tmp/note_off.png", function(error, file) {
		if(error) {
			response.writeHead(404);
			response.write("note_off.png doesn't exist");
			response.end();			
		} else {
			response.writeHead(200, {'Content-Type' : 'image/jpeg'});
			response.end(file);
		}
	});

}
function tstop(response) {
	fs.readFile("tmp/tstop.png", function(error, file) {
		if(error) {
			response.writeHead(404);
			response.write("tstop.png doesn't exist");
			response.end();			
		} else {
			response.writeHead(200, {'Content-Type' : 'image/jpeg'});
			response.end(file);
		}
	});

}

exports.start = start;
exports.upload = upload;
exports.show = show;
exports.dondelete = dondelete;
exports.inscontrol = instrument_contorl;
exports.sixteensegment = sixteensegment;
exports.play = play;
exports.stop = stop;
exports.note_off = note_off;
exports.tstop = tstop;