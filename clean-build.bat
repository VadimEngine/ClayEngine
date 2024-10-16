@echo off
for /D %%p in (.\build\*) do rmdir "%%p" /s /q
del /q /s ".\build\*"