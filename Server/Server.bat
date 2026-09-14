echo WScript.sleep 1000 >sss.vbs

start SdDbServer.bat
sss.vbs
start SdCenterServer.bat
sss.vbs
start SdGateServer.bat
sss.vbs
start SdLoginServer.bat