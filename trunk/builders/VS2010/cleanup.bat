@ECHO OFF
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S _ReSharper.*') DO echo "%%G" && rd /Q /S  "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *Debug') DO echo "%%G" && rd /Q /S  "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *Release') DO echo "%%G" && rd /Q /S "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.user') DO echo "%%G" && del /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.bak') DO echo "%%G" && del /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.ncb') DO echo "%%G" && del /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S *.zip') DO echo "%%G" && del /Q "%%G"
FOR /F "tokens=*" %%G IN ('DIR /B /S /AH *.suo') DO echo "%%G" && del /Q /AH "%%G"
