#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <benchmark.h>

Benchmark::Benchmark() : 
  _active(false),
  _wStart(0.0), _wElapsed(0.0), 
  _pStart(0.0), _pElapsed(0.0)
{
  // Empty
}

/**
 * Set start time to now
 * @returns True if not already started
 */
bool Benchmark::start(){
  if (_active) return false;
  _active = true;

  _wStart = wtime();
  _pStart = ptime();
  return true;
}

/**
 * Stops timer
 * @returns True if not already stopped
 */
bool Benchmark::stop(){
  if (!_active) return false;
  _active = false;

  _wElapsed = wtime() - _wStart;
  _pElapsed = ptime() - _pStart;
  return true;
}

double Benchmark::getProcessMicros(){
  if (_active) _pElapsed = ptime() - _pStart;
  return _pElapsed;
}

/*
 * @returns Elapsed time in micros
 */
double Benchmark::getMicros(){
  if (_active)
    _wElapsed = wtime() - _wStart;
  return _wElapsed;
}

/*
 * @returns Elapsed time in millis
 */
double Benchmark::getMillis(){
  return this->getMicros() / 1e3;
}

/*
 * @returns Elapsed time in secs
 */
double Benchmark::getSecs(){
  return this->getMicros() / 1e6;
}

/*
 * @returns Currents wall time
 */
double Benchmark::wtime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec*1e6 + tv.tv_usec;
}

double Benchmark::ptime() {
  struct rusage ru;
  getrusage(RUSAGE_SELF, &ru);

  return ru.ru_utime.tv_sec*1000000 + (ru.ru_utime.tv_usec);
}
