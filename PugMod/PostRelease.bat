@echo off

tasklist /fi "imagename eq hlds.exe" |find ":" > nul
	if errorlevel 1 (taskkill /f /im "hlds.exe")

ping 127.0.0.1 -n 2 > nul

set "outputFile=..\Release\pugmod_mm.dll"

set "copyPath1=C:\Files\Desktop\HLDS\hlds\cstrike\addons\pugmod\dlls"
set "copyPath2=D:\Files\Desktop\HLDS\hlds\cstrike\addons\pugmod\dlls"

if exist %copyPath1% (copy %outputFile% %copyPath1%)
if exist %copyPath2% (copy %outputFile% %copyPath2%)

set "hldsParam=-console -game cstrike +port 27020 +map de_dust2 +maxplayers 32 +sys_ticrate 1000 -pingboost 3 -master -bots"

set "hldsPath1=C:\Files\Desktop\HLDS\hlds"
set "hldsPath2=D:\Files\Desktop\HLDS\hlds"

if exist %hldsPath1% (start /min /d "%hldsPath1%" hlds.exe %hldsParam%)
if exist %hldsPath2% (start /min /d "%hldsPath2%" hlds.exe %hldsParam%)