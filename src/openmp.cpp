// deklarasi header yang akan digunakan
#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <cmath>
#include <omp.h>

// metode untuk menghitung 
void hitung_jarak_fork_join(
    const std::vector<unsigned char>& pixel,
    const std::array<std::array<int,3>,2>& centroid,
    std::vector<float>& jarak,
    int jumlah_pixel
) {
    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for(int i = 0; i < jumlah_pixel; i++)
        {
            for(int c = 0; c < 2; c++)
            {
                float r =
                    static_cast<float>(pixel[i * 3 + 0]) -
                    centroid[c][0];

                float g =
                    static_cast<float>(pixel[i * 3 + 1]) -
                    centroid[c][1];

                float b =
                    static_cast<float>(pixel[i * 3 + 2]) -
                    centroid[c][2];

                jarak[i * 2 + c] =
                    std::sqrt(r*r + g*g + b*b);
            }
        }
    }
}

int main()
{
    std::vector<int> ukuran_gambar =
    {
        128,
        256,
        512,
        1024,
        2048
    };

    std::vector<int> jumlah_pixel =
    {
        128*128,
        256*256,
        512*512,
        1024*1024,
        2048*2048
    };

    constexpr int jumlah_gambar = 5;

    std::array<
        std::array<
            std::array<int,3>,2>,
        jumlah_gambar> centroids;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0,255);

    std::cout
        << "Jumlah Thread OpenMP : "
        << omp_get_max_threads()
        << "\n\n";

    for(int i = 0; i < jumlah_gambar; i++)
    {
        std::cout
            << "Gambar ke-" << i+1
            << " | Ukuran "
            << ukuran_gambar[i]
            << "x"
            << ukuran_gambar[i]
            << " | Total Pixel "
            << jumlah_pixel[i]
            << '\n';
    }

    std::cout
        << "-------------------------------------------------\n";

    for(int i = 0; i < jumlah_gambar; i++)
    {
        for(int c = 0; c < 2; c++)
        {
            centroids[i][c][0] = dis(gen);
            centroids[i][c][1] = dis(gen);
            centroids[i][c][2] = dis(gen);
        }
    }

    std::cout
        << "\n=== EKSEKUSI FORK-JOIN OPENMP ===\n\n";

    for(int i = 0; i < jumlah_gambar; i++)
    {
        int n_pixel = jumlah_pixel[i];

        std::vector<unsigned char> data_pixel(
            n_pixel * 3
        );

        std::vector<float> jarak(
            n_pixel * 2
        );

        for(auto &p : data_pixel)
        {
            p = static_cast<unsigned char>(
                dis(gen)
            );
        }

        double start =
            omp_get_wtime();

        hitung_jarak_fork_join(
            data_pixel,
            centroids[i],
            jarak,
            n_pixel
        );

        double end =
            omp_get_wtime();

        std::cout
            << "Ukuran "
            << ukuran_gambar[i]
            << "x"
            << ukuran_gambar[i]
            << '\n';

        std::cout
            << "Jumlah Thread : "
            << omp_get_max_threads()
            << '\n';

        std::cout
            << "Waktu Eksekusi : "
            << (end - start)
            << " detik\n\n";
    }

    return 0;
}