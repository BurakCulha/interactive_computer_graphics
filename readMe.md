## these projects uses these:  
opengl, glew, freeglut, glm, c++, cmake, glfw, glad and dear Imgui.   
these packages are installed via:    

sudo apt update    
sudo apt install build-essential    
sudo apt install cmake    
sudo apt install mesa-utils                 
sudo apt install libgl1-mesa-dev            
sudo apt install freeglut3-dev              
sudo apt install libglew-dev                
sudo apt install libglm-dev                 

## you should also install glfw to your system.  
## you can build glfw from source. 
## these 2 commands below install dependencies of glfw.  
sudo apt install xorg-dev  
sudo apt install libwayland-dev libxkbcommon-dev wayland-protocols extra-cmake-modules  
## building glfw from source is like this:
- go to glfw downloads web page or github page.  
- download the source code of glfw.  
- go to glfw source code directory.  
- open that directory in your terminal. and paste these commands:  
    - mkdir build  
    - cd build  
    - cmake ..  
    - sudo make install  
- after the installation process you are ready.  


