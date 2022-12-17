msbuild ./DirectXTex/DirectXTex_Desktop_2022_Win10.sln /p:configuration=debug;Platform="x64";RuntimeLibraryr=MultiThreadedDebug
move .\DirectXTex\DirectXTex\Bin\Desktop_2022_Win10\x64\Debug\DirectXTex.lib .\Debug
pause