REM Compile the proto files in the respective project directories

REM Client:
Pushd ..\Client\client_app\client_app
..\..\..\protoc-3.10.1-win32\bin\protoc.exe -I=..\..\..\Proto\ --cpp_out=. ..\..\..\Proto\messages.proto
echo #include "pch.h" > .\temp_to_del.txt
type .\messages.pb.cc >> .\temp_to_del.txt
type .\temp_to_del.txt > .\messages.pb.cc
del .\temp_to_del.txt
Popd

REM Server: 
Pushd ..\Server\server_app\server_app
..\..\..\protoc-3.10.1-win32\bin\protoc.exe -I=..\..\..\Proto\ --cpp_out=. ..\..\..\Proto\messages.proto
echo #include "pch.h" > .\temp_to_del.txt
type .\messages.pb.cc >> .\temp_to_del.txt
type .\temp_to_del.txt > .\messages.pb.cc
del .\temp_to_del.txt
Popd