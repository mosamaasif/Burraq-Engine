
@ECHO "Compling Shaders!"
for %%i in (*.vert *.frag *.tesc *.tese *.geom *.comp) do "glslangValidator.exe" -V "%%~i" -o "%%~i.spv"