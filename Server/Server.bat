echo WScript.sleep 500 >sss.vbs

start SdDbServer.bat
sss.vbs
start SdGateServer.bat
sss.vbs
start SdLoginServer.bat