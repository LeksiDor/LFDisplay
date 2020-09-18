# Light Field (LF) Display project


## Table of Contents
+ [Introduction](#Introduction)
+ [How to use](#HowToUse)
+ [Published papers](#Papers)


## <a name="Introduction"></a> Introduction

<em> What is it all about? </em><br>
This project was created as a part of doctorate research by Oleksii Doronin.
It contains a modelling of the most common types of light field dipslays, including some algorithms to enhance the quality of their visual output without changing the hardware configuration.

<em> What is light field display? </em><br>
Light field (LF) display is a 3D display which is able to show different picture from different viewing angles, without the need for user to wear a special glasses or enabling position tracking.
The most common types of LF displays are lenslet-based (integral imaging, also most of multi-view, and more simple parallax barrier), projector-based (e.g., Holografika), and multiplication-based (e.g., MIT Tensor).

<em> Where is the source code from your paper? </em><br>
If you learned about this project from one of my papers, you can go to [Published papers](#Papers) section.


## <a name="HowToUse"></a> How to use

This project was created and maintained on Windows 10 machine, but potentially can be easily transfered to another platform.

It has the following dependencies:<br>
+ [OpenCV](https://opencv.org/) (tested 4.3) &mdash; mandatory.
+ [LFRayTracerPBRT](https://github.com/LeksiDor/LFRayTracerPBRT/) &mdash; mandatory (for now).

You should build the project as follows.<br>
1. Make sure you have OpenCV build, preferably 4.3. If not, [download](https://opencv.org/) it.
2. Clone this project to some folder on your machine. E.g., in `D:/Work/` call `git clone https://github.com/LeksiDor/LFDisplay.git`.
3. Clone LFRayTracerPBRT to the same folder. E.g., in `D:/Work/` call `git clone --recursive https://github.com/LeksiDor/LFRayTracerPBRT.git`.
4. Build LFRayTracerPBRT. For detailed instructions, go to its [project page](https://github.com/LeksiDor/LFRayTracerPBRT/). Shortly, you should execute a regular CMake build, while choosing build folder as, e.g., `D:/Work/LFRayTracerPBRT/build`. Don't forget to check `PBRT_FLOAT_AS_DOUBLE` flag. Then, compile in both Debug and Release.
5. Call `Initialize3rdparty.sh` script from `D:/Work/LFDisplay/` folder. This script will instantiate the `3rdparty` folder, which contains all necessary things from LFRayTracerPBRT.
6. Build this project using CMake. Choose build folder as `D:/Work/LFDisplay/build`.
7. Compile this project. If you have errors, it's not ok.




## <a name="Papers"></a> Published papers

Oleksii Doronin, Erdem Sahin, Robert Bregovic, Atanas Gotchev<br>
***A Framework for Assessing Rendering Techniques for Near-Eye Integral Imaging Displays.***<br>
To appear in ICIP 2020 proceedings.<br>
**Source code folder**: /src/ExampleICIP2020/ <br>
**Source code Wiki**: [ExampleICIP2020](https://github.com/LeksiDor/LFDisplay/wiki/ExampleICIP2020)

Oleksii Doronin, Robert Bregovic, Atanas Gotchev<br>
***Optimized 3D Scene Rendering on Projection-Based 3D Displays.***<br>
To appear in EUSIPCO 2020 proceedings.<br>
**Source code folder**: /src/ExampleEUSIPCO2020/ <br>
