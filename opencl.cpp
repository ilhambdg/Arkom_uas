#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>
#include <iostream>
#include <vector>

using namespace std;

// LANGKAH 1: Tulis rumus matematika (Kernel) untuk GPU dalam bentuk string C++
const char* kernelSource = R"(
__kernel void tambah_array(__global const int* A, __global const int* B, __global int* C) {
    int id = get_global_id(0); // Dapatkan nomor ID utas/buruh GPU
    C[id] = A[id] + B[id];     // Setiap utas mengerjakan satu indeks data
}
)";

int main() {
    // LANGKAH 2: Cari Platform dan Device GPU (Dari kode Anda)
    vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        cerr << "Tidak ada platform OpenCL!\n";
        return -1;
    }

    // Ambil platform pertama dan device GPU pertama
    cl::Platform platform = platforms[0];
    vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices); // Kita ganti ke CL_DEVICE_TYPE_GPU agar pasti pakai GPU
    
    if (devices.empty()) {
        cerr << "Tidak ditemukan GPU pada platform ini. Mencoba pakai CPU...\n";
        platform.getDevices(CL_DEVICE_TYPE_CPU, &devices); // Fallback ke CPU jika GPU tidak ada
    }
    cl::Device device = devices[0];
    cout << "Menggunakan Perangkat: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

    // LANGKAH 3: Buat Context dan Command Queue (Jalur komunikasi dan perintah)
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    // LANGKAH 4: Siapkan data di C++ (RAM Laptop)
    const int UKURAN_DATA = 5;
    int data_A[UKURAN_DATA] = {10, 20, 30, 40, 50};
    int data_B[UKURAN_DATA] = {1,  2,  3,  4,  5};
    int data_Hasil[UKURAN_DATA] = {0,  0,  0,  0,  0}; // Wadah kosong untuk hasil dari GPU

    // LANGKAH 5: Alokasi memori di GPU (VRAM) dan kirim data dari RAM ke VRAM
    // CL_MEM_READ_ONLY = GPU cuma baca data ini
    // CL_MEM_USE_HOST_PTR = Mengirim data dari pointer array C++ kita
    cl::Buffer buffer_A(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(int) * UKURAN_DATA, data_A);
    cl::Buffer buffer_B(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(int) * UKURAN_DATA, data_B);
    // CL_MEM_WRITE_ONLY = GPU akan menulis hasil ke memori ini
    cl::Buffer buffer_Hasil(context, CL_MEM_WRITE_ONLY, sizeof(int) * UKURAN_DATA);

    // LANGKAH 6: Compile/Build kode kernel GPU saat runtime
    cl::Program::Sources sources;
    sources.push_back({kernelSource, strlen(kernelSource)});
    cl::Program program(context, sources);
    
    if (program.build({device}) != CL_SUCCESS) {
        cerr << "Gagal compile kode kernel! Log Error: " 
             << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
        return -1;
    }

    // LANGKAH 7: Ambil fungsi spesifik di dalam kernel dan pasang argumennya
    cl::Kernel kernel_tambah(program, "tambah_array");
    kernel_tambah.setArg(0, buffer_A);
    kernel_tambah.setArg(1, buffer_B);
    kernel_tambah.setArg(2, buffer_Hasil);

    // LANGKAH 8: Eksekusi NDRange (Tombol START untuk menyuruh GPU jalan)
    // cl::NullRange = offset awal (isi default)
    // cl::NDRange(UKURAN_DATA) = TOTAL UTAS yang jalan. Karena data kita ada 5, kita jalankan 5 utas paralel!
    queue.enqueueNDRangeKernel(kernel_tambah, cl::NullRange, cl::NDRange(UKURAN_DATA), cl::NullRange);

    // LANGKAH 9: Ambil hasil perhitungan dari GPU (VRAM) kembali ke RAM Laptop
    // CL_TRUE = Tunggu sampai proses salin selesai baru lanjut ke baris C++ berikutnya
    queue.enqueueReadBuffer(buffer_Hasil, CL_TRUE, 0, sizeof(int) * UKURAN_DATA, data_Hasil);

    // LANGKAH 10: Tampilkan hasil di terminal untuk pembuktian
    cout << "\nHasil Penjumlahan di GPU:\n";
    for (int i = 0; i < UKURAN_DATA; i++) {
        cout << data_A[i] << " + " << data_B[i] << " = " << data_Hasil[i] << "\n";
    }

    return 0;
}
