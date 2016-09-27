var inspect = require('util').inspect;
var Client = require('mariasql');
 
var c = new Client();
c.connect({
  host: '127.0.0.1',
  user: 'root',
  password: 'qlqlxks12'
  db: 'wwwdb'
});
 
c.on('connect', function() {
   console.log('Client connected');
 })
 .on('error', function(err) {
   console.log('Client error: ' + err);
 })
 .on('close', function(hadError) {
   console.log('Client closed');
 });
 
var result = c.query('SHOW DATABASES');

result.on('result', function(res){ 
	res.on('row', function(row) {
    console.log('Result row: ' + inspect(row));
 });
});


result.on('end', function() {
	console.log('Done with all results');
});

c.end();