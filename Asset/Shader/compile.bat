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

glslangValidator.exe -t -V TBF_OIT_DP_Shader.vert -IUtils/ -o Spv/TBF_OIT_DP_Shader.vert.spv
glslangValidator.exe -t -V TBF_OIT_DP_Shader.frag -IUtils/ -o Spv/TBF_OIT_DP_Shader.frag.spv

glslangValidator.exe -t -V TBF_OIT_ALL_Shader.vert -IUtils/ -o Spv/TBF_OIT_ALL_Shader.vert.spv
glslangValidator.exe -t -V TBF_OIT_ALL_Shader.frag -IUtils/ -o Spv/TBF_OIT_ALL_Shader.frag.spv

glslangValidator.exe -t -V OIT_DP_BlendShader.vert -IUtils/ -o Spv/OIT_DP_BlendShader.vert.spv
glslangValidator.exe -t -V OIT_DP_BlendShader.frag -IUtils/ -o Spv/OIT_DP_BlendShader.frag.spv

glslangValidator.exe -t -V OIT_ALL_SortBlendShader.vert -IUtils/ -o Spv/OIT_ALL_SortBlendShader.vert.spv
glslangValidator.exe -t -V OIT_ALL_SortBlendShader.frag -IUtils/ -o Spv/OIT_ALL_SortBlendShader.frag.spv

glslangValidator.exe -t -V PresentShader.vert -IUtils/ -o Spv/PresentShader.vert.spv
glslangValidator.exe -t -V PresentShader.frag -IUtils/ -o Spv/PresentShader.frag.spv

glslangValidator.exe -t -V BuildTBFLightListsShader.comp -IUtils/ -o Spv/BuildTBFLightListsShader.comp.spv

cmd
