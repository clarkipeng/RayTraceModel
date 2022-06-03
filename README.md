# RayTraceModel
CPU Raytracer based on the [Ray Tracing in One Weekend Series](https://raytracing.github.io/) with basic Assimp model loading


You will need to install [Assimp](https://github.com/assimp/assimp) in order to load models using this code.


This code uses multithreading and bounding volume hierarchies(bvhs) in order to speed up the rendering process. However rendering is still painfully slow.

Some rendered images I made can be found in the "good images" folder.

After using probability distribution functions(pdfs) to optimize light calculations some materials in the material.h file may no longer be functional.
