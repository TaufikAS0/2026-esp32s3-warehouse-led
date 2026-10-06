#pragma once
#include <Arduino.h>
const char kWebUi[] PROGMEM = R"HTML(<!doctype html>
<html lang="id"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Warehouse LED</title><style>
:root{color-scheme:dark;font:15px system-ui;background:#0b1220;color:#e2e8f0}body{max-width:1100px;margin:auto;padding:24px}h1{margin-bottom:4px}h2{font-size:20px}section{background:#152135;border:1px solid #30415d;border-radius:12px;padding:18px;margin:18px 0}input,select,button{font:inherit;padding:9px;border-radius:6px;border:1px solid #4c607e;background:#0b1220;color:inherit}button{cursor:pointer;background:#235fa2}button.danger{background:#963b40}label{display:inline-flex;flex-direction:column;gap:5px;margin:5px}small,.muted{color:#aab9cf}#message{white-space:pre-wrap;padding:12px;border-left:4px solid #4ca8ef}#box-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(195px,1fr));gap:10px;margin-top:15px}.box{border:1px solid #45556d;padding:12px;border-radius:8px}.box.active{border-color:#60dba4;background:#183c36}.box button{margin:8px 5px 0 0}.box strong{display:block}table{width:100%;border-collapse:collapse}td,th{text-align:left;padding:4px}td input{width:90%;min-width:55px;box-sizing:border-box}.scroll{overflow:auto}th{font-size:13px}#token{width:280px}#summary{line-height:1.8}.row{display:flex;align-items:end;flex-wrap:wrap;gap:8px}#config-state{color:#f5cb74}
</style></head><body>
<h1>Warehouse LED <small id="version"></small></h1><p class="muted">Penunjuk box • ESP32-S3 • 4 output / maksimum 300 LED</p>
<section><div class="row"><label>Token API dari USB<input id="token" type="password" autocomplete="off"></label><button id="connect">Hubungkan</button><button id="logout">Lupakan token</button></div><p class="muted">Token hanya disimpan selama sesi tab ini. Pertama kali: kirim <code>provision</code> melalui USB serial 115200 baud, lalu sambungkan ke AP Warehouse yang muncul.</p><div id="summary">Belum terhubung.</div></section>
<div id="message" role="status" aria-live="polite">Siap.</div>
<section><h2>Kontrol box</h2><div class="row"><label>Cari ID / nama<input id="search"></label><label>Warna<input id="color" type="color" value="#00ff00"></label><label>Brightness 0–255<input id="brightness" type="number" min="0" max="255" value="128"></label><label>Mode<select id="mode"><option value="steady">Nyala tetap</option><option value="blink">Berkedip</option></select></label><label>Durasi detik<input id="seconds" type="number" min="1" max="3600" value="60"></label><button id="all-off" class="danger">Padamkan semua</button></div><div id="box-grid"></div><p class="muted">Status menunjukkan perintah firmware, bukan deteksi fisik LED. Timer tetap berjalan ketika jaringan terputus.</p></section>
<section><h2>Konfigurasi mapping <small id="config-state"></small></h2><p>Nomor output dan LED mulai dari 1. Jumlah LED diisi sesuai yang terpasang; tidak terdeteksi otomatis.</p>
<div class="row" id="output-inputs"></div><label>Batas brightness global<input id="limit" type="number" min="0" max="255" value="128"></label>
<div class="row"><label>Jumlah rak generator<input id="rack-count" type="number" min="1" max="4" value="1"></label><label>Box per rak<input id="boxes-per-rack" type="number" min="1" max="25" value="1"></label><button id="generate">Ganti draft mapping (3 LED/box)</button><button id="add-box">Tambah box</button></div>
<p class="muted">Generator menggantikan draft. Perubahan baru aktif setelah Simpan; Simpan memadamkan semua box.</p>
<div class="scroll"><table><thead><tr><th>ID</th><th>Nama</th><th>Output 1–4</th><th>LED awal 1–75</th><th>Jumlah LED</th><th></th></tr></thead><tbody id="mapping"></tbody></table></div>
<h2>Wi-Fi</h2><label>SSID router<input id="ssid" maxlength="32" autocomplete="off"></label><label>Password baru<input id="wifi-pass" type="password" maxlength="63" autocomplete="new-password"></label><label><span><input id="replace-pass" type="checkbox"> Ganti password (kosong untuk jaringan terbuka)</span></label><p class="muted">Tanpa centang, password lama dipertahankan pada SSID yang sama. SSID kosong memakai AP saja. Mengubah Wi-Fi memerlukan koneksi ulang.</p>
<div class="row"><button id="save">Simpan konfigurasi</button><button id="reload">Muat ulang konfigurasi</button></div></section>
<section><h2>Pemeliharaan</h2><button id="ota-on">Aktifkan OTA 10 menit</button> <button id="ota-off">Matikan OTA</button><p class="muted">OTA hanya ketika tersambung ke router. Aktivasi memadamkan seluruh box. Password OTA dibaca melalui USB, tidak melalui web.</p></section>
<script>
'use strict';
const $=id=>document.getElementById(id);
let token=sessionStorage.getItem('warehouse-token')||'', config=null, latest=null, polling=false;
$('token').value=token;
function message(text){$('message').textContent=text;}
async function api(path,method='GET',data){
 const headers={'Authorization':'Bearer '+token};if(data!==undefined)headers['Content-Type']='application/json';
 const timeout=new AbortController(), timer=setTimeout(()=>timeout.abort(),5000);
 try{const res=await fetch('/api/v1'+path,{method,headers,body:data===undefined?undefined:JSON.stringify(data),signal:timeout.signal});
 const json=await res.json();if(!res.ok)throw new Error(res.status+' '+(json.error||'Request gagal'));return json;
 }finally{clearTimeout(timer);}
}
function integer(id,min,max){const n=Number($(id).value);if(!$(id).value||!Number.isInteger(n)||n<min||n>max)throw new Error(id+' di luar batas');return n;}
function input(value,type='text',min,max){const el=document.createElement('input');el.type=type;el.value=value;if(min!==undefined)el.min=min;if(max!==undefined)el.max=max;el.oninput=()=>{$('config-state').textContent='Draft belum disimpan';};return el;}
function mappingRow(b){
 const row=document.createElement('tr');
 for(const [key,type,min,max] of [['id','text'],['name','text'],['output','number',1,4],['start_led','number',1,75],['led_count','number',1,75]]){
 const td=document.createElement('td'),el=input(b[key],type,min,max);el.dataset.field=key;td.append(el);row.append(td);}
 const td=document.createElement('td'),remove=document.createElement('button');remove.textContent='Hapus';remove.onclick=()=>{row.remove();$('config-state').textContent='Draft belum disimpan';};td.append(remove);row.append(td);$('mapping').append(row);
}
function showConfig(){
 $('output-inputs').replaceChildren();config.outputs.forEach((count,i)=>{const label=document.createElement('label');label.append('Jumlah LED Rak '+(i+1));const el=input(count,'number',0,75);el.id='output-'+i;label.append(el);$('output-inputs').append(label);});
 $('limit').value=config.brightness_limit;$('ssid').value=config.wifi.ssid;$('wifi-pass').value='';$('replace-pass').checked=false;
 $('mapping').replaceChildren();config.boxes.forEach(mappingRow);$('config-state').textContent='Tersimpan';
}
function draft(){
 const boxes=Array.from($('mapping').children).map(row=>{const b={};row.querySelectorAll('input').forEach(el=>{b[el.dataset.field]=el.type==='number'?Number(el.value):el.value;});return b;});
 const result={schema:1,brightness_limit:integer('limit',0,255),outputs:[0,1,2,3].map(i=>integer('output-'+i,0,75)),boxes,wifi:{ssid:$('ssid').value}};
 if($('replace-pass').checked)result.wifi.password=$('wifi-pass').value;return result;
}
function cards(){
 if(!config||!latest)return;const term=$('search').value.toLowerCase();
 const existing=new Map(Array.from($('box-grid').children).map(card=>[card.dataset.boxId,card]));
 const visible=new Set();
 for(const box of config.boxes){if(!(box.id+' '+box.name).toLowerCase().includes(term))continue;
 visible.add(box.id);const state=latest.boxes.find(s=>s.id===box.id);let card=existing.get(box.id);
 if(!card){card=document.createElement('div');card.dataset.boxId=box.id;
 const id=document.createElement('strong'),name=document.createElement('div'),detail=document.createElement('small'),status=document.createElement('div');
 const on=document.createElement('button');on.textContent='Tes / nyalakan';on.onclick=()=>command(box.id,true);
 const off=document.createElement('button');off.textContent='Mati';off.onclick=()=>command(box.id,false);card.append(id,name,detail,status,on,off);$('box-grid').append(card);}
 card.className='box'+(state&&state.on?' active':'');
 card.children[0].textContent=box.id;card.children[1].textContent=box.name;
 card.children[2].textContent='Rak '+box.output+' • LED '+box.start_led+'–'+(box.start_led+box.led_count-1);
 card.children[3].textContent=state&&state.on?'Aktif: '+Math.ceil(state.remaining_ms/1000)+' dtk • RGB '+state.color.join(',')+' • '+state.mode:'Padam';}
 for(const [id,card] of existing)if(!visible.has(id))card.remove();
}
async function refresh(){latest=await api('/status');$('version').textContent=latest.version;
 $('summary').textContent='IP '+latest.ip+' | Router '+(latest.sta_connected?'terhubung':'terputus')+' | AP '+(latest.ap_active?latest.ap_ssid:'mati')+' | LED/output '+latest.outputs.join(', ')+' | Batas brightness '+latest.brightness_limit+' | OTA '+Math.ceil(latest.ota_remaining_ms/1000)+' dtk'+(latest.storage_error?' | Penyimpanan: '+latest.storage_error:'');cards();}
async function run(action){try{await action();}catch(e){message(e.name==='AbortError'?'Timeout: hasil perintah belum diketahui. Baca status sebelum mengirim ulang.':e.message);}}
async function command(id,on){await run(async()=>{const c=$('color').value;
 const payload=on?{on:true,color:[1,3,5].map(i=>parseInt(c.slice(i,i+2),16)),brightness:integer('brightness',0,255),mode:$('mode').value,duration_ms:integer('seconds',1,3600)*1000}:{on:false};
 const state=await api('/boxes/'+encodeURIComponent(id)+'/state','PUT',payload);message(id+': '+(state.on?'nyala':'padam')+'; brightness efektif '+state.brightness);await refresh();});}
$('connect').onclick=()=>run(async()=>{token=$('token').value.trim();if(!/^[a-f0-9]{64}$/.test(token))throw new Error('Token harus 64 karakter hex dari USB');sessionStorage.setItem('warehouse-token',token);config=await api('/config');showConfig();await refresh();message('Terhubung.');});
$('logout').onclick=()=>{sessionStorage.removeItem('warehouse-token');token='';$('token').value='';latest=null;config=null;$('box-grid').replaceChildren();$('mapping').replaceChildren();$('ssid').value='';$('wifi-pass').value='';$('summary').textContent='Token dilupakan.';message('Token dihapus dari sesi.');};
$('search').oninput=cards;
$('all-off').onclick=()=>run(async()=>{await api('/all-off','POST');await refresh();message('Semua box padam.');});
$('generate').onclick=()=>run(async()=>{if(!config)throw new Error('Hubungkan dahulu');const racks=integer('rack-count',1,4),boxes=integer('boxes-per-rack',1,25);
 $('mapping').replaceChildren();for(let r=1;r<=4;r++){$('output-'+(r-1)).value=r<=racks?boxes*3:0;if(r<=racks)for(let b=1;b<=boxes;b++)mappingRow({id:'R'+String(r).padStart(2,'0')+'-B'+String(b).padStart(2,'0'),name:'Rak '+r+' Box '+b,output:r,start_led:(b-1)*3+1,led_count:3});}$('config-state').textContent='Draft belum disimpan';message('Draft diganti dengan '+(racks*boxes)+' box. Belum diterapkan; tekan Simpan atau Muat ulang untuk membuang draft.');});
$('add-box').onclick=()=>{if(!config)return message('Hubungkan dahulu');if($('mapping').children.length>=100)return message('Maksimum 100 box');mappingRow({id:'',name:'',output:1,start_led:1,led_count:3});$('config-state').textContent='Draft belum disimpan';};
$('save').onclick=()=>run(async()=>{if(!config)throw new Error('Hubungkan dahulu');config=await api('/config','PUT',draft());showConfig();message('Konfigurasi tersimpan; seluruh LED padam. Jika Wi-Fi berubah, sambungkan ulang.');await refresh();});
$('reload').onclick=()=>run(async()=>{config=await api('/config');showConfig();message('Konfigurasi dimuat; draft sebelumnya dibuang.');});
for(const [id,enabled] of [['ota-on',true],['ota-off',false]])$(id).onclick=()=>run(async()=>{await api('/ota','POST',{enabled});await refresh();message(enabled?'OTA aktif maksimum 10 menit.':'OTA dimatikan.');});
setInterval(async()=>{if(!token||!config||polling||document.hidden)return;polling=true;try{await refresh();}catch(e){message('Status tidak tersedia: '+e.message);}finally{polling=false;}},1000);
for(const id of ['limit','ssid','wifi-pass','replace-pass'])$(id).addEventListener('input',()=>{$('config-state').textContent='Draft belum disimpan';});
</script></body></html>)HTML";
