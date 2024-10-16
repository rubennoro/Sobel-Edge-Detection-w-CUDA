#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define THRESHOLD 128
#define BLOCKS 16
#define THREADS 32

__global__ void sobel_gpu(float* intensity, float* output, int height, int width){
    int row = threadIdx.x + blockIdx.x * blockDim.x;
    int col = threadIdx.y + blockIdx.y * blockDim.y;
    float dx, dy;

    if(row > 0 && col > 0 && col < height-1 && row < width-1){
    	
	dx = (intensity[(row-1)*width+(col+1)]+2*intensity[(row)*width+(col+1)]+intensity[(row+1)*width+(col+1)]) - (intensity[(row-1)*width+(col-1)]+2*intensity[(row)*width+(col-1)]+intensity[(row+1)*width+(col-1)]);
	
	dy = (intensity[(row-1)*width+(col-1)]+2*intensity[(row-1)*width+col]+intensity[(row-1)*width+(col+1)]) - (intensity[(row+1)*width+(col-1)]+2*intensity[(row+1)*width+(col)]+intensity[(row+1)*width+(col+1)]);


	//dx = (intensity[i-1][j+1]+2*intensity[i][j+1]+intensity[i+1][j+1])-(intensity[i-1][j-1]+2*intensity[i][j-1]+intensity[i+1][j-1]);
        //dy = (intensity[i-1][j-1]+2*intensity[i-1][j]+intensity[i-1][j-1])-(intensity[i+1][j-1]+2*intensity[i+1][j]+intensity[i+1][j+1]);
        //print("dx: %f, dy: %f", dx, dy);                                                                                                                                                                                                                                          
        output[row*width+col] = (float)sqrt((dx*dx)+(dy*dy));

         
    }    
}

int main(){

    FILE *file_in = fopen("bmp_file_exp.bmp", "r");
    FILE *file_sobel = fopen("bmp_out.bmp", "w+");

    if(file_in == NULL){
        printf("File is not found");
    }
    int i, j;
    unsigned char bytes[54];
    for(i = 0; i < 54; i++){
    	  bytes[i] = getc(file_in);
    }
    fwrite(bytes, sizeof(unsigned char), 54, file_in);
    fwrite(bytes, sizeof(unsigned char), 54, file_sobel);
    
    int height = *(int *)&bytes[18];
    int width = *(int *)&bytes[22];
    
    int size = width * height;
    printf("WidthxHeight: %d\n", size);
    float* intensity;
    float* sobel;
    cudaMallocHost((void **) &intensity, sizeof(float) * size);
    cudaMallocHost((void **) &sobel, sizeof(float) * size);
 
    //float* intensity = malloc(size * sizeof(float));
    //float* sobel = malloc(size * sizeof(float));
    unsigned char r, g, b;
    
    //Calculation for Intensity of operation file
    for(i = 0; i < size; i++){
    	  b = getc(file_in);
	  g = getc(file_in);
	  r = getc(file_in);
    	  intensity[i] = 0.299 * r + 0.587 * g + 0.114 * b;
	  sobel[i] = 0;
    }
    //Tracking time it took
    float gpu_time_ms;

    float* sobel_cu;
    float* intensity_cu;
    
    cudaMalloc((void **) &sobel_cu, size * sizeof(float));
    cudaMalloc((void **) &intensity_cu, size * sizeof(float));
    
    cudaMemcpy(sobel_cu, sobel, sizeof(float) * size, cudaMemcpyHostToDevice);
    cudaMemcpy(intensity_cu, intensity, sizeof(float) * size, cudaMemcpyHostToDevice);
    
    unsigned int threads_per = THREADS;
    
    //MATH TO FULLY COVER THE MATRIX
    unsigned int blocks_per_x = (width+THREADS)-1/(THREADS);
    unsigned int blocks_per_y = (height+THREADS)-1/(THREADS);
    
    //DEFINE THESE VALUES
    dim3 threadsPerBlock(threads_per, threads_per);
    dim3 numBlocks(blocks_per_x, blocks_per_y);
    
    printf("here\n");

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);    
    cudaEventRecord(start, 0);

    sobel_gpu<<<numBlocks, threadsPerBlock>>>(intensity_cu, sobel_cu, height, width); 

    cudaDeviceSynchronize();
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    cudaMemcpy(sobel, sobel_cu, sizeof(float)*size, cudaMemcpyDeviceToHost);
    
    cudaEventElapsedTime(&gpu_time_ms, start, stop);
    printf("Time: %f\n", gpu_time_ms);
    float final_val;
    for(i = 0; i < size; i++){
    	  
    	  final_val = sobel[i] > THRESHOLD? 255 : 0;
	  putc(final_val, file_sobel);
	  putc(final_val, file_sobel);
	  putc(final_val, file_sobel);
    }

    cudaFreeHost(intensity);
    cudaFreeHost(sobel);
    cudaFree(sobel_cu);
    cudaFree(intensity_cu);
    fclose(file_sobel);
    fclose(file_in);
    return 0;    

}
