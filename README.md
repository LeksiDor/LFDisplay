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
+ <a href="https://opencv.org/">OpenCV</a> (tested 4.3) &mdash; mandatory.
+ <a href="https://github.com/LeksiDor/LFRayTracerPBRT/">LFRayTracerPBRT</a> &mdash; optional, but highly recommended.


## <a name="Papers"></a> Published papers

Oleksii Doronin, Erdem Sahin, Robert Bregovic, Atanas Gotchev<br>
<em>A Framework for Assessing Rendering Techniques for Near-Eye Integral Imaging Displays.</em><br>
To appear in ICIP 2020 proceedings.
&mdash; See ExampleNearEyeInImRT sub-project.

Oleksii Doronin, Robert Bregovic, Atanas Gotchev<br>
<em>Optimized 3D Scene Rendering on Projection-Based 3D Displays.</em><br>
&mdash; See ExampleProjectorOptimization sub-project.
To appear in EUSIPCO 2020 proceedings.
