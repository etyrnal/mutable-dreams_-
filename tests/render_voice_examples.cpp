/* Reproducible dry, isolated voice phrases; both revisions read identical patches. */
#include "dsp/dreams.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cstring>
int main(int argc,char**argv){assert(argc==3);float lr[256];
 const int presets[15]={0,16,32,58,70,80,107,116,137,144,165,211,231,246,260};
 if(!strcmp(argv[1],"write")){
  for(int m=0;m<15;m++){
   md_init();md_command(MD_SCENE,0,MD_BLANK_BIOME);md_render(lr,1);
   md_command(MD_TONE_PRESET,0,presets[m]);md_render(lr,1);md_patch p=*md_state();
   p.biome_id=-1;p.scale=0;p.quality=0;p.fx_level=0;p.macro[1]=p.macro[3]=0;p.macro[2]=500;
   for(int i=0;i<6;i++){p.track[i].mute=i!=0;p.track[i].send=0;memset(&p.ext[i],0,sizeof p.ext[i]);}
   auto&t=p.track[0];t.level=700;t.pan=500;t.attack=m>=3?120:0;t.decay=700;
   assert(t.model==m&&md_validate(&p));char path[1024];snprintf(path,sizeof path,"%s/voice-%02d.patch",argv[2],m);
   FILE*f=fopen(path,"wb");assert(f);fwrite(&p,1,sizeof p,f);fclose(f);
  }return 0;
 }
 for(int m=0;m<15;m++){
  md_init();char path[1024];snprintf(path,sizeof path,"%s/voice-%02d.patch",argv[1],m);FILE*f=fopen(path,"rb");assert(f);
  md_patch p;assert(fread(&p,1,sizeof p,f)==sizeof p);fclose(f);assert(md_load(&p));
  snprintf(path,sizeof path,"%s/voice-%02d.f32",argv[2],m);f=fopen(path,"wb");assert(f);
  const int pitches[]={0,7,3,12},velocities[]={70,90,60,80};unsigned total=MD_RATE*12;
  for(unsigned at=0;at<total;){
   unsigned beat=at/MD_RATE,within=at%MD_RATE;
   int note=m==0?36:m==1?48:m==2?72:m==8?36:60;
   if(within==0&&beat%3==0){int k=beat/3;md_midi(0x90,note+(m<3?0:pitches[k]),velocities[k],0);}
   if(within==0&&beat%3==2){int k=beat/3;md_midi(0x80,note+(m<3?0:pitches[k]),0,0);}
   unsigned n=128;if(n>MD_RATE-within)n=MD_RATE-within;if(n>total-at)n=total-at;
   md_render(lr,n);for(unsigned k=0;k<2*n;k++)assert(std::isfinite(lr[k])&&fabs(lr[k])<1);
   fwrite(lr,sizeof(float),2*n,f);at+=n;
  }fclose(f);printf("Rendered %s\n",md_model_name(m));
 }
}
