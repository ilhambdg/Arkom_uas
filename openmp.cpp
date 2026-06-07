// berjalan di CPU (untuk membagi tugas pada setiap thread)

#include <iostream>
#include <omp.h> 


int main() {
    // Mengatur jumlah thread secara manual (opsional)
    omp_set_num_threads(4);

    // Directive OpenMP untuk menjalankan blok kode secara paralel
    #pragma omp parallel
    {
        // Mengambil ID dari thread yang sedang berjalan
        int id = omp_get_thread_num();
        // Mengambil total thread yang tersedia
        int total = omp_get_num_threads();
        
        std::cout << "Halo dari Thread ke-" << id << " dari total " << total << " thread!\n";
    }

    return 0;
}


