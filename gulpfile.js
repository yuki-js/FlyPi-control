const gulp = require("gulp");
const cProc=require("child_process");
const os =require("os");

const fileList=[
  "main.c",
  //  "communication.c",
  //  "control.c"
];
const syncList=[
  "main.c",
  "gulpfile.js",
  "packetBits.h"
]
const gccArg=[
  "-o",
  __dirname+"/flypi.out",
  ...fileList,
  "-lpthread",
  "-lpigpio", "-lrt","-lm"
];
let out=null

if(os.platform()=="linux"){
  function re(){
    out=cProc.spawn(gccArg[1],{stdio:["ignore",process.stdout,process.stderr]})
    out.once("exit",(code)=>{
      console.log("Exited with "+code);
      setTimeout(re,5000)
    })
  }

  
  gulp.task("build",()=>{
    out&&out.kill();
    if(!out){
      cProc.execFile("gcc",gccArg,(er,so,se)=>{
        console.log(se);
        re()
      });
    }else{
      out.once("exit",()=>{
        cProc.execFile("gcc",gccArg,(er,so,se)=>{
          console.log(se);
          re()
        });
      });
    }
  });
  
  //cProc.spawn("rsync",["--daemon","--config=/etc/rsyncd.conf"])
  
}else{
  gulp.task("build",()=>{
    cProc.exec("rsync -truv ./ rsync://flypi.local/flypi",()=>{
      
    });
  });
}

gulp.task("default",["build"],()=>{
  console.log("fileList=",fileList);
  console.log("gccArg=",gccArg);
  console.log("dir:".__dirname);
  gulp.watch(syncList,["build"]);
  
});

