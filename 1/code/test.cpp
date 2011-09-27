#include <fstream>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>

#include <benchmark.h>

void writeFile(const char * filename, int32_t maxNum);
std::streampos getSize(const char * filename);
double read(const char * filename, std::streampos size, int n_files, int blocksize);
double random_read(const char * filename, std::streampos size, int blocksize);

bool debug = false;

int main(int argc, char **argv)
{
    srand(time(0));
    
    std::string filename;
    std::string garbage;
    bool runGarbage = false;
    bool write = false;
    bool timing = true;
    
    int c;
    while ((c = getopt(argc, argv, "f:g:wtd")) != EOF)
        switch (c)
    {
        case 'f': filename = optarg; break;
        case 't': timing = true; break;
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
    
    std::vector<uint32_t> blocksizes;
    
    blocksizes.push_back(1024);
    blocksizes.push_back(4096);
    blocksizes.push_back(16384);
    
    
    std::vector<uint32_t>::const_iterator bs;
    
    
    
    for(bs=blocksizes.begin(); bs!=blocksizes.end(); bs++){
        for (int i = 1; i<=32; i = i<<1){
            // Clear buffers
            if (runGarbage){
                if(debug) printf("Reading garbage...\n");
                read(garbage.c_str(), sizeGarbage, 1, sizeof(uint32_t));
            }
            
            // Read file
            //printf("Reading with %d concurrent files open... bs: %d \n", i, *bs);
            double t = read(filename.c_str(), size, i, *bs);
            printf("%2d files(%d): %.6f sec\n", i, *bs, t);
        }
        double t = random_read(filename.c_str(), size, *bs);
        printf("random block(%d): %.6f sec\n", *bs, t);
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

double read(const char * filename, std::streampos size, int n_files, int blocksize)
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
    
    char * buffer = new char[blocksize];
    
    // Every slice is 250' / number of files big
    for (uint32_t i=0; i<(size/sizeof(uint32_t)/n_files); i+=blocksize)
    {
        // should be unrolled?
        for (int j = 0; j<n_files; ++j)
        {
            
            files[j].read(buffer, blocksize);
            //	printf("%4d", buffer);
        }
        //    printf("\n");
    }
    t.stop();
    
    // Close all files
    for (int i = 0; i<n_files; ++i)
        files[i].close();
    
    return t.getSecs();
}

double random_read(const char * filename, std::streampos size, int blocksize)
{
    std::ifstream file;
    
    // Open files, and set pos
    file.open(filename);
    std::streampos pos = std::rand()%size;
    
    file.seekg(pos, std::ios_base::beg);
    if (debug) printf("%lld \n", (unsigned long long) pos);
    
    // Read size/n_files bytes from all files
    Benchmark t; t.start();
    
    char * buffer = new char[blocksize];
    
    // Every slice is 250' / number of files big
    for (uint32_t i=0; i<(size/sizeof(uint32_t)); i+=blocksize)
    {
        
        file.read(buffer, blocksize);
        //    printf("\n");
    }
    t.stop();
    
    // Close all files
    file.close();
    
    return t.getSecs();
}


