<p style="text-align:center">
    <img src="https://cs184.eecs.berkeley.edu/cs184_sp17_content/article_images/12_.jpg" width="800px" />
</p>

## Deadlines

| <span style="font-weight:normal">[Checkpoint Quiz](https://www.gradescope.com/courses/83189)</span> | **Tuesday 03/10, 11:59PM** |
|:-------------------------------------------------------:|:--------------------------:|
|  <span style="font-weight:normal">Assignment 3-1</span>   | **Tuesday 03/17, 11:59PM** |

You have a total of 5 slip days to use on assignments throughout the semester, but you do not have slip days for checkpoint quizzes. Assignments submitted after 11:59PM are considered a full day late. There are no late minutes or late hours. After you have used all 5 slips days, you will lose 10% for each day you submit an assignment late.

Your final assignment submission must include **both** your code and write-up. Please consult this article on [how to submit assignment for CS184](https://cs184.eecs.berkeley.edu/sp20/article/15/submitting-cs184-assignments).

## Overview
You will implement the core routines of a physically-based renderer using a pathtracing algorithm. This assignment reinforces many of the ideas covered in class recently, including ray-scene intersection, acceleration structures, and physically based lighting and materials. By the time you are done, you'll be able to generate some realistic pictures (given enough patience and CPU time). You will also have the chance to extend the assignment in many technically challenging and intellectually stimulating directions.

**This project is much longer than the other projects. Rendering images for your writeup will take several *hours* at the minimum, so start early! Make sure you read the [Experiments, Report and Deliverables](https://cs184.eecs.berkeley.edu/sp20/article/22/assignment-3-1-write-up) article ahead of time.**

In addition, you may want to render using the instructional machines. Instructions for how to do this are in the [how to build](https://cs184.eecs.berkeley.edu/sp20/article/11/building-and-submitting-cs184-as) article.

## Assignment Structure

This assignment has 5 parts. All parts are equally weighted at 20 points each, for a total of 100 points.

* Part 1: [Ray Generation and Scene Intersection](https://cs184.eecs.berkeley.edu/sp20/article/21/assignment-3-1-part-1)
* Part 2: [Bounding Volume Hierarchy](https://cs184.eecs.berkeley.edu/sp20/article/24/assignment-3-1-part-2)
* Part 3: [Direct Illumination](spec/part-3.md)
* Part 4: [Global Illumination](spec/part-4.md)
* Part 5: [Adaptive Sampling](https://cs184.eecs.berkeley.edu/sp20/article/25/assignment-3-1-part-5)

You will also need to read these articles:

* [Experiments, Report and Deliverables](https://cs184.eecs.berkeley.edu/sp20/article/22/assignment-3-1-write-up)
* [Optional Extra Credit Opportunities](https://cs184.eecs.berkeley.edu/sp20/article/23/assignment-3-1-extra-credit)

In particular, please read the Experiments, Report, and Deliverables page before beginning the project. **There are many deliverables for this project, so please plan accordingly.** Several parts of this assignment ask you to compare various methods/implementations, and we don't want you to be caught off guard!

## Getting Started

As in Assignment 1, you should accept this assignment on your CS184 website profile, follow the instructions on GitHub Classroom, and clone the generated repo (**not** the class skeleton). Make sure you enable GitHub Pages for your assignment.

    $ git clone <YOUR_PRIVATE_REPO>

Please consult this article on [how to build assignments for CS184](https://cs184.eecs.berkeley.edu/sp20/article/11/building-and-submitting-cs184-as). 

**We recommend that you accumulate deliverables into your write-up as you work through each part this assignment.** We have included write-up instructions at the end of each part of this assignment.

## Running the Executable

The executable, `pathtracer`, must be run with a COLLADA file *(.dae)* when you invoke the program. COLLADA files use an XML-based schema to describe a scene graph (much like SVG). They are a hierarchical representation of all objects in the scene (meshes, cameras, lights, etc.), as well as their coordinate transformations. 

There are many command line options for this project, which you can read about below. Use these between the executable name and the *dae* file.

For example, to simply run the regular GUI with the *CBspheres.dae* file and 8 threads, you could type:

    ./pathtracer -t 8 ../dae/sky/CBspheres_lambertian.dae

Unlike previous assignments, we've provided a windowless run mode, which is triggered by providing a filename with the `-f` flag. This is useful for rendering if you're *ssh*-ed into an instructional machine, for example, or if you want to render on your own machine without opening the application window.

If you wanted to save directly to the *spheres_64_16_6.png* file with 64 samples per pixel, 16 samples per light, 6 bounce ray depth, and 480x360 resolution, you might rather use something like this:

    ./pathtracer -t 8 -s 64 -l 16 -m 6 -r 480 360 -f spheres_16_4_6.png ../dae/sky/CBspheres_lambertian.dae

### Rendering with Instructional Machines
    
This means that when trying to generate high quality results for your final writeup, you can use the windowless mode to farm out long render jobs to the s349 machines! You'll probably want to use [*screen*](https://www.howtoforge.com/linux_screen) to keep your jobs running after you logout of *ssh*. After the jobs complete, you can view them using the *display* command, assuming you've *ssh*-ed in with graphics forwarding enabled, or *scp* them back to your local machine.

### A Note about Multi-Threading

We recommend running with 4-8 threads almost always -- the exception is that you should use `-t 1` when debugging with print statements, since `printf` is not guaranteed to be thread safe. (However, cout/cin are thread safe) Furthermore, if you are using a virtual machine, **make sure that you allocate multiple CPU cores to it**. This is important in order for your `-t` flag to work. In practice, it's good to assign $N-2$ cores to the virtual machine, where $N$ is the number of your physical machine's number of CPU cores or hyperthreaded cores.

## Speeding up Rendering Tests

High quality renders take a long time to complete! During development and testing, *don't* spend a long time waiting for full-sized, high quality images. **Make sure you use the parameters specified in the writeup for your website, though!**

Here's some ways to get quick test images:

* Render with fewer samples!
* Utilize the cell rendering feature - start a render with `R`, then hit `C` and click-drag on the viewer. The pathtracer will now only render pixels inside the rectangular region you selected.
* Set a smaller window size using the -r flag (example: `./pathtracer -t 8 -s 64 -r 120 90 ../dae/sky/CBspheres_lambertian.dae`). Zoom out until the entire scene is visible in the window, then start a render with `R`. 

## Using the Executable and Graphical User Interface (GUI)

### Command line options

| Flag and parameters                              | Description                     |
| ------------------------------------ |:--------------------------------------------------:|
|`-s  <INT>` |        Number of camera rays per pixel (default=1, should be a power of 2)  |
|`-l  <INT>` |       Number of samples per area light (default=1) |
|`-t  <INT>` |       Number of render threads (default=1) |
|`-m  <INT>` |        Maximum ray depth (default=1) |
|`-f  <FILENAME>`|   Image (.png) file to save output to in windowless mode |
|`-r  <INT> <INT>`|  Width and height in pixels of output image (if windowless) or of GUI window |
|`-p <x> <y> <dx> <dy>`| Used with the -f flag (windowless mode) to render a cell with its upper left corner at [x,y] and spanning [dx, dy] pixels. |
|`-c  <FILENAME>` |               Load camera settings file &#40;mainly to set camera position when windowless&#41; |
|`-a  <INT> <FLOAT>`|   Samples per batch and tolerance for adaptive sampling |
|`-H` | Enable hemisphere sampling for direct lighting |
|`-h` |               Print command line help message |

### Moving the Camera (in Edit and BVH mode)

| Command                              | Action                                                |
| ------------------------------------ |:--------------------------------------------------:|
| Rotate                 | Left-click and drag                                       |
| Translate             | Right-click and drag                                   |
| Zoom in and out              | Scroll                                          |
| Reset view           | Spacebar                                       |


### Keyboard Commands

| Command                              | Keys                                                |
| ------------------------------------ |:--------------------------------------------------:|
| Mesh-edit mode (default) | <kbd>E</kbd>|
| | | 
| BVH visualizer mode | <kbd>V</kbd>|
| Descend to left/right child (BVH viz) | <kbd>←</kbd> / <kbd>→</kbd> |
| Move up to parent node (BVH viz) | <kbd>↑</kbd> |
| | |
| Start rendering | <kbd>R</kbd> |
| Save a screenshot | <kbd>S</kbd>|
| Decrease/increase area light samples | <kbd>-</kbd> / <kbd>+</kbd>|
| Decrease/increase camera rays per pixel | <kbd>[</kbd> / <kbd>]</kbd>|
| Decrease/increase maximum ray depth | <kbd><</kbd> / <kbd>></kbd>|
| Toggle cell render mode | <kbd>C</kbd>|
| Toggle uniform hemisphere sampling | <kbd>H</kbd>|
| Dump camera settings to file |<kbd>D</kbd>|

**Cell render mode lets you use your mouse to highlight a region of interest so that you can see quick results in that area when fiddling with per pixel ray count, per light ray count, or ray depth.**

## Basic Code Pipeline

What happens when you invoke *pathtracer* in the starter code? Logistical details of setup and parallelization:

1. The `main()` function inside *main.cpp* parses the scene file using a `ColladaParser` from *collada/collada.h*.
2. A new `Viewer` and `Application` are created. `Viewer` manages the low-level OpenGL details of opening the window, and it passes most user input into `Application`. `Application` owns and sets up its own `pathtracer` with a camera and scene. 
3. An infinite loop is started with `viewer.start()`. The GUI waits for various inputs, the most important of which launch calls to `set_up_pathtracer()` and `PathTracer::start_raytracing()`. 
4. `set_up_pathtracer()` sets up the camera and the scene, notably resulting in a call to `PathTracer::build_accel()` to set up the BVH.
5. Inside `start_raytracing()` (implemented in *pathtracer.cpp*), some machinery runs to divide up the scene into "tiles," which are put into a work queue that is processed by `numWorkerThreads` threads.
6. Until the queue is empty, each thread pulls tiles off the queue and runs `raytrace_tile()` to render them. `raytrace_tile()` calls `raytrace_pixel()` for each pixel inside its extent. The results are dumped into the pathtracer's `sampleBuffer`, an instance of an `HDRImageBuffer` (defined in *image.h*).

Most of the core rendering loop is left for you to implement.

1. Inside `raytrace_pixel()`, you will write a loop that calls `camera->generate_ray(...)` to get camera rays and `est_radiance_global_illumination(...)` to get the radiance along those rays.
2. Inside `est_radiance_global_illumination`, you will check for a scene intersection using `bvh->intersect(...)`. If there is an intersection, you will accumulate the return value in `Vector3D L_out`, 
    * adding the BSDF's emission with `zero_bounce_radiance` which uses `bsdf->get_emission()`, 
    * adding global illumination with `at_least_one_bounce_radiance`, which calls `one_bounce_radiance` (which calls a direct illumination function), and recursively calls itself as necessary

You will also be implementing the functions to intersect with triangles, spheres, and bounding boxes, the functions to construct and traverse the BVH, and the functions to sample from various BSDFs. 

Approximately in order, you will edit (at least) the files

* *pathtracer/pathtracer.cpp* (part 1, 3, 4, 5)
* *pathtracer/camera.cpp* (part 1)
* *scene/triangle.cpp* (part 1)
* *scene/sphere.cpp* (part 1)
* *scene/bvh.cpp* (part 2)
* *scene/bbox.cpp* (part 2)
* *pathtracer/bsdf.cpp* (part 3)

You will want to skim over the files:

* *pathtracer/ray.h*
* *pathtracer/intersection.h*
* *pathtracer/sampler.h*
* *pathtracer/sampler.cpp*
* *util/random_util.h*
* *scene/light.h*
* *scene/light.cpp*

since you will be using the classes and functions defined therein.

In addition, *ray.h* contains a defined variable `PART`. This is currently set to 1. You may use this variable if you like to test separate parts independently. For example, you can write things like `if (PART != 4) { return false; }` to easily "revert" parts of your code. **In particular, you will need to set `PART` to 5 to get your rate sampling images in Part 5.**
