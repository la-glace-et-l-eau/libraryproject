#pragma once

#include <string>
#include "miniz.h"
#include "fmt/core.h"
/**
 * @brief Return the name of this header-only library
 */
inline auto name() -> std::string
{
  return "libraryproject";
}

void printk() {
  printf("######## k !\n");
}

#if defined(__GNUC__)
// Ensure we get the 64-bit variants of the CRT's file I/O calls
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif
#endif

#include <stdio.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

// The string to compress.
static const char *s_pTest_str =
  "MISSION CONTROL I wouldn't worry too much about the computer. First of all, there is still a chance that he is right, despite your tests, and" \
  "if it should happen again, we suggest eliminating this possibility by allowing the unit to remain in place and seeing whether or not it" \
  "actually fails. If the computer should turn out to be wrong, the situation is still not alarming. The type of obsessional error he may be" \
  "guilty of is not unknown among the latest generation of HAL 9000 computers. It has almost always revolved around a single detail, such as" \
  "the one you have described, and it has never interfered with the integrity or reliability of the computer's performance in other areas." \
  "No one is certain of the cause of this kind of malfunctioning. It may be over-programming, but it could also be any number of reasons. In any" \
  "event, it is somewhat analogous to human neurotic behavior. Does this answer your query?  Zero-five-three-Zero, MC, transmission concluded.";

static const char *s_pComment = "This is a comment";

int main_function(int argc, char *argv[])
{
  int i, sort_iter;
  mz_bool status;
  size_t uncomp_size;
  mz_zip_archive zip_archive;
  void *p;
  const int N = 50;
  char data[2048];
  char archive_filename[64];
  static const char *s_Test_archive_filename = "__mz_example2_test__.zip";

  assert((strlen(s_pTest_str) + 64) < sizeof(data));

  printf("miniz.c version: %s\n", MZ_VERSION);

  (void)argc, (void)argv;

  // Delete the test archive, so it doesn't keep growing as we run this test
  remove(s_Test_archive_filename);

  // Append a bunch of text files to the test archive
  for (i = (N - 1); i >= 0; --i)
  {
    sprintf_s(archive_filename, "%u.txt", i);
    sprintf_s(data, "%u %s %u", (N - 1) - i, s_pTest_str, i);

    // Add a new file to the archive. Note this is an IN-PLACE operation, so if it fails your archive is probably hosed (its central directory may not be complete) but it should be recoverable using zip -F or -FF. So use caution with this guy.
    // A more robust way to add a file to an archive would be to read it into memory, perform the operation, then write a new archive out to a temp file and then delete/rename the files.
    // Or, write a new archive to disk to a temp file, then delete/rename the files. For this test this API is fine.
    status = mz_zip_add_mem_to_archive_file_in_place(s_Test_archive_filename, archive_filename, data, strlen(data) + 1, s_pComment, static_cast<uint16>(strlen(s_pComment)), MZ_BEST_COMPRESSION);
    if (!status)
    {
      printf("mz_zip_add_mem_to_archive_file_in_place failed!\n");
      return EXIT_FAILURE;
    }
  }

  // Add a directory entry for testing
  status = mz_zip_add_mem_to_archive_file_in_place(s_Test_archive_filename, "directory/", NULL, 0, "no comment", static_cast<uint16>(strlen("no comment")), MZ_BEST_COMPRESSION);
  if (!status)
  {
    printf("mz_zip_add_mem_to_archive_file_in_place failed!\n");
    return EXIT_FAILURE;
  }

  // Now try to open the archive.
  memset(&zip_archive, 0, sizeof(zip_archive));

  status = mz_zip_reader_init_file(&zip_archive, s_Test_archive_filename, 0);
  if (!status)
  {
    printf("mz_zip_reader_init_file() failed!\n");
    return EXIT_FAILURE;
  }

  // Get and print information about each file in the archive.
  for (uint j = 0; j < static_cast<uint>(mz_zip_reader_get_num_files(&zip_archive)); j++)
  {
    mz_zip_archive_file_stat file_stat;
    if (!mz_zip_reader_file_stat(&zip_archive, j, &file_stat))
    {
      printf("mz_zip_reader_file_stat() failed!\n");
      mz_zip_reader_end(&zip_archive);
      return EXIT_FAILURE;
    }

    printf("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %u, Compressed size: %u, Is Dir: %u\n", file_stat.m_filename, file_stat.m_comment, static_cast<uint>(file_stat.m_uncomp_size), static_cast<uint>(file_stat.m_comp_size), mz_zip_reader_is_file_a_directory(&zip_archive, j));

    if (!strcmp(file_stat.m_filename, "directory/"))
    {
      if (!mz_zip_reader_is_file_a_directory(&zip_archive, j))
      {
        printf("mz_zip_reader_is_file_a_directory() didn't return the expected results!\n");
        mz_zip_reader_end(&zip_archive);
        return EXIT_FAILURE;
      }
    }
  }

  // Close the archive, freeing any resources it was using
  mz_zip_reader_end(&zip_archive);

  // Now verify the compressed data
  for (sort_iter = 0; sort_iter < 2; sort_iter++)
  {
    memset(&zip_archive, 0, sizeof(zip_archive));
    status = mz_zip_reader_init_file(&zip_archive, s_Test_archive_filename, sort_iter ? MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY : 0);
    if (!status)
    {
      printf("mz_zip_reader_init_file() failed!\n");
      return EXIT_FAILURE;
    }

    for (i = 0; i < N; i++)
    {
      sprintf_s(archive_filename, "%u.txt", i);
      sprintf_s(data, "%u %s %u", (N - 1) - i, s_pTest_str, i);

      // Try to extract all the files to the heap.
      p = mz_zip_reader_extract_file_to_heap(&zip_archive, archive_filename, &uncomp_size, 0);
      if (!p)
      {
        printf("mz_zip_reader_extract_file_to_heap() failed!\n");
        mz_zip_reader_end(&zip_archive);
        return EXIT_FAILURE;
      }

      // Make sure the extraction really succeeded.
      if ((uncomp_size != (strlen(data) + 1)) || (memcmp(p, data, strlen(data))))
      {
        printf("mz_zip_reader_extract_file_to_heap() failed to extract the proper data\n");
        mz_free(p);
        mz_zip_reader_end(&zip_archive);
        return EXIT_FAILURE;
      }

      printf("Successfully extracted file \"%s\", size %u\n", archive_filename, static_cast<uint>(uncomp_size));
      printf("File data: \"%s\"\n", static_cast < const char *>(p));

      // We're done.
      mz_free(p);
    }

    // Close the archive, freeing any resources it was using
    mz_zip_reader_end(&zip_archive);
  }

  printf("Success.\n");
  return EXIT_SUCCESS;
}

#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <functional>

static const long MATRIX_SIZE = 30;
static const int THREADS_NUMBER = 4;
static const long N_EXECUTIONS = 1e3;

struct Matrix {
  double ** elements;

  void initialize_zero() {
    elements = new double*[MATRIX_SIZE];
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      elements[i] = new double[MATRIX_SIZE];
      for (int j = 0; j < MATRIX_SIZE; ++j) {
        elements[i][j] = static_cast<double>(0.0f);
      }
    }
  }

  void initialize_random() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-1e9, -1e9);
    auto random = std::bind(dist, mt);
    elements = new double*[MATRIX_SIZE];
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      elements[i] = new double[MATRIX_SIZE];
      for (int j = 0; j < MATRIX_SIZE; ++j) {
        elements[i][j] = random();
      }
    }
  }

  void print() {
    std::cout << std::endl;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      std::cout << "|\t";

      for (int j = 0; j < MATRIX_SIZE; ++j) {
        std::cout << elements[i][j] << "\t";
      }
      std::cout << "|" << std::endl;
    }
  }

};

void multiply(Matrix& r, const Matrix& m1, const Matrix& m2);
void single_execution(Matrix& r, const Matrix& m1, const Matrix& m2);
void multithreading_execution(Matrix& r, const Matrix& m1, const Matrix& m2);
void multiply_threading(Matrix& result, const int thread_number, const Matrix& m1, const Matrix& m2);
void run_execution(void(*execution_function)(Matrix& r, const Matrix& m1, const Matrix& m2));

int matrix_multiply() {
  std::cout << "Single execution" << std::endl;
  run_execution(single_execution);
  std::cout << "Multi thread execution" << std::endl;
  run_execution(multithreading_execution);
  std::cout << "End of program" << std::endl;
  return 0;
}

void run_execution(void(*execution_function)(Matrix& r, const Matrix& m1, const Matrix& m2)) {
  Matrix m1, m2, r;

  for (int i = 0; i < N_EXECUTIONS; ++i) {
    m1.initialize_random();
    m2.initialize_random();
    r.initialize_zero();

    execution_function(r, m1, m2);
  }
}

void multiply(Matrix& r, const Matrix& m1, const Matrix& m2) {
  for (int i = 0; i < MATRIX_SIZE; ++i) {
    for (int j = 0; j < MATRIX_SIZE; ++j) {
      double result = static_cast<double>(0.0f);
      for (int k = 0; k < MATRIX_SIZE; ++k) {
        const double e1 = m1.elements[i][k];
        const double e2 = m2.elements[k][j];
        result += e1 * e2;
      }
      r.elements[i][j] = result;
    }
  }
}

void single_execution(Matrix& r, const Matrix& m1, const Matrix& m2) {
  //std::cout << "Starting single thread execution..." << std::endl;

  //std::cout << "Calculating...." << std::endl;
  multiply(r, m1, m2);

  //std::cout << "Finishing single thread execution..." << std::endl;

}

void multiply_threading(Matrix& result, const int thread_number, const Matrix& m1, const Matrix& m2) {
  // Calculate workload
  const int n_elements = (MATRIX_SIZE * MATRIX_SIZE);
  const int n_operations = n_elements / THREADS_NUMBER;
  const int rest_operations = n_elements % THREADS_NUMBER;

  int start_op, end_op;

  if (thread_number == 0) {
    // First thread does more job
    start_op = n_operations * thread_number;
    end_op = (n_operations * (thread_number + 1)) + rest_operations;
  }
  else {
    start_op = n_operations * thread_number + rest_operations;
    end_op = (n_operations * (thread_number + 1)) + rest_operations;
  }

  for (int op = start_op; op < end_op; ++op) {
    const int row = op % MATRIX_SIZE;
    const int col = op / MATRIX_SIZE;
    double r = static_cast<double>(0.0f);
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      const double e1 = m1.elements[row][i];
      const double e2 = m2.elements[i][col];
      r += e1 * e2;
    }

    result.elements[row][col] = r;
  }
}

void multithreading_execution(Matrix& r, const Matrix& m1, const Matrix& m2) {
  //std::cout << "Starting multithreading execution..." << std::endl;

  std::thread threads[THREADS_NUMBER];

  for (int i = 0; i < THREADS_NUMBER; ++i) {
    //std::cout << "Starting thread " << i << std::endl;
    threads[i] = std::thread(multiply_threading, std::ref(r), i, std::ref(m1), std::ref(m2));
  }

  //std::cout << "Calculating...." << std::endl;

  for (int i = 0; i < THREADS_NUMBER; ++i) {
    //std::cout << "Joining thread " << i << std::endl;
    threads[i].join();
  }

  //std::cout << "Finishing multithreading execution..." << std::endl;
}
