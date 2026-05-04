REM 頂点シェーダー
set VS_HLSL=.\VertexShader.hlsl
set VS_ENTRY=main
set VS_PROFILE=vs_6_0
set VS_OUT=.\VertexShader.vso

REM ピクセルシェーダー
set PS_HLSL=.\PixelShader.hlsl
set PS_ENTRY=main
set PS_PROFILE=ps_6_0
set PS_OUT=.\PixelShader.pso

REM 頂点シェーダーのコンパイル
dxc -T %VS_PROFILE% -E %VS_ENTRY% -Fo %VS_OUT% -Fc VertexShader.asm %VS_HLSL%

REM ピクセルシェーダーのコンパイル
dxc -T %PS_PROFILE% -E %PS_ENTRY% -Fo %PS_OUT% -Fc PixelShader.asm %PS_HLSL%


pause
