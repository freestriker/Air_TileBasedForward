glslangValidator.exe -t -V DefaultShader.vert -IUtils/ -o DefaultShaderVert.spv
glslangValidator.exe -t -V DefaultShader.frag -IUtils/ -o DefaultShaderFrag.spv

glslangValidator.exe -t -V DefaultBackgroundShader.vert -IUtils/ -o DefaultBackgroundShaderVert.spv
glslangValidator.exe -t -V DefaultBackgroundShader.frag -IUtils/ -o DefaultBackgroundShaderFrag.spv

glslangValidator.exe -t -V DefaultTransprantShader.vert -IUtils/ -o DefaultTransparentShaderVert.spv
glslangValidator.exe -t -V DefaultTransprantShader.frag -IUtils/ -o DefaultTransparentShaderFrag.spv

glslangValidator.exe -t -V MirrorShader.vert -IUtils/ -o MirrorShaderVert.spv
glslangValidator.exe -t -V MirrorShader.frag -IUtils/ -o MirrorShaderFrag.spv

glslangValidator.exe -t -V GlassShader.vert -IUtils/ -o GlassShaderVert.spv
glslangValidator.exe -t -V GlassShader.frag -IUtils/ -o GlassShaderFrag.spv

glslangValidator.exe -t -V TestShader.comp -IUtils/ -o TestShader.comp.spv

cmd
