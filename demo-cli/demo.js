var thrift = require('thrift');
var demo = require('./Demo');
const assert = require('assert');

var transport = thrift.TFramedTransport;
var protocol  = thrift.TBinaryProtocol;

var connection = thrift.createConnection("localhost", 8888, {
    transport : transport,
    protocol : protocol
});

connection.on('error', function(err) {
    assert(false, err);
});

var multiplexer = new thrift.Multiplexer();
var demo_client = multiplexer.createClient('Demo', demo, connection);

let arg = {
  action: "echo",
  params: 1
};
demo_client.rpcCall(JSON.stringify(arg), function (err, ret) {
  // ret = JSON.parse(ret);
  console.log(ret);
  connection.end();
});

arg = {
  action: "sayhi",
  params: 1
};
demo_client.rpcCall(JSON.stringify(arg), function (err, ret) {
  // ret = JSON.parse(ret);
  console.log(ret);
  connection.end();
});
