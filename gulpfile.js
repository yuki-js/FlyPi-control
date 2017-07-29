const gulp = require("gulp");
const cProc=require("child_process");

const fileList=[
  "main.c",
//  "communication.c",
//  "control.c"
];
const gccArg=[
  "-o",
  __dirname+"/flypi.out",
  ...fileList,
  "-lpthread",
  "-lpigpio", "-lrt"
];
let out=null;
gulp.task("build",()=>{
  out&&out.kill();
  cProc.execFile("gcc",gccArg,(er,so,se)=>{
    console.log(se);
    out=cProc.spawn(gccArg[1],{stdio:["ignore",process.stdout,process.stderr]})
    //out.stdout.on("data",(d)=>console.log(d.toString()));
    //out.stderr.on("data",(d)=>console.log(d.toString()));
    out.on("close",(code)=>{
      console.log("Exited with "+code);
    })
  });
});

gulp.task("default",["build"],()=>{
  console.log("fileList=",fileList);
  console.log("gccArg=",gccArg);
  fileList.forEach(v=>{
    gulp.watch("./"+v,["build"]);
  });
  
});

