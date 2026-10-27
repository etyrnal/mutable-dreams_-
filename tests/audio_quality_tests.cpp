/* Exercise the actual internal granular mixer with controlled tape contents.
 * This translation unit replaces dreams.cpp only in this regression executable. */
#ifndef MD_DSP_SOURCE
#define MD_DSP_SOURCE "../app/dsp/dreams.cpp"
#endif
#include MD_DSP_SOURCE
#include <cstdio>
#include <cassert>
#include <initializer_list>
// A neutral/low resonance setting must not produce an audible narrow peak.
static void check_filter_response() {
 for (float cutoff : {60.f, 1000.f, 8000.f, 18000.f}) {
  for (float resonance : {0.f, .1f, .2f, 1.f}) {
   stmlib::Svf f;f.Init();f.set_f_q<stmlib::FREQUENCY_ACCURATE>(cutoff/MD_RATE,filter_q(resonance));
   float impulse[8192];for(int i=0;i<8192;i++)impulse[i]=f.Process<stmlib::FILTER_MODE_LOW_PASS>(i==0?1.f:0.f);
   double peak=0;for(int k=0;k<=256;k++){double re=0,im=0,w=3.141592653589793*k/256;for(int i=0;i<8192;i++){re+=impulse[i]*__builtin_cos(w*i);im-=impulse[i]*__builtin_sin(w*i);}double a=__builtin_sqrt(re*re+im*im);if(a>peak)peak=a;}
   assert(isfinite(peak));if(resonance<=.2f)assert(peak<1.03);else assert(peak<3.0);
   printf("filter cutoff=%.0f resonance=%.2f peak_gain_db=%.4f\n",cutoff,resonance,20*__builtin_log10(peak));
  }
 }
}
static void check_clean_voice() {
 md_init();md_patch p=*md_state();p.scale=0;p.quality=1;p.macro[2]=500;p.macro[3]=0;
 for(int i=0;i<6;i++){md_track &t=p.track[i];t.model=MD_FM;t.mute=i!=0;t.filter=0;t.attack=0;t.decay=1000;t.level=1000;t.pan=500;t.tone=0;t.harm=234;t.shape=500;}
 assert(md_load(&p));md_command(MD_TRIGGER,0,69);float lr[256];for(int b=0;b<40;b++)md_render(lr,128);
 static float signal[44100];unsigned at=0;float peak=0;while(at<44100){unsigned n=44100-at;if(n>128)n=128;md_render(lr,n);for(unsigned k=0;k<n;k++){float x=.5f*(lr[2*k]+lr[2*k+1]);signal[at++]=x;peak=fmaxf(peak,fabsf(x));}}
 double fundamental=0,harmonics=0;for(int h=1;h<=12;h++){double re=0,im=0;for(int i=0;i<44100;i++){double w=6.283185307179586*i/44100;double x=signal[i]*(.5-.5*__builtin_cos(w));re+=x*__builtin_cos(w*h*440);im-=x*__builtin_sin(w*h*440);}double energy=re*re+im*im;if(h==1)fundamental=energy;else harmonics+=energy;}
 double thd=__builtin_sqrt(harmonics/fundamental);printf("Pure FM dry voice: harmonic distortion %.8f, peak %.6f\n",thd,peak);assert(peak<.3f&&peak>.1f);assert(thd<.005); // At least sixfold cleaner than the measured 3% clipped baseline.
}
static void check_articulation_and_diffusion(){
 md_init();md_patch p=*md_state();for(auto &t:p.track)t.mute=1;
 auto &t=p.track[0];t.mute=0;t.model=MD_FM;t.attack=0;t.decay=500;t.tone=200;t.shape=500;t.harm=234;t.level=600;p.fx_level=0;p.macro[3]=0;
 assert(md_load(&p));md_midi(0x90,60,100,0);float lr[256];md_render(lr,128);
 md_midi(0x80,60,0,0);for(int j=0;j<100;j++)md_render(lr,128);
 float before=voices[0].env;md_midi(0x90,60,25,0);md_render(lr,1);
 assert(voices[0].env-before<.013f&&voices[0].env<.1f);
 for(int j=0;j<80;j++)md_render(lr,128);
 float gain=voices[0].amp;md_midi(0x90,60,127,0);md_render(lr,1);
 assert(fabsf(voices[0].amp-gain)<.002f);
 puts("PASS melodic retrigger attack and velocity slew: no gain reset jump");
 reset_fx();double energy=0;int reflections=0;
 for(int i=0;i<MD_RATE;i++){float y=diffuse_room(i==0?.1f:0);assert(isfinite(y));energy+=y*y;if(fabsf(y)>1e-5)reflections++;}
 assert(__builtin_fabs(energy-.01)<1e-6&&reflections>30);
 reset_fx();for(int i=0;i<1000;i++)assert(diffuse_room(0)==0);
 puts("PASS room diffusion preserves impulse energy, spreads arrivals, clears to silence");
}
static double partial_energy(int model,int tone,int shape,int partial){
 md_init();md_patch p=*md_state();p.scale=0;p.fx_level=0;p.macro[2]=500;p.macro[3]=0;
 for(int i=0;i<6;i++)p.track[i].mute=i!=0;
 auto &t=p.track[0];t.model=model;t.tone=tone;t.shape=shape;t.harm=0;
 t.level=700;t.filter=0;t.breathe=0;t.attack=0;t.decay=1000;t.pan=500;
 p.ext[0].tides=p.ext[0].warps=p.ext[0].delay_send=p.ext[0].reverb_send=0;
 assert(md_load(&p));md_midi(0x90,69,100,0);float lr[256];
 for(int b=0;b<120;b++)md_render(lr,128);
 double re=0,im=0;unsigned at=0;
 while(at<MD_RATE){unsigned n=MD_RATE-at;if(n>128)n=128;md_render(lr,n);
  for(unsigned k=0;k<n;k++,at++){
   double w=6.283185307179586*at/MD_RATE;
   double x=(lr[k*2]+lr[k*2+1])*(.5-.5*__builtin_cos(w));
   re+=x*__builtin_cos(w*440*partial);im-=x*__builtin_sin(w*440*partial);
  }
 }
 return re*re+im*im;
}
static void check_voice_colour(){
 // The old bass ignored Shape, and Choir's tone changed only sine levels.
 // These controls must now produce a genuine harmonic change, not a gain boost.
 double bass_low=partial_energy(MD_BASS,600,0,2)/partial_energy(MD_BASS,600,0,1);
 double bass_high=partial_energy(MD_BASS,600,1000,2)/partial_energy(MD_BASS,600,1000,1);
 assert(bass_high>3*bass_low&&bass_high<.5);
 double choir_low=partial_energy(MD_CHOIR,100,0,2)/partial_energy(MD_CHOIR,100,0,1);
 double choir_high=partial_energy(MD_CHOIR,800,0,2)/partial_energy(MD_CHOIR,800,0,1);
 assert(choir_high>20*choir_low&&choir_high<.2);
 printf("PASS source colour: bass Shape harmonic ratio %.5f -> %.5f; choir Tone %.5f -> %.5f\n",bass_low,bass_high,choir_low,choir_high);
}
static void check_live_colour_edits(){
 // Compare the actual first 64 output samples after a large Tone/Shape edit
 // against the exact same continuing note. This detects a spectral hard step,
 // rather than merely checking the value of an internal smoother.
 for(int model:{MD_CHOIR,MD_BASS,MD_MELLOW,MD_VELVET_KEYS,MD_BREATH}){
  float worst=0;
  for(int offset=0;offset<8;offset++){
   float sample[2][128];
   for(int edit=0;edit<2;edit++){
    md_init();md_patch p=*md_state();p.scale=0;p.macro[2]=500;p.macro[3]=p.fx_level=0;
    for(int i=0;i<6;i++){p.track[i].mute=i!=0;p.ext[i].tides=p.ext[i].warps=p.ext[i].delay_send=p.ext[i].reverb_send=0;}
    auto&t=p.track[0];t.model=model;t.tone=100;t.shape=0;t.harm=0;t.breathe=0;t.filter=0;t.level=700;t.pan=500;t.attack=0;t.decay=1000;
    assert(md_load(&p));md_midi(0x90,60,100,0);float tmp[256];for(int b=0;b<180+offset;b++)md_render(tmp,128);
    if(edit){md_command(MD_TONE,0,900);md_command(MD_SHAPE,0,1000);}
    md_render(sample[edit],64);
   }
   for(int k=0;k<128;k++)worst=fmaxf(worst,fabsf(sample[1][k]-sample[0][k]));
  }
  printf("live colour edit model=%d first-tile difference %.8f\n",model,worst);
  assert(worst<.004f);
 }
 puts("PASS large live Tone/Shape changes enter smoothly on all five original direct voices");
}
static double resonator_attack(unsigned seed,int note,int tone,int velocity){
 md_init();md_patch p=*md_state();p.scale=0;p.quality=1;p.macro[2]=500;p.macro[3]=p.fx_level=p.duck_amount=0;
 for(int i=0;i<6;i++){p.track[i].mute=i!=0;p.ext[i].tides=p.ext[i].warps=p.ext[i].delay_send=p.ext[i].reverb_send=0;}
 auto &t=p.track[0];t.model=MD_RINGS;t.tone=tone;t.harm=350;t.decay=600;t.level=700;t.filter=0;t.breathe=0;t.pan=500;
 assert(md_load(&p));rng=seed;md_midi(0x90,note,velocity,0);float lr[128];double energy=0;
 for(int b=0;b<414;b++){md_render(lr,64);for(float x:lr){assert(isfinite(x));energy+=x*x;}}
 return __builtin_sqrt(energy/(414*128));
}
static void check_resonator_balance(){
 // Equal touch and pitch should not become a surprise accent merely because
 // the noise generator reached a different point. Keep genuine touch dynamics.
 double worst=1;
 for(int note:{48,60,72})for(int tone:{200,500,800}){
  double lo=1,hi=0;
  for(unsigned seed=1;seed<=12;seed++){double rms=resonator_attack(seed*7919,note,tone,100);if(rms<lo)lo=rms;if(rms>hi)hi=rms;}
  if(hi/lo>worst)worst=hi/lo;assert(lo>.0001);assert(hi/lo<1.4);
 }
 double soft=resonator_attack(7919,60,500,40),loud=resonator_attack(7919,60,500,100);
 assert(loud/soft>2.4&&loud/soft<2.6);
 printf("PASS Rings equal-touch energy ratio <= %.5f across 108 strikes; velocity ratio %.5f\n",worst,loud/soft);
}
static void check_sequence_timing(){
 // A note created at this render boundary must obey its gate immediately,
 // including gates shorter than one DSP tile. Compare with single-frame calls.
 float env[2];
 for(int partition=0;partition<2;partition++){
  md_init();md_patch p=*md_state();p.tempo=220;p.swing=0;p.scale=0;
  for(int i=0;i<6;i++)p.track[i].mute=i!=0;
  p.track[0].model=MD_FM;p.track[0].attack=0;p.track[0].speed=1;p.track[0].prob=1000;
  p.sequence[0].enabled=1;p.sequence[0].length=2;p.sequence[0].edit=0;
  p.sequence[0].steps[0]={60,90,100,1};p.sequence[0].steps[1]={0,90,0,1};
  assert(md_load(&p));md_command(MD_PLAY,0,1);float lr[128];
  if(partition)for(int n=0;n<64;n++)md_render(lr,1);else md_render(lr,64);
  env[partition]=voices[0].env;assert(voices[0].midi_gate<0);
 }
 printf("Short-gate envelope: tile %.8f, single-frame %.8f\n",env[0],env[1]);
 assert(fabsf(env[0]-env[1])<.000001f);
 // Six different short gates must not subdivide every oscillator/filter
 // render. Only their own envelopes need sample-accurate release.
 md_init();md_patch p=*md_state();p.tempo=220;p.swing=0;
 for(int i=0;i<6;i++){
  p.track[i].model=MD_HARMONIC;p.track[i].mute=0;p.track[i].speed=1;p.track[i].prob=1000;
  p.sequence[i].enabled=1;p.sequence[i].length=1;p.sequence[i].edit=0;
  p.sequence[i].steps[0]={60,90,100,(uint8_t)(i+1)};
 }
 assert(md_load(&p));md_command(MD_PLAY,0,1);float lr[512];md_render(lr,256);
 assert(md_status()->voice_quanta==6*4);
 for(int i=0;i<6;i++)assert(voices[i].midi_gate<0);
 puts("PASS six staggered short gates: four rack tiles, sample-accurate envelope releases");
 // A slow track spans alternating swing ticks. Two ticks retain their total
 // duration; an odd number gets just one long/short swing offset.
 for(int speed:{1,2,3,4,5,6,7,8})for(int phase:{0,1}){
  md_init();patch.tempo=120;patch.swing=600;
  for(auto &t:patch.track)t.mute=1;
  auto&t=patch.track[0];t.mute=0;t.model=MD_FM;t.speed=speed;t.prob=1000;
  auto&q=patch.sequence[0];q.enabled=1;q.length=1;q.steps[0]={60,90,100,50};
  step=phase*speed;tick();
  float expected=MD_RATE*60.f/(120*4)*(speed+(speed%2?(step-1)%2?-.3f:.3f:0))*.5f;
  assert(fabsf(seq_gate[0]-expected)<.01f);
 }
 puts("PASS short gates independent of render partition; swing gate spans slow-track intervals");
}
static void check_long_recording(){
 // 2^32/100 + 1 samples makes the former integer product wrap to 4.
 // Simulate elapsed audio frames rather than waiting sixteen minutes.
 for(unsigned start:{100u,0xfffffff0u}){
  md_init();record_mode=2;record_track=0;record_note=60;record_step=0;
  record_start=start;stats.frames=start+42949673u;capture_off(0,60);
  assert(patch.sequence[0].steps[0].gate==100);
  assert(record_note==-1&&record_step==-1);
 }
 puts("PASS long held recording saturates at full gate across frame-counter wrap");
}
int main(int argc,char**){
 setvbuf(stdout,nullptr,_IONBF,0);
 check_sequence_timing();
 check_long_recording();
 check_resonator_balance();
 check_articulation_and_diffusion();
 check_voice_colour();
 check_live_colour_edits();
 check_filter_response();
 check_clean_voice();
 bool enforce=argc>1;unsigned total_restarts=0;float worst=0;
 for(int quality:{0,1})for(int size=0;size<=1000;size+=10){
  md_init();for(int i=0;i<6;i++)patch.track[i].mute=1;patch.freeze=1;patch.quality=quality;patch.grain_size=size;patch.grain_pos=500;patch.macro[3]=1000;smooth[3]=1;
  for(unsigned i=0;i<GN;i++)tape[i]=3277;tape_valid=GN;
  unsigned restarts=0;float jump=0,prev[2]={};
  for(unsigned s=0;s<MD_RATE*3;s++){
   Grain old[4];memcpy(old,grains,sizeof old);float lr[2];quantum(lr,1);
   for(int j=0;j<4;j++)if(old[j].life&&old[j].phase<.999f&&grains[j].phase<old[j].phase)restarts++;
   if(s>MD_RATE)for(int c=0;c<2;c++)jump=fmaxf(jump,fabsf(lr[c]-prev[c]));
   memcpy(prev,lr,sizeof prev);assert(patch.quality==quality);assert(isfinite(lr[0])&&isfinite(lr[1]));
  }
  printf("constant tape quality=%d size=%d interrupted_grains=%u max_sample_step=%.9f\n",quality,size,restarts,jump);
  total_restarts+=restarts;worst=fmaxf(worst,jump);
 }
 printf("SUMMARY interrupted_grains=%u worst_step=%.9f\n",total_restarts,worst);
 if(enforce){assert(total_restarts==0);assert(worst<.002f);}
 if(enforce){
  // Live size/quality changes must never recycle an unfinished window.
  md_init();for(int i=0;i<6;i++)patch.track[i].mute=1;patch.freeze=1;tape_valid=GN;smooth[3]=1;patch.macro[3]=1000;
  for(unsigned i=0;i<GN;i++)tape[i]=3277;
  unsigned interrupted=0;
  for(unsigned s=0;s<MD_RATE*6;s++){
   if(s%2205==0){patch.grain_size=(s/2205)%2?1000:10;patch.quality=(s/4410)%2;patch.grain_pos=(s/2205)%2?0:1000;patch.grain_pitch=(s/2205)%2?24:-24;patch.reverse=(s/8820)%2;}
   Grain old[4];memcpy(old,grains,sizeof old);float lr[2];quantum(lr,1);
   for(int j=0;j<4;j++){
    if(old[j].life&&old[j].phase<.999f&&grains[j].phase<old[j].phase)interrupted++;
    if(grains[j].life&&grains[j].age<grains[j].life){float d=writehead-grains[j].pos;if(d<0)d+=GN;assert(d>3&&d<GN-3);}
   }
   assert(isfinite(lr[0])&&isfinite(lr[1]));
  }
  assert(interrupted==0);puts("PASS live size / quality / position / pitch / reverse changes: no interrupted grains or tape seam crossings");
  assert(md_cutoff_hz(1000)>17990&&md_cutoff_hz(0)==60);
 }
 // A limiter should be transparent at ordinary signal levels.
 double err=0,signal=0;
 for(int i=0;i<44100;i++){float x=.5f*sinf(i*6.28318530718f*997/44100),y=clip(x);err+=(y-x)*(y-x);signal+=x*x;}
 printf("limiter relative error at peak 0.5: %.9f\n",sqrtf((float)(err/signal)));
 if(enforce)assert(err==0);
 return 0;
}
