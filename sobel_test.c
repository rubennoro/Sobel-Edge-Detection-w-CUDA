#include <stdio.h> 
#include <stdlib.h>
#include <math.h>

double CLOCK(){
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  return(t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

int main(){

  FILE *file_in = fopen("lena_color.bmp", "r");
  FILE *file_out = fopen("lena_grey.bmp", "w+");
  FILE *file_sobel = fopen("lena_sob.bmp", "w+");

  //First part of code gets greyscale image                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
  if(file_in == NULL){
    printf("This file does not exist\n");
  }
  int i, j;
  unsigned char bytes[54];
  for(i = 0; i < 54; i++){
    bytes[i] = getc(file_in);
  }
  fwrite(bytes, sizeof(unsigned char), 54, file_out);
  fwrite(bytes, sizeof(unsigned char), 54, file_sobel);

  int height = *(int *)&bytes[18];
  int width = *(int *)&bytes[22];
  int bitDepth = *(int *)&bytes[28];

  printf("width: %d\n", width);
  printf("height: %d\n", height);


  int size = height * width;
  //unsigned char rgb[size][3];                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
  //float intensity = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
  unsigned char r, g, b;

  float **intensity = (float**)malloc(height*sizeof(float*));
  float **sobel = (float**)malloc(height*sizeof(float*));
  for(i = 0; i < width; i++){
    intensity[i] = (float*)malloc(width * sizeof(float));
    sobel[i] = (float*)malloc(width*sizeof(float));
  }
  for(i = 0; i < height; i++){
    for(j = 0; j < width; j++){

      b = getc(file_in);
      g = getc(file_in);
      r = getc(file_in);

      intensity[i][j] = 0.299 * r + 0.587 * g + 0.114 * b;
      //sobel[i][j] = intensity[i][j];                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
      putc(intensity[i][j], file_out);
      putc(intensity[i][j], file_out);
      putc(intensity[i][j], file_out);
      //putc(sobel[i][j], file_sobel);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
      //putc(sobel[i][j], file_sobel);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
      //putc(sobel[i][j], file_sobel);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
    }
  }
  float dx, dy;
  float threshold = 128;
  unsigned char final_val = 128;
  /*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
																																																																													     for(i = 1; i < width - 1; i++){                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
  putc(sobel[0][i], file_sobel);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
  putc(sobel[height-1][i], file_sobel);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
  for(i = 0; i < height; i++){                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
  putc(sobel[i][0], file_sobel);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
  putc(sobel[i][width-1], file_sobel);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
}*/
double start, end;
for(i = 0; i < height; i++){
  for(j = 0; j < width; j++){

    if (i == 0 || i == height-1 || j == 0 || j == width-1) {
      putc(0, file_sobel); // You can choose what to write here (e.g., black)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
      putc(0, file_sobel);
      putc(0, file_sobel);
    }
    else{

      dx = (intensity[i-1][j+1]+2*intensity[i][j+1]+intensity[i+1][j+1])-(intensity[i-1][j-1]+2*intensity[i][j-1]+intensity[i+1][j-1]);
      dy = (intensity[i-1][j-1]+2*intensity[i-1][j]+intensity[i-1][j-1])-(intensity[i+1][j-1]+2*intensity[i+1][j]+intensity[i+1][j+1]);
      //print("dx: %f, dy: %f", dx, dy);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
      sobel[i][j] = (float)sqrt((dx*dx)+(dy*dy));

      final_val = sobel[i][j] > threshold? 255 : 0;
      //printf("%d", final_val);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
      putc(final_val, file_sobel);
      putc(final_val, file_sobel);
      putc(final_val, file_sobel);
    }

  }
 }
double tot = start - end;
printf("Time: %f", tot);
for(i = 0; i < width; i++){
  free(intensity[i]);
  free(sobel[i]);
 }
free(intensity);
free(sobel);
fclose(file_in);
fclose(file_out);
fclose(file_sobel);
}
