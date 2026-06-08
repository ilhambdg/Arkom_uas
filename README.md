# Komparasi Performa Komputasi Paralel: OpenMP vs OpenCL

---

## Nama Anggota:
1. **Devain Rirung Samperuru**
   **NIM:** 25032014062
2. **Shibghoh Robbaani**
   **NIM:** 25032014045
   
* **Mata Kuliah:** Arsitektur dan Sistem Komputer
* **Universitas Negeri Surabaya (UNESA)**

---

## Deskripsi Proyek & Fitur

Proyek ini bertujuan untuk mengukur, menganalisis, dan membandingkan efisiensi waktu eksekusi komputasi paralel pada CPU dan GPU. Simulasi dilakukan dengan menghitung jarak Euclidean dari jutaan piksel gambar acak (RGB) menuju dua titik koordinat *centroid* target.

###  Fitur Utama:
* **Generasi Data Dinamis:** Alokasi memori gambar menggunakan *Heap Memory* (`malloc`) secara berkala untuk mencegah terjadinya *Stack Overflow* (Segmentation Fault) pada data masukan raksasa.
* **Pengujian Multiskala:** Mendukung simulasi otomatis untuk 5 variasi resolusi gambar standar:
  * 128 x 128 piksel (16.384 total piksel)
  * 256 x 256 piksel (65.536 total piksel)
  * 512 x 512 piksel (262.144 total piksel)
  * 1024 x 1024 piksel (1.048.576 total piksel)
  * 2048 x 2048 piksel (4.194.304 total piksel)
* **Akurasi Catatan Waktu:** Menggunakan fungsi *high-resolution timer* (`omp_get_wtime` dan `clock_gettime`) murni untuk mengukur waktu kalkulasi paralel, terisolasi dari waktu pembuatan data acak.
* **Warm-up Loop:** Mengeliminasi distorsi waktu *overhead initial thread* pada pengujian OpenMP pertama.

---

## Langkah-Langkah Menjalankan / Cara Simulasi

### Prasyarat Sistem (Prerequisites)
* Pastikan lingkungan Anda sudah terpasang compiler GCC dan library OpenCL. 
* Jika sistem Anda belum memasang driver khusus GPU proprietary, pastikan baris clGetDeviceIDs di dalam file tes_openCL.c   Anda disetel ke perangkat CL_DEVICE_TYPE_CPU agar berjalan di atas emulator runtime PoCL bawaan.

---

## Video Penjelasan Proyek

Penjelasan mengenai logika kode, kendala teknis, dan hasil analisis grafik perbandingan dapat diakses melalui tautan berikut:
https://youtube.com/@devainsamperuru?si=4C2PdUo1t-4rv9E0
