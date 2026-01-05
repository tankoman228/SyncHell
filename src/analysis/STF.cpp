#include <iostream> // For output
#include <fstream>  // For file streams
#include <vector>
#include <string>
#include <sstream>  // For efficient reading into a string
#include <CL/cl.h>
#include <Spectro.hpp>

namespace STF {

    Spectro::Spectrogram SpectroToStf(const Spectro::Spectrogram &spectro, int chunkSize) {

        size_t spectroWidth = spectro.timeLength;                                                    
        size_t spectroHeight = spectro.heigth;                      

        size_t numChunks = spectroWidth / chunkSize;

        std::cout << "Processing: height=" << spectroHeight << ", width=" << spectroWidth
            << ", chunkSize=" << chunkSize << ", numChunks=" << numChunks << "\n";
                        
        std::vector<float> flatInput;
        flatInput.reserve(spectroHeight * spectroWidth);

        for (size_t timeIdx = 0; timeIdx < spectroWidth; ++timeIdx) {
            float* frequencyColumn = spectro.spectro[timeIdx];
            flatInput.insert(flatInput.end(), frequencyColumn, frequencyColumn + spectroHeight);
        }

        std::cout << "Flat input copied";

        //           OpenCL
        cl_int err;
        cl_uint numPlatforms;
        cl_platform_id platform = nullptr;
        err = clGetPlatformIDs(1, &platform, &numPlatforms);
        if (err != CL_SUCCESS || numPlatforms == 0) {
            std::cerr << "No OpenCL platform found, error: " << err << "\n";
            return {};
        }

        cl_uint numDevices;
        cl_device_id device = nullptr;
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &numDevices);
        if (err != CL_SUCCESS) {
            //         CPU
            err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, &numDevices);
            if (err != CL_SUCCESS) {
                std::cerr << "No OpenCL device found\n";
                return {};
            }
        }

        cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cerr << "Failed to create context, error: " << err << "\n";
            return {};
        }

        cl_queue_properties props[] = { 0 };
        cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, props, &err);
        if (err != CL_SUCCESS) {
            std::cerr << "Failed to create command queue, error: " << err << "\n";
            clReleaseContext(context);
            return {};
        }



        //        kernel     
        std::ifstream inputFile("assets/stf.cl", std::ios::binary);
        if (!inputFile.is_open()) {
            std::cerr << "Failed to open kernel file CTF.cl!\n";
            clReleaseCommandQueue(queue);
            clReleaseContext(context);
            return {};
        }
        std::string fileContent((std::istreambuf_iterator<char>(inputFile)),
            std::istreambuf_iterator<char>());
        inputFile.close();
        if (fileContent.empty()) {
            std::cerr << "Kernel file is empty!\n";
            clReleaseCommandQueue(queue);
            clReleaseContext(context);
            return {};
        }

        const char* kernelSrc = fileContent.c_str();
        size_t kernelLength = fileContent.length();

        cl_program program = clCreateProgramWithSource(context, 1, &kernelSrc, &kernelLength, &err);
        if (err != CL_SUCCESS) {
            std::cerr << "Failed to create program, error: " << err << "\n";
            clReleaseCommandQueue(queue);
            clReleaseContext(context);
            return {};
        }

        err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
        if (err != CL_SUCCESS) {
            size_t logSize = 0;
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
            if (logSize > 1) {
                std::vector<char> log(logSize);
                clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);
                std::cerr << "Build failed:\n" << log.data() << "\n";
            }
            clReleaseProgram(program);
            clReleaseCommandQueue(queue);
            clReleaseContext(context);
            return {};
        }

        cl_kernel kernel = clCreateKernel(program, "process", &err);
        if (err != CL_SUCCESS) {
            std::cerr << "Failed to create kernel, error: " << err << "\n";
            clReleaseProgram(program);
            clReleaseCommandQueue(queue);
            clReleaseContext(context);
            return {};
        }

        //               
        cl_mem bufferIn = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(float) * flatInput.size(), flatInput.data(), &err);
        if (err != CL_SUCCESS) {
            std::cerr << "Failed to create input buffer, error: " << err << "\n";
            clReleaseKernel(kernel);
            clReleaseProgram(program);
            clReleaseCommandQueue(queue);
            clReleaseContext(context);
            return {};
        }

        size_t outputSize = 256 * numChunks;
        std::vector<float> flatOutput(outputSize, 0.0f);
        cl_mem bufferOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
            sizeof(float) * outputSize, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cerr << "Failed to create output buffer, error: " << err << "\n";
            clReleaseMemObject(bufferIn);
            clReleaseKernel(kernel);
            clReleaseProgram(program);
            clReleaseCommandQueue(queue);
            clReleaseContext(context);
            return {};
        }

        //                        
        clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferIn);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferOut);
        int h = (int)spectroHeight;
        int w = (int)spectroWidth;
        int cs = (int)chunkSize;
        clSetKernelArg(kernel, 2, sizeof(int), &h);
        clSetKernelArg(kernel, 3, sizeof(int), &w);
        clSetKernelArg(kernel, 4, sizeof(int), &cs);

        size_t globalSize = numChunks; //                      

        err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, nullptr, 0, nullptr, nullptr);
        if (err != CL_SUCCESS) {
            std::cerr << "Failed to execute kernel, error: " << err << "\n";
            clReleaseMemObject(bufferOut);
            clReleaseMemObject(bufferIn);
            clReleaseKernel(kernel);
            clReleaseProgram(program);
            clReleaseCommandQueue(queue);
            clReleaseContext(context);
            return {};
        }

        clFinish(queue);

        //                  
        err = clEnqueueReadBuffer(queue, bufferOut, CL_TRUE, 0, sizeof(float) * outputSize,
            flatOutput.data(), 0, nullptr, nullptr);
        if (err != CL_SUCCESS) {
            std::cerr << "Failed to read output buffer, error: " << err << "\n";
        }

        Spectro::Spectrogram result;
        result.heigth = 256;
        result.timeLength = numChunks;
        result.spectro = new float*[numChunks];

        //result.reserve(numChunks); //            chunk'  

        for (size_t chunk = 0; chunk < numChunks; chunk++) {
            result.spectro[chunk] = new float[256];
            for (size_t feature = 0; feature < 256; feature++) {
                result.spectro[chunk][feature] = flatOutput[chunk * 256 + feature];
            }
        }

        //         OpenCL         
        clReleaseMemObject(bufferOut);
        clReleaseMemObject(bufferIn);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);

        std::cout << "Processing completed successfully\n";
        return result;
    }
}