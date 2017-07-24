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

gulp.task("build",()=>{
  cProc.execFile("gcc",gccArg,(er,so,se)=>{
    console.log(se);
  });
});


gulp.task("default",["build"],()=>{
  console.log("fileList=",fileList);
  console.log("gccArg=",gccArg);
  fileList.forEach(v=>{
    gulp.watch("./"+v,["build"]);
  });
  
});

