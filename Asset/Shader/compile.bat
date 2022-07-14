glslangValidator.exe -t -V OpaqueShader.vert -IUtils/ -o Spv/OpaqueShader.vert.spv
glslangValidator.exe -t -V OpaqueShader.frag -IUtils/ -o Spv/OpaqueShader.frag.spv

glslangValidator.exe -t -V BackgroundShader.vert -IUtils/ -o Spv/BackgroundShader.vert.spv
glslangValidator.exe -t -V BackgroundShader.frag -IUtils/ -o Spv/BackgroundShader.frag.spv

glslangValidator.exe -t -V F_TransparentShader.vert -IUtils/ -o Spv/F_TransparentShader.vert.spv
glslangValidator.exe -t -V F_TransparentShader.frag -IUtils/ -o Spv/F_TransparentShader.frag.spv

glslangValidator.exe -t -V TBF_TransparentShader.vert -IUtils/ -o Spv/TBF_TransparentShader.vert.spv
glslangValidator.exe -t -V TBF_TransparentShader.frag -IUtils/ -o Spv/TBF_TransparentShader.frag.spv

glslangValidator.exe -t -V MirrorShader.vert -IUtils/ -o Spv/MirrorShader.vert.spv
glslangValidator.exe -t -V MirrorShader.frag -IUtils/ -o Spv/MirrorShader.frag.spv

glslangValidator.exe -t -V GlassShader.vert -IUtils/ -o Spv/GlassShader.vert.spv
glslangValidator.exe -t -V GlassShader.frag -IUtils/ -o Spv/GlassShader.frag.spv

glslangValidator.exe -t -V PreZShader.vert -IUtils/ -o Spv/PreZShader.vert.spv
glslangValidator.exe -t -V PreZShader.frag -IUtils/ -o Spv/PreZShader.frag.spv

glslangValidator.exe -t -V TBFBuildLightListsShader.comp -IUtils/ -o Spv/TBFBuildLightListsShader.comp.spv

cmd
