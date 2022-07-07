DEL Spv\*.*

glslangValidator.exe -t -V OpaqueShader.vert -IUtils/ -o Spv/OpaqueShader.vert.spv
glslangValidator.exe -t -V OpaqueShader.frag -IUtils/ -o Spv/OpaqueShader.frag.spv

glslangValidator.exe -t -V BackgroundShader.vert -IUtils/ -o Spv/BackgroundShaderVert.spv
glslangValidator.exe -t -V BackgroundShader.frag -IUtils/ -o Spv/BackgroundShaderFrag.spv

glslangValidator.exe -t -V TransprantShader.vert -IUtils/ -o Spv/TransparentShaderVert.vert.spv
glslangValidator.exe -t -V TransprantShader.frag -IUtils/ -o Spv/TransparentShaderFrag.frag.spv

glslangValidator.exe -t -V MirrorShader.vert -IUtils/ -o Spv/MirrorShaderVert.vert.spv
glslangValidator.exe -t -V MirrorShader.frag -IUtils/ -o Spv/MirrorShaderFrag.frag.spv

glslangValidator.exe -t -V GlassShader.vert -IUtils/ -o Spv/GlassShaderVert.vert.spv
glslangValidator.exe -t -V GlassShader.frag -IUtils/ -o Spv/GlassShaderFrag.frag.spv

::glslangValidator.exe -t -V TestShader.comp -IUtils/ -o Spv/TestShader.comp.spv

cmd
