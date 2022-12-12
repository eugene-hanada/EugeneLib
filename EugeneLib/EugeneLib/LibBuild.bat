msbuild ./DirectXTex/DirectXTex/DirectXTex_Desktop_2022_Win10.vcxproj /p:configuration=debug /p:RuntimeLibrary=MTd /p:Platform="x64"
move .\DirectXTex\DirectXTex\Bin\Desktop_2022_Win10\x64\Debug\DirectXTex.lib .\Debug
pause