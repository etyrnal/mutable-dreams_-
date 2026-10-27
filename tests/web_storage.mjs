// Exercise the shipped worklet and real WASM, without audio playback or a DOM.
import fs from 'node:fs';
import vm from 'node:vm';
import assert from 'node:assert/strict';
const root=new URL('../',import.meta.url);let Processor;
globalThis.currentTime=0;
globalThis.AudioWorkletProcessor=class{constructor(){this.messages=[];this.port={postMessage:m=>this.messages.push(m)}}};
globalThis.registerProcessor=(name,p)=>Processor=p;
vm.runInThisContext(fs.readFileSync(new URL('web/app_worklet.js',root),'utf8'));
vm.runInThisContext(fs.readFileSync(new URL('web/worklet.js',root),'utf8').replace('import "./app_worklet.js";',''));
const wasm=fs.readFileSync(new URL('app/build/wasm/dreams.wasm',root));
const load=async store=>{const f=new Processor();await f.onMessage({type:'load',wasm,store,master:0});return f};
const f=await load({}),out=[[new Float32Array(128),new Float32Array(128)]];
const render=()=>{globalThis.currentTime+=128/44100;f.process([],out)};
for(let n=0;n<1200;n++)render();
f.publish();f.messages=[];
f.ex.dream_command(112,0,0);f.ex.dream_command(111,0,15);f.ex.dream_command(114,0,0);
render();render();f.publish();
assert.equal(f.messages.filter(m=>m.store).length,1,'one persistence event per actual write');
const saved=f.messages.find(m=>m.store).store;
assert(saved[18]?.length>0,'user Biome saved');
f.messages=[];for(let n=0;n<100;n++){render();f.publish()}
assert.equal(f.messages.filter(m=>m.store).length,0,'idle frame publication must not repeat persistence');
const bad={...saved,'-1':[91,92,93], '4294967296':[94,95], '1.5':[96], '01':[97], '99999':[98], '1':[256], '2':Array(f.ex.web_store_max()+1).fill(0)};
const g=await load(bad);
assert.equal(g.messages.find(m=>m.type==='ready').storageRejected,7);
assert.deepEqual([...new Uint8Array(g.mem.buffer,0,4)],[0,0,0,0],'invalid IDs never write at address zero');
assert.equal(g.ex.web_store_len(1),-1);assert.equal(g.ex.web_store_len(2),-1);
assert.deepEqual([...new Uint8Array(g.mem.buffer,g.ex.web_store(18),g.ex.web_store_len(18))],[...saved[18]],'valid neighbouring bank preserved');
for(const value of [null,[],42,'bad']){
 const h=await load(value);assert.equal(h.messages.find(m=>m.type==='ready').storageRejected,1);
}
// localStorage's JSON representation of typed arrays is an indexed object.
const json=JSON.parse(JSON.stringify(saved));const h=await load(json);
assert.equal(h.messages.find(m=>m.type==='ready').storageRejected,0);
assert.deepEqual([...new Uint8Array(h.mem.buffer,h.ex.web_store(18),h.ex.web_store_len(18))],[...saved[18]]);
console.log('PASS single persistence event, invalid IDs/bytes/sizes/top-level data rejected, valid saved Biome survives restart; no audio playback');
