Sobel Edge Detection

The intensity of each element in an image converted to a matrix is determined based on its RGB values. This is referred to as greyscaling an image. Both the C file and CUDA file begin
by greyscaling the image before determining edges.

The Sobel Operators evaluate the difference in the intensity of the pixels in the horizontal direction across one element as well as the vertical direction. The magnitude of the difference is
then calculated and compared with a set threshold. If greater, the intensity of that pixel is then set at 255, where others are set at 0. The output is shown by bmp_out for an example 1024x1024 bitmap.
