glslangValidator.exe -t -V TBF_Opaque_Pbr_Shader.vert -IUtils/ -o Spv/TBF_Opaque_Pbr_Shader.vert.spv
glslangValidator.exe -t -V TBF_Opaque_Pbr_Shader.frag -IUtils/ -o Spv/TBF_Opaque_Pbr_Shader.frag.spv

glslangValidator.exe -t -V IWave_Shader.comp -IUtils/ -o Spv/IWave_Shader.comp.spv

glslangValidator.exe -t -V FftOcean_PhillipsSpectrum_Shader.comp -IUtils/ -o Spv/FftOcean_PhillipsSpectrum_Shader.comp.spv
glslangValidator.exe -t -V FftOcean_GenerateFrequency_Shader.comp -IUtils/ -o Spv/FftOcean_GenerateFrequency_Shader.comp.spv
glslangValidator.exe -t -V FftOcean_Ifft_Shader.comp -IUtils/ -o Spv/FftOcean_Ifft_Shader.comp.spv

glslangValidator.exe -t -V IWave_Surface_Shader.vert -IUtils/ -o Spv/IWave_Surface_Shader.vert.spv
glslangValidator.exe -t -V IWave_Surface_Shader.frag -IUtils/ -o Spv/IWave_Surface_Shader.frag.spv

glslangValidator.exe -t -V GenerateIrradianceMap_Shader.vert -IUtils/ -o Spv/GenerateIrradianceMap_Shader.vert.spv
glslangValidator.exe -t -V GenerateIrradianceMap_Shader.frag -IUtils/ -o Spv/GenerateIrradianceMap_Shader.frag.spv

glslangValidator.exe -t -V GenerateLutMap_Accumulation_Shader.vert -IUtils/ -o Spv/GenerateLutMap_Accumulation_Shader.vert.spv
glslangValidator.exe -t -V GenerateLutMap_Accumulation_Shader.frag -IUtils/ -o Spv/GenerateLutMap_Accumulation_Shader.frag.spv

glslangValidator.exe -t -V GenerateLutMap_Pack_Shader.vert -IUtils/ -o Spv/GenerateLutMap_Pack_Shader.vert.spv
glslangValidator.exe -t -V GenerateLutMap_Pack_Shader.frag -IUtils/ -o Spv/GenerateLutMap_Pack_Shader.frag.spv

glslangValidator.exe -t -V GenerateLutMap_Unpack_Shader.vert -IUtils/ -o Spv/GenerateLutMap_Unpack_Shader.vert.spv
glslangValidator.exe -t -V GenerateLutMap_Unpack_Shader.frag -IUtils/ -o Spv/GenerateLutMap_Unpack_Shader.frag.spv

glslangValidator.exe -t -V GeneratePrefilteredMap_Shader.vert -IUtils/ -o Spv/GeneratePrefilteredMap_Shader.vert.spv
glslangValidator.exe -t -V GeneratePrefilteredMap_Shader.frag -IUtils/ -o Spv/GeneratePrefilteredMap_Shader.frag.spv

glslangValidator.exe -t -V SplitHdrToBackgroundCubeImage_Shader.vert -IUtils/ -o Spv/SplitHdrToBackgroundCubeImage_Shader.vert.spv
glslangValidator.exe -t -V SplitHdrToBackgroundCubeImage_Shader.frag -IUtils/ -o Spv/SplitHdrToBackgroundCubeImage_Shader.frag.spv

glslangValidator.exe -t -V TBF_Transparent_Shader.vert -IUtils/ -o Spv/TBF_Transparent_Shader.vert.spv
glslangValidator.exe -t -V TBF_Transparent_Shader.frag -IUtils/ -o Spv/TBF_Transparent_Shader.frag.spv

glslangValidator.exe -t -V SplitHdrToHdrCubeImage_Shader.vert -IUtils/ -o Spv/SplitHdrToHdrCubeImage_Shader.vert.spv
glslangValidator.exe -t -V SplitHdrToHdrCubeImage_Shader.frag -IUtils/ -o Spv/SplitHdrToHdrCubeImage_Shader.frag.spv

glslangValidator.exe -t -V TBF_Opaque_Pbr_Mirror_Shader.vert -IUtils/ -o Spv/TBF_Opaque_Pbr_Mirror_Shader.vert.spv
glslangValidator.exe -t -V TBF_Opaque_Pbr_Mirror_Shader.frag -IUtils/ -o Spv/TBF_Opaque_Pbr_Mirror_Shader.frag.spv

glslangValidator.exe -t -V F_Opaque_Wall_Shader.vert -IUtils/ -o Spv/F_Opaque_Wall_Shader.vert.spv
glslangValidator.exe -t -V F_Opaque_Wall_Shader.frag -IUtils/ -o Spv/F_Opaque_Wall_Shader.frag.spv

glslangValidator.exe -t -V TBF_Opaque_Wall_Shader.vert -IUtils/ -o Spv/TBF_Opaque_Wall_Shader.vert.spv
glslangValidator.exe -t -V TBF_Opaque_Wall_Shader.frag -IUtils/ -o Spv/TBF_Opaque_Wall_Shader.frag.spv

glslangValidator.exe -t -V F_Opaque_Mirror_Shader.vert -IUtils/ -o Spv/F_Opaque_Mirror_Shader.vert.spv
glslangValidator.exe -t -V F_Opaque_Mirror_Shader.frag -IUtils/ -o Spv/F_Opaque_Mirror_Shader.frag.spv

glslangValidator.exe -t -V TBF_Opaque_Mirror_Shader.vert -IUtils/ -o Spv/TBF_Opaque_Mirror_Shader.vert.spv
glslangValidator.exe -t -V TBF_Opaque_Mirror_Shader.frag -IUtils/ -o Spv/TBF_Opaque_Mirror_Shader.frag.spv

glslangValidator.exe -t -V F_Opaque_Glass_Shader.vert -IUtils/ -o Spv/F_Opaque_Glass_Shader.vert.spv
glslangValidator.exe -t -V F_Opaque_Glass_Shader.frag -IUtils/ -o Spv/F_Opaque_Glass_Shader.frag.spv

glslangValidator.exe -t -V TBF_Opaque_Glass_Shader.vert -IUtils/ -o Spv/TBF_Opaque_Glass_Shader.vert.spv
glslangValidator.exe -t -V TBF_Opaque_Glass_Shader.frag -IUtils/ -o Spv/TBF_Opaque_Glass_Shader.frag.spv

glslangValidator.exe -t -V TBF_Opaque_White_Shader.vert -IUtils/ -o Spv/TBF_Opaque_White_Shader.vert.spv
glslangValidator.exe -t -V TBF_Opaque_White_Shader.frag -IUtils/ -o Spv/TBF_Opaque_White_Shader.frag.spv

glslangValidator.exe -t -V Background_Skybox_Shader.vert -IUtils/ -o Spv/Background_Skybox_Shader.vert.spv
glslangValidator.exe -t -V Background_Skybox_Shader.frag -IUtils/ -o Spv/Background_Skybox_Shader.frag.spv

glslangValidator.exe -t -V Background_Hdr_Shader.vert -IUtils/ -o Spv/Background_Hdr_Shader.vert.spv
glslangValidator.exe -t -V Background_Hdr_Shader.frag -IUtils/ -o Spv/Background_Hdr_Shader.frag.spv

glslangValidator.exe -t -V F_Transparent_BrokenGlass_Shader.vert -IUtils/ -o Spv/F_Transparent_BrokenGlass_Shader.vert.spv
glslangValidator.exe -t -V F_Transparent_BrokenGlass_Shader.frag -IUtils/ -o Spv/F_Transparent_BrokenGlass_Shader.frag.spv

glslangValidator.exe -t -V Geometry_Shader.vert -IUtils/ -o Spv/Geometry_Shader.vert.spv
glslangValidator.exe -t -V Geometry_Shader.frag -IUtils/ -o Spv/Geometry_Shader.frag.spv

glslangValidator.exe -t -V TBF_OIT_DepthPeeling_Shader.vert -IUtils/ -o Spv/TBF_OIT_DepthPeeling_Shader.vert.spv
glslangValidator.exe -t -V TBF_OIT_DepthPeeling_Shader.frag -IUtils/ -o Spv/TBF_OIT_DepthPeeling_Shader.frag.spv

glslangValidator.exe -t -V TBF_OIT_AlphaBuffer_Shader.vert -IUtils/ -o Spv/TBF_OIT_AlphaBuffer_Shader.vert.spv
glslangValidator.exe -t -V TBF_OIT_AlphaBuffer_Shader.frag -IUtils/ -o Spv/TBF_OIT_AlphaBuffer_Shader.frag.spv

glslangValidator.exe -t -V TBF_OIT_DepthPeelingBlend_Shader.vert -IUtils/ -o Spv/TBF_OIT_DepthPeelingBlend_Shader.vert.spv
glslangValidator.exe -t -V TBF_OIT_DepthPeelingBlend_Shader.frag -IUtils/ -o Spv/TBF_OIT_DepthPeelingBlend_Shader.frag.spv

glslangValidator.exe -t -V TBF_OIT_AlphaBufferBlend_Shader.vert -IUtils/ -o Spv/TBF_OIT_AlphaBufferBlend_Shader.vert.spv
glslangValidator.exe -t -V TBF_OIT_AlphaBufferBlend_Shader.frag -IUtils/ -o Spv/TBF_OIT_AlphaBufferBlend_Shader.frag.spv

glslangValidator.exe -t -V SSAO_Occlusion_Shader.vert -IUtils/ -o Spv/SSAO_Occlusion_Shader.vert.spv
glslangValidator.exe -t -V SSAO_Occlusion_Shader.frag -IUtils/ -o Spv/SSAO_Occlusion_Shader.frag.spv

glslangValidator.exe -t -V HBAO_Occlusion_Shader.vert -IUtils/ -o Spv/HBAO_Occlusion_Shader.vert.spv
glslangValidator.exe -t -V HBAO_Occlusion_Shader.frag -IUtils/ -o Spv/HBAO_Occlusion_Shader.frag.spv

glslangValidator.exe -t -V GTAO_Occlusion_Shader.vert -IUtils/ -o Spv/GTAO_Occlusion_Shader.vert.spv
glslangValidator.exe -t -V GTAO_Occlusion_Shader.frag -IUtils/ -o Spv/GTAO_Occlusion_Shader.frag.spv

glslangValidator.exe -t -V AO_Blur_Shader.vert -IUtils/ -o Spv/AO_Blur_Shader.vert.spv
glslangValidator.exe -t -V AO_Blur_Shader.frag -IUtils/ -o Spv/AO_Blur_Shader.frag.spv

glslangValidator.exe -t -V AO_Cover_Shader.vert -IUtils/ -o Spv/AO_Cover_Shader.vert.spv
glslangValidator.exe -t -V AO_Cover_Shader.frag -IUtils/ -o Spv/AO_Cover_Shader.frag.spv

glslangValidator.exe -t -V PresentShader.vert -IUtils/ -o Spv/PresentShader.vert.spv
glslangValidator.exe -t -V PresentShader.frag -IUtils/ -o Spv/PresentShader.frag.spv

glslangValidator.exe -t -V CSM_ShadowCaster_Shader.vert -IUtils/ -o Spv/CSM_ShadowCaster_Shader.vert.spv
glslangValidator.exe -t -V CSM_ShadowCaster_Shader.frag -IUtils/ -o Spv/CSM_ShadowCaster_Shader.frag.spv

glslangValidator.exe -t -V CSM_Visualization_Shader.vert -IUtils/ -o Spv/CSM_Visualization_Shader.vert.spv
glslangValidator.exe -t -V CSM_Visualization_Shader.frag -IUtils/ -o Spv/CSM_Visualization_Shader.frag.spv

glslangValidator.exe -t -V CascadeEVSM_ShadowCaster_Shader.vert -IUtils/ -o Spv/CascadeEVSM_ShadowCaster_Shader.vert.spv
glslangValidator.exe -t -V CascadeEVSM_ShadowCaster_Shader.frag -IUtils/ -o Spv/CascadeEVSM_ShadowCaster_Shader.frag.spv

glslangValidator.exe -t -V CascadeEVSM_Blit_Shader.vert -IUtils/ -o Spv/CascadeEVSM_Blit_Shader.vert.spv
glslangValidator.exe -t -V CascadeEVSM_Blit_Shader.frag -IUtils/ -o Spv/CascadeEVSM_Blit_Shader.frag.spv

glslangValidator.exe -t -V CascadeEVSM_Blur_Shader.vert -IUtils/ -o Spv/CascadeEVSM_Blur_Shader.vert.spv
glslangValidator.exe -t -V CascadeEVSM_Blur_Shader.frag -IUtils/ -o Spv/CascadeEVSM_Blur_Shader.frag.spv

glslangValidator.exe -t -V CascadeEVSM_Visualization_Shader.vert -IUtils/ -o Spv/CascadeEVSM_Visualization_Shader.vert.spv
glslangValidator.exe -t -V CascadeEVSM_Visualization_Shader.frag -IUtils/ -o Spv/CascadeEVSM_Visualization_Shader.frag.spv


glslangValidator.exe -t -V TBF_LightList_Shader.comp -IUtils/ -o Spv/TBF_LightList_Shader.comp.spv

cmd
