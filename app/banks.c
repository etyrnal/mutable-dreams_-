/* SPDX-License-Identifier: MIT OR GPL-3.0-only
 * Explicit stopped writes; existing patch/panel/preferences IDs stay unchanged.
 * User Biomes: 0x98000..0xB7FFF; packed tones: 0xB8000..0xBFFFF.
 * Each object uses the platform CRC + committed A/B sectors. */
static int bank_slot,bank_kind,bank_char,bank_armed=-1;
static char bank_name[24]="MY BIOME";
typedef struct { uint32_t magic; char name[24]; md_patch patch; } UserBiome;
static UserBiome user_biome_io;
static uint32_t bank_arm_start;
typedef struct { char name[24]; md_track track; md_extension ext; } UserTone;
typedef struct { uint32_t magic,used; UserTone tone[16]; } ToneBank;
typedef char tone_bank_must_fit[(sizeof(ToneBank)<=PLAT_STORE_MAX)?1:-1];
static ToneBank tone_bank_io;
static md_patch bank_patch_io;
typedef char biome_must_fit[(sizeof(UserBiome)<=PLAT_STORE_MAX)?1:-1];
static int bank_read_biome(int obj){
 int n=plat_store_load(obj,&user_biome_io,sizeof user_biome_io);
 return n>=28&&user_biome_io.magic==0x4d445542&&md_decode(&bank_patch_io,&user_biome_io.patch,n-28);
}
static int bank_object(void){return bank_kind?OBJ_TONES+bank_slot/16:OBJ_USER+bank_slot;}
static int bank_read_tones(void){
 int n=plat_store_load(bank_object(),&tone_bank_io,sizeof tone_bank_io);
 if(n<0){memset(&tone_bank_io,0,sizeof tone_bank_io);tone_bank_io.magic=0x4d445442;return 0;}
 return n==sizeof tone_bank_io&&tone_bank_io.magic==0x4d445442&&!(tone_bank_io.used&0xffff0000u)?1:-1;
}
/* 0 success; 3 empty; 4 busy; 5 overwrite confirmation; 6 format; 7 write. */
static int bank_action(int save){
 if(dream_store_hold){app_say("USB BACKUP IN PROGRESS","");return 4;}
 if(md_playing()||md_clock_source()){app_say("STOP / USE INTERNAL CLOCK","");return 4;}
 if(bank_kind&&md_state()->kit.enabled&&dream_ui.track==3){app_say("SAVE KITS AS USER BIOMES","");return 6;}
 int obj=bank_object(),exists=plat_store_size(obj)>0;
 if(!save&&!bank_kind&&exists&&!bank_allow_load&&md_unsaved()){bank_pending=bank_slot;return 8;}
 if(bank_kind){int r=bank_read_tones();if(r<0){app_say("BANK FORMAT NOT SUPPORTED","");return 6;}exists=r&&(tone_bank_io.used&(1u<<(bank_slot%16)));}
 if(save==1&&exists&&(bank_armed!=obj*64+bank_slot||(uint32_t)(plat_ms()-bank_arm_start)>3000)){bank_armed=obj*64+bank_slot;bank_arm_start=plat_ms();app_say("SAVE AGAIN: OVERWRITE SLOT","");return 5;}
 bank_armed=-1;
 uint32_t lock=plat_lock();bank_patch_io=*md_state();plat_unlock(lock);
 if(save){
  int failed;
  if(bank_kind){memcpy(tone_bank_io.tone[bank_slot%16].name,bank_name,24);tone_bank_io.tone[bank_slot%16].track=bank_patch_io.track[dream_ui.track];tone_bank_io.tone[bank_slot%16].ext=bank_patch_io.ext[dream_ui.track];tone_bank_io.used|=1u<<(bank_slot%16);failed=plat_store_save(obj,&tone_bank_io,sizeof tone_bank_io);}
  else {user_biome_io.magic=0x4d445542;memcpy(user_biome_io.name,bank_name,24);user_biome_io.patch=bank_patch_io;failed=plat_store_save(obj,&user_biome_io,sizeof user_biome_io);}
  if(!failed&&!bank_kind){uint32_t f=plat_lock();md_mark_saved();plat_unlock(f);}
  app_say(failed?"BANK SAVE FAILED":"USER SLOT SAVED","");return failed?7:0;
 }else{
  if(!exists){app_say("EMPTY USER SLOT","");return 3;}
  if(bank_kind){UserTone *t=&tone_bank_io.tone[bank_slot%16];memcpy(bank_name,t->name,24);bank_name[23]=0;md_track *v=&bank_patch_io.track[dream_ui.track];
   /* Sound recall preserves rhythm, pitch, mix and sends. */
   v->model=t->track.model;v->tone=t->track.tone;v->harm=t->track.harm;v->decay=t->track.decay;v->attack=t->track.attack;v->shape=t->track.shape;v->cutoff=t->track.cutoff;v->resonance=t->track.resonance;v->filter=t->track.filter;v->breathe=t->track.breathe;
   md_extension *e=&bank_patch_io.ext[dream_ui.track];
   e->glide=t->ext.glide;e->tides=t->ext.tides;e->tides_rate=t->ext.tides_rate;e->warps=t->ext.warps;
   bank_patch_io.tone_preset[dream_ui.track]=-1;bank_patch_io.edited_mask|=1u|(2u<<dream_ui.track);
  }else {if(!bank_read_biome(obj)){app_say("INVALID USER SLOT","");return 6;}memcpy(bank_name,user_biome_io.name,24);bank_name[23]=0;}
  int ok=bank_kind?dream_install_edit(&bank_patch_io,sizeof bank_patch_io):dream_install(&bank_patch_io,sizeof bank_patch_io);app_say(ok?"USER SLOT LOADED":"INVALID USER SLOT","");return ok?0:6;
 }
}
