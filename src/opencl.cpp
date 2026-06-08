#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <chrono>
#include <CL/cl.h>

const char* kernel_source = R"(
__kernel void hitung_jarak_openCL(
    __global const uchar* pixel,
    __global const int* centroid,
    __global float* jarak,
    const int width,
    const int height)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if(x >= width || y >= height)
        return;

    int i = y * width + x;

    for(int c = 0; c < 2; c++)
    {
        float r = (float)pixel[i * 3 + 0] - centroid[c * 3 + 0];
        float g = (float)pixel[i * 3 + 1] - centroid[c * 3 + 1];
        float b = (float)pixel[i * 3 + 2] - centroid[c * 3 + 2];

        jarak[i * 2 + c] = sqrt(r*r + g*g + b*b);
    }
}
)";

int main()
{
    std::vector<int> ukuran_gambar = {128, 256, 512, 1024, 2048};
    std::vector<int> jumlah_pixel = {
        128 * 128,
        256 * 256,
        512 * 512,
        1024 * 1024,
        2048 * 2048
    };

    constexpr int jumlah_gambar = 5;

    std::array<std::array<std::array<int, 3>, 2>, jumlah_gambar> centroids;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for(int i = 0; i < jumlah_gambar; i++)
    {
        std::cout
            << "Gambar ke-" << i + 1
            << " | Ukuran "
            << ukuran_gambar[i]
            << "x"
            << ukuran_gambar[i]
            << " | Pixel "
            << jumlah_pixel[i]
            << std::endl;
    }

    std::cout << "-------------------------------------------------\n";

    for(int i = 0; i < jumlah_gambar; i++)
    {
        for(int c = 0; c < 2; c++)
        {
            centroids[i][c][0] = dis(gen);
            centroids[i][c][1] = dis(gen);
            centroids[i][c][2] = dis(gen);
        }
    }

    cl_int err;

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    err = clGetPlatformIDs(1, &platform, nullptr);

    if(err != CL_SUCCESS)
    {
        std::cerr << "Gagal mendapatkan platform OpenCL\n";
        return -1;
    }

    err = clGetDeviceIDs(
        platform,
        CL_DEVICE_TYPE_GPU,
        1,
        &device,
        nullptr
    );

    if(err != CL_SUCCESS)
    {
        std::cout
            << "GPU tidak ditemukan. Menggunakan CPU OpenCL.\n";

        err = clGetDeviceIDs(
            platform,
            CL_DEVICE_TYPE_CPU,
            1,
            &device,
            nullptr
        );

        if(err != CL_SUCCESS)
        {
            std::cerr
                << "Tidak ada device OpenCL tersedia.\n";
            return -1;
        }
    }

    context = clCreateContext(
        nullptr,
        1,
        &device,
        nullptr,
        nullptr,
        &err
    );

    if(err != CL_SUCCESS)
    {
        std::cerr << "Gagal membuat context.\n";
        return -1;
    }

#if CL_TARGET_OPENCL_VERSION >= 200
    queue = clCreateCommandQueueWithProperties(
        context,
        device,
        nullptr,
        &err
    );
#else
    queue = clCreateCommandQueue(
        context,
        device,
        0,
        &err
    );
#endif

    if(err != CL_SUCCESS)
    {
        std::cerr << "Gagal membuat command queue.\n";
        return -1;
    }

    program = clCreateProgramWithSource(
        context,
        1,
        &kernel_source,
        nullptr,
        &err
    );

    err = clBuildProgram(
        program,
        1,
        &device,
        nullptr,
        nullptr,
        nullptr
    );

    if(err != CL_SUCCESS)
    {
        size_t log_size;

        clGetProgramBuildInfo(
            program,
            device,
            CL_PROGRAM_BUILD_LOG,
            0,
            nullptr,
            &log_size
        );

        std::vector<char> log(log_size);

        clGetProgramBuildInfo(
            program,
            device,
            CL_PROGRAM_BUILD_LOG,
            log_size,
            log.data(),
            nullptr
        );

        std::cerr << "\nBUILD ERROR:\n";
        std::cerr << log.data() << std::endl;

        return -1;
    }

    kernel = clCreateKernel(
        program,
        "hitung_jarak_openCL",
        &err
    );

    std::cout << "\n=== EKSEKUSI OPENCL NDRange 2D ===\n\n";

    for(int i = 0; i < jumlah_gambar; i++)
    {
        int width = ukuran_gambar[i];
        int height = ukuran_gambar[i];
        int n_pixel = jumlah_pixel[i];

        std::vector<unsigned char> pixels(n_pixel * 3);
        std::vector<float> jarak(n_pixel * 2);

        for(auto& p : pixels)
        {
            p = static_cast<unsigned char>(dis(gen));
        }

        auto start =
            std::chrono::high_resolution_clock::now();

        cl_mem pixel_buf = clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            pixels.size() * sizeof(unsigned char),
            pixels.data(),
            &err
        );

        cl_mem centroid_buf = clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(int) * 6,
            centroids[i].data(),
            &err
        );

        cl_mem jarak_buf = clCreateBuffer(
            context,
            CL_MEM_WRITE_ONLY,
            jarak.size() * sizeof(float),
            nullptr,
            &err
        );

        if(err != CL_SUCCESS)
        {
            std::cerr
                << "Gagal membuat buffer gambar "
                << i + 1
                << std::endl;
            continue;
        }

        clSetKernelArg(kernel, 0, sizeof(cl_mem), &pixel_buf);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), &centroid_buf);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), &jarak_buf);
        clSetKernelArg(kernel, 3, sizeof(int), &width);
        clSetKernelArg(kernel, 4, sizeof(int), &height);

        size_t local_size[2] = {16, 16};

        size_t global_size[2] =
        {
            ((size_t)width + local_size[0] - 1)
            / local_size[0] * local_size[0],

            ((size_t)height + local_size[1] - 1)
            / local_size[1] * local_size[1]
        };

        err = clEnqueueNDRangeKernel(
            queue,
            kernel,
            2,
            nullptr,
            global_size,
            local_size,
            0,
            nullptr,
            nullptr
        );

        if(err != CL_SUCCESS)
        {
            std::cerr
                << "Gagal menjalankan kernel. Error: "
                << err
                << std::endl;
        }

        clFinish(queue);

        clEnqueueReadBuffer(
            queue,
            jarak_buf,
            CL_TRUE,
            0,
            jarak.size() * sizeof(float),
            jarak.data(),
            0,
            nullptr,
            nullptr
        );

        auto end =
            std::chrono::high_resolution_clock::now();

        double elapsed =
            std::chrono::duration<double>(
                end - start
            ).count();

        std::cout
            << "Gambar "
            << width
            << "x"
            << height
            << "\n";

        std::cout
            << "Global NDRange : "
            << global_size[0]
            << " x "
            << global_size[1]
            << "\n";

        std::cout
            << "Local NDRange  : "
            << local_size[0]
            << " x "
            << local_size[1]
            << "\n";

        std::cout
            << "Waktu Eksekusi : "
            << elapsed
            << " detik\n\n";

        clReleaseMemObject(pixel_buf);
        clReleaseMemObject(centroid_buf);
        clReleaseMemObject(jarak_buf);
    }

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}