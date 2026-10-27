#include "dsp/dreams.h"
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <cmath>
int main(int argc,char**argv){assert(argc>=4);md_init();int selected=atoi(argv[2]);float lr[256];
 if(argv[1][0]=='b'){md_command(MD_SCENE,0,selected);md_render(lr,1);md_command(MD_PLAY,0,1);}
 else {FILE*f=fopen(argv[1],"rb");assert(f);unsigned char data[2100];unsigned n=fread(data,1,sizeof data,f);fclose(f);assert(md_import(data,n));md_midi(0x90,selected,100,0);}
 FILE*out=fopen(argv[3],"wb");assert(out);unsigned left=(argc>4?atoi(argv[4]):8)*MD_RATE;while(left){unsigned n=left>128?128:left;md_render(lr,n);for(unsigned k=0;k<n*2;k++)assert(std::isfinite(lr[k]));fwrite(lr,sizeof(float),n*2,out);left-=n;}fclose(out);
}
