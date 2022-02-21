## Incremental OpenGL Codebase

This repository is created with a purpose of very easy to understand OpenGL implementation. There are better ways to organize this code, no way this code/style should be treated as good practices. Aim is to have incremental codebase which clearly highlights incremental feature updates & some way to keeping things organized. 

Here's the final output:

![image](https://user-images.githubusercontent.com/5098227/154793719-bbe19bbf-d470-47ce-82a6-9a236e1d0416.png)

Some of the features: https://www.youtube.com/watch?v=DwSYo4xOgGw

Step by step images:

| Topic | Description |
| ----------- | ----------- |
| [[Create Window](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/01_WindowCreation) | ![image](https://user-images.githubusercontent.com/5098227/150085190-3313c039-304b-415b-8acb-cffce6437776.png)|
| [Create Quad](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/02_WindowCreation_Quad) | ![image](https://user-images.githubusercontent.com/5098227/150499027-039c9a48-c2d1-4f0c-a765-9217e213c463.png) |
| [Create Textured Quad](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/03_WindowCreation_Quad_Texture) | ![image](https://user-images.githubusercontent.com/5098227/150506409-1f163185-84f7-43a1-a80b-0341514656bb.png) |
| [Create Cube](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/04_GLCube) | ![image](https://user-images.githubusercontent.com/5098227/150633271-68745cd8-2804-45ce-b77c-8ea63a91c8f5.png) |
| [Free Roam Camera](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/05_GLCube_Camera)| ![image](https://user-images.githubusercontent.com/5098227/150636347-e8db2cac-8e37-4db0-9311-a6ae9396e7a6.png) |
| [Skybox Rendering](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/06_Skybox)| ![image](https://user-images.githubusercontent.com/5098227/150637009-4f229e24-631e-46df-8b98-3ced7b9415ec.png) |
| [Custom Mesh Loading using Assimp](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/07_Custom_Mesh) | ![image](https://user-images.githubusercontent.com/5098227/151302871-29d8148e-90e3-4ff6-92b7-2d8d35ff19a0.png) |
| [Custom Mesh loading with textures](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/08_Custom_Mesh%20Texturing) | ![image](https://user-images.githubusercontent.com/5098227/151336569-87769798-0c72-440f-83ea-5924e5a94b0d.png) |
| [Custom Mesh loading with multiple textures](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/09_Custom_Mesh%20MultiTexturing)| ![image](https://user-images.githubusercontent.com/5098227/151505503-f3f98984-5eb3-4aa1-8c16-4297c4e771fd.png) |
| [Diffuse lighting](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/10_Basic_Diffuse_Lighting) | ![image](https://user-images.githubusercontent.com/5098227/151507737-e8b3f383-64d2-406e-b71e-f7c57437ccd6.png) |
| [Specular lighting](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/11_Basic_Specular_Lighting) | ![image](https://user-images.githubusercontent.com/5098227/151518771-974d399e-1910-4a14-b583-60b717d4c0ad.png) |
| [Environment Mapping](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/12_Basic_Environment_Reflection)| ![image](https://user-images.githubusercontent.com/5098227/151526272-4d1bc947-e2e2-444f-b54a-875303e8e5cc.png) |
| [Basic lights management](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/13_Lighting_System)| ![image](https://user-images.githubusercontent.com/5098227/151527942-4c9ff540-1e72-4fbc-a3c4-d602583ad3e4.png) |
| [World Space Normal mapping](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/14_NormalMapping_WorldSpace)| ![image](https://user-images.githubusercontent.com/5098227/151652318-ab177180-63d8-4f2e-9c45-490b3ec847ab.png) |
| [Tangent Space normal mapping](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/15_NormalMapping_TangentSpace) | ![image](https://user-images.githubusercontent.com/5098227/151653200-6b308610-b10e-48e8-bfce-e52de1d3aef7.png) |
| [Framebuffers](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/16_Framebuffers)| ![image](https://user-images.githubusercontent.com/5098227/151656929-60b63684-918b-4bd8-8ae7-1f40507dc638.png) |
| [HDR Framebuffers](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/17_Framebuffers%20HDR)| ![image](https://user-images.githubusercontent.com/5098227/151658537-1a922b49-c223-48c2-878e-694b904045f5.png) |
| [Simple Bloom effect](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/18_Bloom)| ![image](https://user-images.githubusercontent.com/5098227/151698982-012e594f-9889-499b-9938-8c471b660fc7.png) |
| [Deferred Rendering](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/19_Deferred) | ![image](https://user-images.githubusercontent.com/5098227/151744474-80d9cb19-2eea-4363-8624-1bf4c441cc21.png)|
| [Bloom with Deferred Rendering](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/20_Deferred_Bloom) | ![image](https://user-images.githubusercontent.com/5098227/151750736-e9143d56-4010-42bc-9f3f-c75f6cd57f50.png) |
| [Shadow mapping with Deferred Rendering](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/21_Deferred_ShadowMapping) | ![image](https://user-images.githubusercontent.com/5098227/152962001-0fbc8905-7a26-40b4-920d-fd58074bf161.png) |
| [Debug views](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/22_Deferred_DebugViews) | ![image](https://user-images.githubusercontent.com/5098227/153569300-ff651bf5-734b-41c3-b449-308f869de264.png) |
| [HDRI Skybox with Deferred Rendering](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/23_Deferred_HDRI) | ![image](https://user-images.githubusercontent.com/5098227/153581518-a0a4160b-8b00-4f4b-a6b8-26a65b916432.png) |
| [Wireframe Rendering](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/24_Wireframe)| ![image](https://user-images.githubusercontent.com/5098227/153607449-9854fcd8-06ee-48e5-8418-5380047fc0a3.png) |
| [IMGUI Integration](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/25_UI) | ![image](https://user-images.githubusercontent.com/5098227/153703995-44cf18e7-e22a-4085-bc64-53ad8faa1841.png) |
| [PBR + Mask](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/26_PBR)| ![image](https://user-images.githubusercontent.com/5098227/154259284-941eb3b7-2530-4129-8eb8-dd209b1f8434.png) |
| [PBR Diffuse IBL](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/27_PBR_IBL_Diffuse) | ![image](https://user-images.githubusercontent.com/5098227/154328099-cd47b193-3983-4e56-9aa8-0f368289dc8f.png) |
| [PBR Specular IBL](https://github.com/TheOrestes/OpenGL_StepByStep/tree/master/28_PBR_IBL_DiffuseSpecular) | ![image](https://user-images.githubusercontent.com/5098227/154792782-4f5ee71e-8e7a-4530-aa69-414ecf3b55b3.png) |

References: 
1. https://learnopengl.com/
2. http://www.songho.ca/opengl/
3. https://ogldev.org/
