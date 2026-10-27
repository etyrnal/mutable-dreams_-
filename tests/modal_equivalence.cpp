/* Compile once normally and once with MD_REFERENCE_RESONATOR in resonator.cc.
 * Byte-compare their output across moving pickups, mode counts and block sizes. */
#include "rings/dsp/resonator.h"
#include <cstdio>
#include <cassert>
#include <cmath>
int main(int argc,char**argv){
 assert(argc==2);FILE*f=fopen(argv[1],"wb");assert(f);
 rings::Resonator r;r.Init();float in[97],out[97],aux[97];
 const unsigned sizes[]={1,7,31,64,97};unsigned rng=56789;
 for(int b=0;b<4096;b++){
  unsigned n=sizes[b%5];r.set_resolution((b%8+1)*8);
  r.set_frequency((40+b%800)/44100.f);r.set_structure((b%99)*.01f);
  r.set_brightness((b%75)*.01f);r.set_damping((b%90)*.01f);r.set_position((b%101)*.009f);
  for(unsigned k=0;k<n;k++){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;in[k]=((rng>>8)/8388608.f-1)*.1f;}
  r.Process(in,out,aux,n);
  for(unsigned k=0;k<n;k++){assert(std::isfinite(out[k])&&std::isfinite(aux[k]));fwrite(out+k,4,1,f);fwrite(aux+k,4,1,f);}
 }
 fclose(f);
}
