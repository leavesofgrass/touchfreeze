.SUFFIXES: .wxs .wixobj

!if "$(CFG)" == "Debug"
OutDir=..\..\Executable\bin_dbg
!elseif "$(CFG)" == "Release"
OutDir=..\..\Executable\bin
!endif

IntDir=..\..\Intermediate\$(CFG)\Setup

MSI=$(OutDir)\TouchFreeze.msi

all: makedirs Setup.mak $(MSI)

clean:
    @if exist $(IntDir) rmdir /Q /S $(IntDir)
    @if exist $(MSI)    del $(MSI)
    
makedirs: 
    @if not exist $(IntDir) mkdir $(IntDir)
    @if not exist $(OutDir) mkdir $(OutDir)

.wxs{$(IntDir)\}.wixobj: 
    @candle -nologo -out $(IntDir)\ %s -dExecDir=$(OutDir) -dVERSION=1.0.3

$(MSI): $(IntDir)\Setup.wixobj $(OutDir)\TouchFreeze.exe $(OutDir)\TouchFreeze.dll
    @light -nologo -out $(MSI) $(IntDir)\Setup.wixobj
