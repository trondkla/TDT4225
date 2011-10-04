#ifndef _BENCHMARK
#define _BENCHMARK

class Benchmark
{
 private:
  bool _active;

  // Wall time
  double _wStart;
  double _wElapsed;
  double wtime();

  // Process time
  double _pStart;
  double _pElapsed;
  double ptime();

 public:
  Benchmark();
  bool start();
  bool stop();

  // Wall time
  double getMicros();
  double getMillis();
  double getSecs();

  // Process time
  double getProcessMicros();
  
};

#endif
