;del /Q projects\MDK-ARM\JLink*
;del /Q projects\MDK-ARM\project.uvguix.*
;del /Q projects\MDK-ARM\*.bak
;del /Q projects\MDK-ARM\*.dep
;rd /Q /S projects\MDK-ARM\Flash
;rd /Q /S projects\MDK-ARM\CpuRAM
;rd /Q /S projects\MDK-ARM\ExtRAM

del /Q ..\JLink*
del /Q ..\project.uvguix.*
del /Q ..\*.bak
del /Q ..\*.dep
del /Q ..\*.plg
rd /Q /S ..\Flash
rd /Q /S ..\CpuRAM
rd /Q /S ..\ExtRAM

del /Q projects\EWARMv5\Project.dep
del /Q projects\EWARMv5\Flash
del /Q projects\EWARMv5\CpuRAM
del /Q projects\EWARMv5\settings
rd  /Q /S projects\EWARMv5\Flash
rd /Q /S projects\EWARMv5\CpuRAM
rd /Q /S projects\EWARMv5\ExtSRAM
rd /Q /S projects\EWARMv5\settings

del /Q ..\Debug.BAT
del /Q ..\EventRecorderStub.scvd
rd /Q /S ..\DebugConfig
rd /Q /S ..\RTE