const net = require('net');
const socket = net.connect("192.168.0.9:33400", () => {
  console.log("Connected to Fly Pi server");
});
let first=false;
let firstData=[];
const fItmByte=1+16+4+3;
socket.on("data",data=>{
  let len=Buffer.byteLength(data);
  if(!first){
    let fLen=len/fItmByte;
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
  if(len==2*7){//it is sensor data
    let sens=[data.readInt16LE(0),data.readInt16LE(2),data.readInt16LE(4)];
    console.log("sensor(raw,Int16LE):",sens);
    let angleY=Math.atan2(sens[0],Math.abs(sens[1])+sens[2])*180/Math.PI;
    let angleX=Math.atan2(sens[1],Math.abs(sens[0])+sens[2])*180/Math.PI;
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
