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

#include<iostream>
#include<boost/bind.hpp>
#include<boost/thread/thread.hpp>
using std::cout;
using std::endl;
using std::cin;
// multiply matrice of SIZE by SIZE of type T
template<typename T, int SIZE = 10>
class Matrix
{
public:
  Matrix(int num)
  {
    init(A);
    init(B);
    //init(C); // initialize matrix C in multiply() instead
    num_thread = num;
  }


  void multiply(const int& slice)
  {
    // each thread works on its own separate slice only
    // so there is no need for synchronization among threads
    // note that this 'slicing' works fine
    // even if SIZE is not divisible by num_thread
    const int from = (slice * SIZE) / num_thread;
    const int to = ((slice + 1) * SIZE) / num_thread;

    for (int i = from; i < to; i++)
    {
      for (int j = 0; j < SIZE; j++)
      {
        C[i][j] = 0; // initialize matrix C
        for (int k = 0; k < SIZE; k++)
          C[i][j] += A[i][k] * B[k][j];
      }
    }
  }

  void output()
  {
    print(A);
    cout << endl << "   * " << endl;
    print(B);
    cout << endl << "   = " << endl;
    print(C);
    cout << endl;
  }
private:
  T A[SIZE][SIZE];  // matrix multiplication
  T B[SIZE][SIZE];  // C = A * B
  T C[SIZE][SIZE];
  int num_thread;   // number of threads

  void init(T M[SIZE][SIZE]) // initialize a matrix
  {
    int value = 0;
    for (int i = 0; i < SIZE; i++)
    {
      for (int j = 0; j < SIZE; j++)
        M[i][j] = value++;
    }
  }

  void print(const T M[SIZE][SIZE]) const // print out result
  {
    for (int i = 0; i < SIZE; i++)
    {
      std::cout << endl << "     |";
      for (int j = 0 ; j < SIZE; j++)
      {
        cout << M[i][j] << "  ";
      }
      cout << "|";
    }
  }
};

int main_multiplication() {
  cout << "There are " << boost::thread::hardware_concurrency()
       << " cores/processors on your computer." << endl;
  cout << "How many threads do you want to use?" << endl;

  int thrd_num;
  cout << "You are going to use total - 2 cores";
  thrd_num = boost::thread::hardware_concurrency() - 2;
  if (thrd_num <= 0) {
    thrd_num = 1;
  };

  // default matrix of 10 by 10 integers for multiplication
  // concurrently computed by thrd_mum of working threads
  Matrix<int> m(thrd_num);

  boost::thread_group threads;
  for (int i = 0; i < thrd_num; i++)
  {
    threads.create_thread(
      boost::bind(
        &Matrix<int>::multiply,
        boost::ref(m),
        i));
  }

  // main thread can do other things here concurrently. We just
  // simply let it wait for working threads to complete

  threads.join_all();

  m.output();
  return 0;
}