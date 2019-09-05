@echo off

if exist C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe (
if exist .\..\..\Flash\Obj\output.axf (C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe ".\..\..\Flash\Obj\output.axf" --bin --output ".\..\..\Flash\output.bin")
 )

pause

