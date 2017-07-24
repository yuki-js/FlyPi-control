const net = require('net');
const socket = net.connect({path:"/tmp/flypi-socket"}, () => {
  console.log("Connected to Fly Pi server");
});
let first=false;
let firstData=[];
const fItmByte=1+16+4+3;
socket.on("data",data=>{
  if(!first){
    let fLen=Buffer.byteLength(data)/fItmByte;
    for(let i=0;i<fLen;i++){
      firstData.push({
        pin:data.readUInt8(fItmByte*i,true),
        name:data.slice(fItmByte*i+1,fItmByte*i+17).toString().split("\0")[0],
        type:data.readUInt8(fItmByte*i+20,true)
      });
    }
    console.log(firstData);
    first=true;
  }
  console.log(">",data);
});

const reader = require('readline').createInterface({
  input: process.stdin,
  output: process.stdout
});
reader.on('line', function (line) {
  let bf=[];
  let ln = line.split(" ");
  
  for(let i=0;i<ln.length;i++){
    bf.push(parseInt(ln[i]));
  }
  
  socket.write(new Buffer(bf))
});
reader.on('close', function () {
  //do something
});
