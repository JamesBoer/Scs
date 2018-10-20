@echo off

rem Add Courier as a backup font to Consolas, which may not be available on all systems
powershell -Command "(gc ../Docs/Tutorial.htm) -replace 'Consolas;', 'Consolas, Hack, Inconsolata, Monaco, monospace;' | Out-File ../Docs/Tutorial.htm"
