#include <fstream>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>

#include <benchmark.h>

void writeFile(const char * filename, int32_t maxNum);
std::streampos getSize(const char * filename);
double read(const char * filename, std::streampos size, int n_files);

bool debug = false;

int main(int argc, char **argv)
{
  std::string filename;
  std::string garbage;
  bool runGarbage = false;
  bool write = false;

  int c;
  while ((c = getopt(argc, argv, "f:g:wd")) != EOF)
    switch (c)
    {
    case 'f': filename = optarg; break;
    case 'g': garbage = optarg; runGarbage=true; break;
    case 'w': write = true; break;
    case 'd': debug = true; break;
    case '?': return 1;
    }

  if (filename.empty()){
    fprintf(stderr, "-f <filename> is not set\n");
    return 1;
  }

  // Remember: mod(size, parallel files) must be zero
  if (write) {
    writeFile(filename.c_str(), 250000000);
  }
  std::streampos size = getSize(filename.c_str());
  if (size < 0) {
    fprintf(stderr, "File do not exists: %s\n", filename.c_str());
    return 1;
  }

  if (garbage.empty())
    fprintf(stderr, "-g <filename> is not set - Not running garbage file\n");

  std::streampos sizeGarbage;
  if (runGarbage){
    sizeGarbage = getSize(garbage.c_str());
    if (sizeGarbage < 0) {
      fprintf(stderr, "File do not exists: %s\n", garbage.c_str());
      return 1;
    }
  }
  
  for (int i = 1; i<=32; i = i<<1){
    // Clear buffers
    if (runGarbage){
      printf("Reading garbage...\n");
      read(garbage.c_str(), sizeGarbage, 1);
    }

    // Read file
    printf("Reading with %d concurrent files open...\n", i);
    double t = read(filename.c_str(), size, i);
    printf("%2d files: %.3f sec\n", i, t);
  }
}

void writeFile(const char * filename, int32_t maxNum)
{
  std::ofstream myFile(filename, std::ofstream::binary);
  if (debug) printf("Writing file...\r");
  // Write data to file
  for (int32_t i=0; i<maxNum; ++i) {
    myFile.write(reinterpret_cast<char*>( &i ), sizeof(int32_t));
  }
  myFile.close();
  if (debug) printf("Writing file... Finished\n");
}

std::streampos getSize(const char * filename)
{
  std::ifstream myFile(filename, std::ifstream::binary);
  if (myFile) {
    myFile.seekg(0, std::ios_base::end);
    std::streampos size = myFile.tellg();
    myFile.close();
    return size;
  }
  else
    return -1;
}

double read(const char * filename, std::streampos size, int n_files)
{
  if (debug) printf("\n%d files: ", n_files);
  std::ifstream files[n_files];

  // Open files, and set pos
  for (int i = 0; i<n_files; ++i)
  {
    files[i].open(filename);
    std::streampos pos = (size/n_files)*i;
    files[i].seekg(pos, std::ios_base::beg);
    if (debug) printf("%lld, ", (unsigned long long) pos);
  }
  if (debug) printf("\n");

  // Read size/n_files bytes from all files
  Benchmark t; t.start();

  // Every slice is 250' / number of files big
  for (int i=0; i<(size/sizeof(uint32_t)/n_files); ++i)
  {
    // should be unrolled?
    for (int j = 0; j<n_files; ++j)
      {
	uint32_t val;
	files[j].read((char*) &val, sizeof(uint32_t));
	//	printf("%4d", val);
      }
    //    printf("\n");
  }
  t.stop();

  // Close all files
  for (int i = 0; i<n_files; ++i)
    files[i].close();

  return t.getSecs();
}


