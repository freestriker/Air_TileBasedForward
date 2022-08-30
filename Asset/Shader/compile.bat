glslangValidator.exe -t -V F_Opaque_Wall_Shader.vert -IUtils/ -o Spv/F_Opaque_Wall_Shader.vert.spv
glslangValidator.exe -t -V F_Opaque_Wall_Shader.frag -IUtils/ -o Spv/F_Opaque_Wall_Shader.frag.spv

glslangValidator.exe -t -V F_Opaque_Mirror_Shader.vert -IUtils/ -o Spv/F_Opaque_Mirror_Shader.vert.spv
glslangValidator.exe -t -V F_Opaque_Mirror_Shader.frag -IUtils/ -o Spv/F_Opaque_Mirror_Shader.frag.spv

glslangValidator.exe -t -V F_Opaque_Glass_Shader.vert -IUtils/ -o Spv/F_Opaque_Glass_Shader.vert.spv
glslangValidator.exe -t -V F_Opaque_Glass_Shader.frag -IUtils/ -o Spv/F_Opaque_Glass_Shader.frag.spv

glslangValidator.exe -t -V Background_Skybox_Shader.vert -IUtils/ -o Spv/Background_Skybox_Shader.vert.spv
glslangValidator.exe -t -V Background_Skybox_Shader.frag -IUtils/ -o Spv/Background_Skybox_Shader.frag.spv

glslangValidator.exe -t -V F_Transparent_BrokenGlass_Shader.vert -IUtils/ -o Spv/F_Transparent_BrokenGlass_Shader.vert.spv
glslangValidator.exe -t -V F_Transparent_BrokenGlass_Shader.frag -IUtils/ -o Spv/F_Transparent_BrokenGlass_Shader.frag.spv

glslangValidator.exe -t -V TBF_TransparentShader.vert -IUtils/ -o Spv/TBF_TransparentShader.vert.spv
glslangValidator.exe -t -V TBF_TransparentShader.frag -IUtils/ -o Spv/TBF_TransparentShader.frag.spv

glslangValidator.exe -t -V GeometryShader.vert -IUtils/ -o Spv/GeometryShader.vert.spv
glslangValidator.exe -t -V GeometryShader.frag -IUtils/ -o Spv/GeometryShader.frag.spv

glslangValidator.exe -t -V TBF_OIT_DP_Shader.vert -IUtils/ -o Spv/TBF_OIT_DP_Shader.vert.spv
glslangValidator.exe -t -V TBF_OIT_DP_Shader.frag -IUtils/ -o Spv/TBF_OIT_DP_Shader.frag.spv

glslangValidator.exe -t -V TBF_OIT_ALL_Shader.vert -IUtils/ -o Spv/TBF_OIT_ALL_Shader.vert.spv
glslangValidator.exe -t -V TBF_OIT_ALL_Shader.frag -IUtils/ -o Spv/TBF_OIT_ALL_Shader.frag.spv

glslangValidator.exe -t -V OIT_DP_BlendShader.vert -IUtils/ -o Spv/OIT_DP_BlendShader.vert.spv
glslangValidator.exe -t -V OIT_DP_BlendShader.frag -IUtils/ -o Spv/OIT_DP_BlendShader.frag.spv

glslangValidator.exe -t -V OIT_ALL_SortBlendShader.vert -IUtils/ -o Spv/OIT_ALL_SortBlendShader.vert.spv
glslangValidator.exe -t -V OIT_ALL_SortBlendShader.frag -IUtils/ -o Spv/OIT_ALL_SortBlendShader.frag.spv

glslangValidator.exe -t -V SsaoOcclusionShader.vert -IUtils/ -o Spv/SsaoOcclusionShader.vert.spv
glslangValidator.exe -t -V SsaoOcclusionShader.frag -IUtils/ -o Spv/SsaoOcclusionShader.frag.spv

glslangValidator.exe -t -V SsaoBlurShader.vert -IUtils/ -o Spv/SsaoBlurShader.vert.spv
glslangValidator.exe -t -V SsaoBlurShader.frag -IUtils/ -o Spv/SsaoBlurShader.frag.spv

glslangValidator.exe -t -V SsaoShader.vert -IUtils/ -o Spv/SsaoShader.vert.spv
glslangValidator.exe -t -V SsaoShader.frag -IUtils/ -o Spv/SsaoShader.frag.spv

glslangValidator.exe -t -V PresentShader.vert -IUtils/ -o Spv/PresentShader.vert.spv
glslangValidator.exe -t -V PresentShader.frag -IUtils/ -o Spv/PresentShader.frag.spv

glslangValidator.exe -t -V BuildTBFLightListsShader.comp -IUtils/ -o Spv/BuildTBFLightListsShader.comp.spv

cmd
