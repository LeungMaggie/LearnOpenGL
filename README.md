# LearnOpenGL
Learn OpenGL exercices

## Build with VSCode + MSVC + CMake
1. Prepare MSVC compiler && CMake && C/C++, CMake Tools extensions in VSCode
2. Download glfw from [glfw download page](https://www.glfw.org/download.html)
  * I chose the `Source package`, thus I needed to compile it:
  ```sh
  cd path/to/glfw-3.4
  mkdir build
  cd build
  cmake.exe -B . -S ..
  cmake.exe --build . --config Release -j 8
  ```

  Then the `glfw3.lib` can be found in the folder `path/to/glfw-3.4/build/src/Release`.

3. Generate `glad.zip` from [glad.dav1d.de](https://glad.dav1d.de/)
  * **Language**: choose **C/C++**
  * **Specification** : choose **OpenGL**
  * **API**: 
    * **gl** choose **Version 3.3** or higher
    * **Profile**: choose **Core**
  * **Options**: cochez **Generate a loader**
  * **GENERATE**
  Then click the `glad.zip` and it is downloaded. Unzip this file and you'll get:
    ```
    glad
    ├── include/
    |   ├── glad/
    |   |   └── glad.h
    |   └── KHR/
    |       └── khrplatform.h
    └── src/
        └── glad.c
    ```

4. I organized folders and files in my OpenGL projects as follows:
    ```
    LearnOpenGL
    ├── libraries/
    |   ├── include/
    |   |   ├── glad/        # from glad/include
    |   |   ├── GLFW/        # put glfw3.h from path/to/glfw-3.4/include/GLFW
    |   |   └── KHR/
    |   └── lib/
    |      └── glfw3.lib    # from path/to/glfw-3.4/build/src/Release
    |
    └── MyOpenGLProject/
        ├── src/
        |   └── glad.c
        ├── CMakeLists.txt
        └── main.cpp
    ```
    Thus, my `CMakeLists.txt` in every `MyOpenGLProject` is like:
    ```cmake
    cmake_minimum_required(VERSION 3.10)
    project(MyOpenGLProject)

    set(GL_LIB_PATH "path/to/LearnOpenGL/libraries")
    include_directories("${GL_LIB_PATH}/include")
    link_directories("${GL_LIB_PATH}/lib")

    set(EXE_NAME "MyOpenGLProject")
    add_executable(${EXE_NAME} main.cpp src/glad.c)
    target_link_libraries(${EXE_NAME} glfw3 opengl32.lib)
    ```
5. Same way to build every `MyOpenGLProject`:
    ```sh
    cd path/to/LearnOpenGL/MyOpenGLProject
    mkdir build
    cd build
    cmake.exe -B . -S ..
    cmake.exe --build . --config Release -j 8
    ```
