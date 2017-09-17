const net = require('net');
const socket = net.connect(33400,"flypi.local", () => {
  console.log("Connected to Fly Pi server");
});

function getRotation(a, b, c) {
	var radians = Math.atan2(a,Math.sqrt(c*c + b*b));
  return -radians * (180 / Math.PI);
}
function getXRotation(x, y, z) {
	return getRotation(y, x, z);
}
function getYRotation(x, y, z) {
	return getRotation(x, y, z);
}
let first=false;
let firstData=[];
const fItmByte=1+16+4+3+4;
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
  if(len==12){//it is sensor data
    let sens=[data.readFloatLE(0),data.readFloatLE(4),data.readFloatLE(8)];
    //console.log("sensor(raw,FloatLE):",sens);
    console.log(getXRotation(...sens)+"\t"+getYRotation(...sens));
  }else{
  
    console.log(">",data);
  }
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
  
  socket.write(Buffer.from(bf))
});
reader.on('close', function () {
  //do something
});
