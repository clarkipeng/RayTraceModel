# RayTraceModel
CPU Raytracer based on the [Ray Tracing in One Weekend Series](https://raytracing.github.io/) with basic Assimp model loading

You will need to install [Assimp](https://github.com/assimp/assimp) in order to load models using this code. Materials need to be manually defined for custom models. Below is a backpack model with a metallic material:

![backback_image](https://github.com/user-attachments/assets/05980ba5-a633-4a4b-b734-3c737aec8fe8)

This code uses multithreading and bounding volume hierarchies(bvhs) in order to speed up the rendering process. However rendering is still painfully slow. Run `main.cpp` to generate images.

Some rendered images I made can be found in the "good images" folder.

After using probability distribution functions(pdfs) to optimize light calculations some materials in the material.h file may no longer be functional.
