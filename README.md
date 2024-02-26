This is my personal repo for following along with the openGL tutorial, with a mix of tutorial code, my code, and an custom point blazepoze decomposed point animation system!

Here are some fun examples:

![WHEEE](https://github.com/MaxRStevens-1/OpenGL_Testing/blob/skeleton_blazepose/git_gifs/sping.gif)
![So Flexible!](https://github.com/MaxRStevens-1/OpenGL_Testing/blob/skeleton_blazepose/git_gifs/bow.gif)


TODO: 
    1. I need to create a bodymodel 3dmodel vamp bone -> assimp 3d model bone / joint mapping to accurately apply transformations
    2. create hashmap of assimp 3d model bone name -> transformation matrix  
        A. possibly need to remake with scalar matrix instead of prescaling points
        B. transformation = translatoin * rotation * scale
    3. create own drawer which uses opengl model animator bone name -> indicies to pass matrices to gpu
    4. verify / bugfix