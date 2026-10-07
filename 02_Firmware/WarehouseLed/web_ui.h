#pragma once
#include <Arduino.h>
#include "firmware_version.h"
const char kWebUi[] PROGMEM = R"HTML(<!doctype html>
<html lang="id"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Warehouse LED Matrix</title><style>
:root{color-scheme:dark;--bg:#0b1326;--panel:#131b2e;--line:#2d3950;--cyan:#4cd7f6;--text:#dae2fd;--muted:#9cacc6;font:14px system-ui,sans-serif;background:var(--bg);color:var(--text)}*{box-sizing:border-box}body{margin:0}button,input,select{font:inherit}button{cursor:pointer;min-height:42px;border:1px solid var(--line);border-radius:7px;padding:9px 14px;background:#222a3d;color:var(--text)}button:hover{border-color:var(--cyan)}button:disabled{opacity:.4;cursor:not-allowed}button.primary{background:var(--cyan);color:#003640;border-color:var(--cyan);font-weight:750}button.danger{background:#67242c;border-color:#9e3944;color:#ffdad6}input,select{min-height:42px;max-width:100%;border:1px solid var(--line);border-radius:6px;background:#080f20;color:var(--text);padding:9px}input[type=checkbox]{min-height:auto}input[type=color]{width:64px;padding:4px}input[type=range]{padding:0;accent-color:var(--cyan)}label{display:flex;flex-direction:column;gap:7px;font-size:12px;color:var(--muted)}h1,h2,h3,p{margin-top:0}h1{font-size:23px;letter-spacing:-.5px;margin-bottom:5px}h2{font-size:18px}small,.muted{color:var(--muted);line-height:1.6}.eyebrow{font:11px ui-monospace,monospace;letter-spacing:1.5px;color:var(--cyan);text-transform:uppercase}.top{display:flex;align-items:center;gap:24px;padding:22px 30px;border-bottom:1px solid var(--line);background:#080f20;flex-wrap:wrap}.top nav{display:flex;gap:6px;margin-left:auto}.tab.active{background:#18394a;color:var(--cyan);border-color:var(--cyan)}.badge{display:inline-block;border-radius:5px;border:1px solid var(--line);padding:5px 9px;font:11px ui-monospace,monospace;color:#4edea3}.badge.stale{color:#ffc777;border-color:#8f6740}main{padding:24px 30px;max-width:1800px;margin:auto}.panel{background:var(--panel);border:1px solid var(--line);border-radius:12px;padding:20px}.auth{margin-bottom:16px}.auth summary{cursor:pointer;color:var(--muted)}.auth[open] summary{margin-bottom:14px}.row{display:flex;gap:12px;align-items:end;flex-wrap:wrap}.row label{flex:1;min-width:140px}.row button{flex-shrink:0}.stats{display:grid;grid-template-columns:repeat(4,1fr);gap:12px;margin:18px 0}.stat{padding:16px 20px;background:var(--panel);border-radius:10px;border:1px solid var(--line)}.stat span{display:block;font:11px ui-monospace,monospace;color:var(--muted);margin-bottom:8px}.stat strong{font-size:25px;color:var(--cyan)}.toolbar{display:grid;grid-template-columns:minmax(200px,1fr) 160px 180px;gap:12px;margin-bottom:14px}.workspace{display:grid;grid-template-columns:minmax(0,1fr) 330px;gap:20px;align-items:start}#box-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:20px}.rack{padding:18px;min-width:0}.rack h2{display:flex;align-items:center;gap:8px;flex-wrap:wrap;color:var(--cyan);margin-bottom:14px}.rack h2 small{font:11px ui-monospace,monospace;color:var(--muted)}.rack-grid{display:grid;grid-template-columns:repeat(5,minmax(0,1fr));gap:7px}.box{min-width:0;min-height:91px;background:#192236;border:1px solid transparent;border-top:3px solid #39445a;padding:9px 7px;display:flex;flex-direction:column;gap:7px;align-items:start;text-align:left;border-radius:5px}.box strong{font-size:13px}.box .box-name{font-size:11px;width:100%;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;color:#b5c4dc}.box small{font:10px ui-monospace,monospace;color:var(--muted)}.box.active{border-top-color:var(--led);background:#203348}.box.selected{outline:2px solid var(--cyan);outline-offset:1px}.box.placeholder{opacity:.42;border-top-style:dashed}.box .state-label{font-size:9px;color:var(--muted)}.box.active .state-label{color:var(--led)}.box[hidden],.rack[hidden],[hidden]{display:none!important}.control{position:sticky;top:20px}.control h2{padding:14px;background:#080f20;margin:-2px -2px 16px}.detail-line{display:flex;justify-content:space-between;gap:10px;margin-bottom:10px;font-size:12px}.detail-line span{color:var(--muted)}.detail-line strong{text-align:right;overflow-wrap:anywhere}.led-preview{display:flex;gap:8px;padding:16px;background:#080f20;border-radius:8px;margin:18px 0;flex-wrap:wrap}.diode{width:30px;height:30px;border:1px solid var(--line);border-radius:5px;display:grid;place-items:center;font:10px ui-monospace,monospace;color:#9cacc6}.control label{margin:16px 0 8px}.control .row label{margin:0}.swatches{display:flex;gap:7px;flex-wrap:wrap}.swatches button{width:32px;min-height:32px;padding:0;background:var(--swatch);border:2px solid #55617b}.control-actions{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-top:18px}.control-actions .primary{grid-column:1/-1}.test-buttons{display:grid;grid-template-columns:repeat(3,1fr);gap:6px;margin:10px 0}.test-buttons button{font-size:12px;padding:8px 3px}.note{font-size:11px;color:var(--muted);line-height:1.7}.divider{border-top:1px solid var(--line);margin:20px 0}.settings-grid{display:grid;grid-template-columns:1fr 1fr;gap:20px;margin-top:20px}.settings-grid .wide{grid-column:1/-1}.scroll{overflow:auto;max-height:520px}table{width:100%;border-collapse:collapse;font-size:12px}th{text-align:left;padding:12px 8px;color:var(--muted);position:sticky;top:0;background:#131b2e}td{padding:6px;border-top:1px solid var(--line)}td input{width:100%;min-width:85px}td:first-child input{min-width:125px}#config-state{color:#ffc777}#message{padding:12px 16px;border:1px solid var(--line);border-left:3px solid var(--cyan);border-radius:6px;margin:12px 0;color:#bceffa;white-space:pre-wrap}#summary{margin-top:10px;font-size:12px;color:var(--muted)}.settings-actions{display:flex;justify-content:space-between;align-items:center;gap:14px;flex-wrap:wrap}.control input[type=number]{width:95px}footer{margin-top:24px;padding-top:16px;border-top:1px solid var(--line);font-size:11px;color:var(--muted)}:focus-visible{outline:2px solid var(--cyan);outline-offset:3px}
@media(min-width:1600px){.box{min-height:105px}.box .box-name{font-size:12px}}
@media(max-width:1150px){.workspace{grid-template-columns:minmax(0,1fr) 300px}#box-grid{grid-template-columns:1fr}.top{padding:18px}.stats{gap:8px}}
@media(max-width:760px){main{padding:16px}.top{gap:12px}.top nav{margin-left:0}.top h1{font-size:20px}.stats{grid-template-columns:1fr 1fr}.stat{padding:12px}.stat strong{font-size:21px}.toolbar{grid-template-columns:1fr 1fr}.toolbar input{grid-column:1/-1}.workspace{grid-template-columns:1fr}.control{position:static}.settings-grid{grid-template-columns:1fr}.rack{padding:12px}.rack-grid{gap:5px}.box{min-height:87px;padding:7px 4px}.settings-actions button{width:100%}}
.workspace{display:block}.control{position:relative;top:auto}.modal{position:fixed;inset:0;z-index:30;background:#000a;display:grid;place-items:center;padding:20px}.modal .control{width:min(850px,100%);max-height:90vh;overflow:auto;box-shadow:0 24px 90px #0008}.modal-head{display:flex;justify-content:space-between;align-items:center;position:sticky;top:-20px;background:var(--panel);padding:12px 0;z-index:1}.modal-body{display:grid;grid-template-columns:1fr 1fr;gap:24px}.box-color{font:10px ui-monospace,monospace;display:flex;gap:6px;align-items:center}.box-color:before{content:'';width:10px;height:10px;border-radius:50%;background:var(--saved-color);flex-shrink:0}.box{border-top-color:var(--saved-color,#39445a)}.program-panel{margin:18px 0}.program-panel .row{margin-top:12px}#popup-message{color:#bceffa;font-size:12px;white-space:pre-wrap;margin:12px 0}body.modal-open{overflow:hidden}@media(max-width:760px){.modal{padding:8px}.modal-body{grid-template-columns:1fr}.modal .control{max-height:95vh}.modal-head{top:-20px}#box-grid{grid-template-columns:1fr}}@media(min-width:761px){#box-grid{grid-template-columns:repeat(2,minmax(0,1fr))}}
</style></head><body>
<header class="top"><div><div class="eyebrow">ESP32-S3 • Warehouse controller</div><h1>WAREHOUSE LED MATRIX</h1><small>Lokasi jelas. Kontrol langsung.</small></div><span id="connection" class="badge stale">BELUM TERHUBUNG</span><span id="version" class="badge">Firmware )HTML" WAREHOUSE_FIRMWARE_VERSION R"HTML(</span><nav aria-label="Halaman"><button id="nav-map" class="tab active">Peta rak &amp; box</button><button id="nav-config" class="tab">Konfigurasi</button></nav><button id="all-off" class="danger">Padamkan semua</button></header>
<main><section class="panel auth"><div class="row"><div style="flex:1"><strong>Akses LAN terbuka</strong><div id="summary" class="note">Memuat mapping controller…</div></div><button id="connect">Muat ulang koneksi</button></div></section>
<div id="message" role="status" aria-live="polite">Mapping dimuat otomatis. Pilih box untuk konfigurasi atau tes LED.</div>
<div class="stats"><div class="stat"><span>BOX AKTIF</span><strong id="active-count">—</strong></div><div class="stat"><span>BOX TERKONFIGURASI / 100</span><strong id="configured-count">—</strong></div><div class="stat"><span>LED TERPASANG / 300</span><strong id="led-count">—</strong></div><div class="stat"><span>BATAS BRIGHTNESS</span><strong id="global-limit">—</strong></div></div>
<section id="map-page"><section class="panel program-panel"><div class="eyebrow">Program otomatis / hardware test</div><h2>Gerak & warna antar-box</h2><div class="row"><label>Pola animasi<select id="program-pattern"><option value="running">Running box • warna konfigurasi</option><option value="blink">Kedip bergilir</option><option value="rainbow">Pelangi mengalir</option><option value="breathe">Napas warna</option></select></label><label>Brightness program<input id="program-brightness" type="number" min="0" max="255" value="32"></label><label>Waktu langkah (ms)<input id="program-step" type="number" min="100" max="5000" value="500"></label><label>Durasi tes (detik)<input id="program-seconds" type="number" min="1" max="3600" value="30"></label></div><div class="row"><button id="program-test" class="primary">Mulai tes otomatis</button><button id="program-idle">Jalankan sebagai idle</button><button id="program-stop">Hentikan program</button><strong id="program-state">Program padam</strong></div><p class="note">Mengikuti mapping box tersimpan. Running/kedip memakai warna konfigurasi tiap box. Perintah manual/JARVIS mendapat prioritas pada box tersebut. Idle terus berjalan sampai dihentikan; reboot, Simpan konfigurasi, Padamkan semua dan aktivasi OTA menghentikannya. Tidak mengubah flash saat berjalan.</p></section><div class="toolbar"><input id="search" aria-label="Cari ID atau nama box" placeholder="Cari ID box atau nama…"><select id="rack-filter" aria-label="Filter rak"><option value="0">Semua rak</option><option value="1">Rak 01</option><option value="2">Rak 02</option><option value="3">Rak 03</option><option value="4">Rak 04</option></select><select id="state-filter" aria-label="Filter status"><option value="all">Semua status</option><option value="on">Aktif</option><option value="off">Padam</option><option value="unconfigured">Belum dikonfigurasi</option></select></div>
<p class="note">Kapasitas 4 rak × 25 box. Klik setiap box untuk konfigurasi dan tes LED. Posisi abu-abu belum dikonfigurasi. Grid 5×5 adalah urutan box; cocokkan susunannya dengan rak fisik.</p>
<div class="workspace"><div id="box-grid"></div></div><div id="box-modal" class="modal" hidden><aside id="control" class="panel control" role="dialog" aria-modal="true" aria-labelledby="selected-id"><div class="modal-head"><div class="eyebrow">Konfigurasi & kontrol box</div><button id="close-box" aria-label="Tutup popup box">Tutup ✕</button></div><div id="popup-message" role="status"></div><div class="modal-body"><div><h2 id="selected-id">Pilih box</h2><div class="detail-line"><span>Nama</span><strong id="selected-name">—</strong></div><div class="detail-line"><span>Lokasi</span><strong id="selected-location">—</strong></div><div class="detail-line"><span>Rentang LED</span><strong id="selected-leds">—</strong></div><div class="detail-line"><span>Status firmware</span><strong id="selected-state">—</strong></div><div id="led-preview" class="led-preview" aria-label="Ilustrasi LED"></div>
<section id="box-editor" hidden><div class="divider"></div><div class="eyebrow">Konfigurasi box terpilih</div><label>ID box<input id="box-id" maxlength="24"></label><label>Nama box<input id="box-name"></label><div class="row"><label>Output / Rak<select id="box-output"><option value="1">Rak 01 / GPIO 4</option><option value="2">Rak 02 / GPIO 5</option><option value="3">Rak 03 / GPIO 6</option><option value="4">Rak 04 / GPIO 7</option></select></label></div><div class="row"><label>LED awal<input id="box-start" type="number" min="1" max="75"></label><label>Jumlah LED box<input id="box-count" type="number" min="1" max="75"></label></div><label>Warna konfigurasi box<input id="box-color" type="color" value="#00ff00"></label><p id="box-capacity" class="note"></p><button id="go-output-settings">Buka setting jumlah LED rak</button><p class="note">Mapping mengikuti jumlah LED rak yang sudah disimpan. Simpan box menyimpan warna konfigurasi dan mapping; seluruh LED dipadamkan.</p><button id="save-box" class="primary">Simpan box</button><p id="box-save-state" class="note">Simpan mapping sebelum tes LED.</p></section></div><div><div class="eyebrow">Kontrol & tes LED</div><label>Warna LED</label><div class="swatches"><button data-color="#4cd7f6" style="--swatch:#4cd7f6" aria-label="Cyan"></button><button data-color="#00ff00" style="--swatch:#00ff00" aria-label="Hijau"></button><button data-color="#ff0000" style="--swatch:#ff0000" aria-label="Merah"></button><button data-color="#0000ff" style="--swatch:#0000ff" aria-label="Biru"></button><button data-color="#b395ff" style="--swatch:#b395ff" aria-label="Ungu"></button><button data-color="#ffffff" style="--swatch:#ffffff" aria-label="Putih"></button></div><div class="row" style="margin-top:10px"><input id="color" type="color" value="#00ff00" aria-label="Warna custom"><small id="color-value">#00FF00</small></div>
<label for="brightness-range">Brightness <span id="brightness-value">50%</span></label><div class="row"><input id="brightness-range" type="range" min="0" max="255" value="128" style="flex:1"><input id="brightness" aria-label="Brightness 0–255" type="number" min="0" max="255" value="128"></div><p class="note">Nilai efektif dibatasi batas global. Pengaturan tidak langsung menyalakan LED.</p>
<div class="row"><label>Mode<select id="mode"><option value="steady">Nyala tetap</option><option value="blink">Kedip 1 detik</option></select></label><label>Durasi detik<input id="seconds" type="number" min="1" max="3600" value="60"></label></div><div class="row" style="margin-top:10px"><button data-seconds="10">10s</button><button data-seconds="60">60s</button><button data-seconds="300">5m</button></div>
<div class="control-actions"><button id="turn-on" class="primary" data-control>Nyalakan LED box</button><button id="turn-off" data-control>Padamkan box</button><button id="test-blink" data-control>Tes kedip 3 detik</button></div><div class="divider"></div><div class="eyebrow">Tes warna cepat • 3 detik</div><div class="test-buttons"><button id="test-red" data-control>Merah</button><button id="test-green" data-control>Hijau</button><button id="test-blue" data-control>Biru</button></div><p class="note">Tes mengganti perintah pada box terpilih, memakai brightness yang dipilih dan timer 3 detik. Warna pada layar adalah ilustrasi; firmware tidak mendeteksi nyala LED fisik.</p></div></div></aside></div></section>
<section id="config-page" hidden><div class="panel settings-actions"><div><h2 style="margin-bottom:6px">Konfigurasi mapping <small id="config-state"></small></h2><small>Simpan memadamkan semua box. Draft tidak mengubah LED sampai disimpan.</small></div><div class="row"><button id="reload">Buang draft / muat ulang</button><button id="save" class="primary">Simpan konfigurasi</button></div></div><div class="settings-grid">
<section class="panel"><div class="eyebrow">01 / Output hardware</div><h2>Jumlah LED terpasang</h2><div class="row" id="output-inputs"></div><label style="margin-top:15px">Batas brightness global 0–255<input id="limit" type="number" min="0" max="255" value="128"></label><p class="note">Output 1–4 memakai GPIO 4,5,6,7. Isi 0 untuk output tidak terpasang. Jumlah LED tidak dideteksi otomatis. Setting ini terpisah dari mapping box; mengurangi jumlah LED yang masih dipakai box akan ditolak.</p><button id="save-outputs" class="primary">Simpan setting rak</button></section>
<section class="panel"><div class="eyebrow">02 / Setup cepat</div><h2>Generator box</h2><div class="row"><label>Jumlah rak<input id="rack-count" type="number" min="1" max="4" value="1"></label><label>Box per rak<input id="boxes-per-rack" type="number" min="1" max="25" value="1"></label><label>LED per box<input id="leds-per-box" type="number" min="1" max="75" value="3"></label></div><div class="row" style="margin-top:14px"><button id="generate" class="primary">Buat draft mapping</button><button id="setup-three">Setup 1 box / 3 LED</button><button id="setup-individual">Tes tiap LED: 3 box / 1 LED</button></div><p class="note">Untuk pengujian awal: Setup 1 box / 3 LED → Simpan → Peta rak → pilih B01 → Tes warna. Generator mengganti seluruh draft, maksimal 75 LED per output.</p></section>
<section class="panel wide"><div class="settings-actions"><div><div class="eyebrow">03 / Pemetaan lokasi</div><h2>Editor box <small id="mapping-count"></small></h2></div><button id="add-box">Tambah box</button></div><div class="scroll"><table><thead><tr><th>ID box tetap</th><th>Nama / label</th><th>Output 1–4</th><th>LED awal 1–75</th><th>Jumlah LED</th><th>Warna konfigurasi</th><th>Aksi</th></tr></thead><tbody id="mapping"></tbody></table></div><p class="note">ID harus unik. Rentang LED tidak boleh tumpang tindih atau melewati jumlah terpasang. Angka LED mulai dari 1.</p></section>
<section class="panel"><div class="eyebrow">04 / Jaringan</div><h2>Wi-Fi router</h2><div class="row"><label>SSID router<input id="ssid" maxlength="32" autocomplete="off"></label><label>Password baru<input id="wifi-pass" type="password" maxlength="63" autocomplete="new-password"></label></div><label style="margin-top:14px"><span><input id="replace-pass" type="checkbox"> Ganti password router</span></label><p class="note">SSID kosong memakai AP saja. Tanpa centang, password lama dipertahankan pada SSID yang sama. SSID baru memerlukan password baru; kosong untuk router terbuka. Mengubah jaringan memerlukan koneksi ulang.</p></section>
<section class="panel"><div class="eyebrow">05 / Pemeliharaan</div><h2>OTA</h2><div class="row"><button id="ota-on">Aktifkan 10 menit</button><button id="ota-off">Matikan OTA</button></div><p class="note">Hanya tersedia saat controller terhubung ke router. Aktivasi memadamkan semua LED. Password OTA dibaca lewat USB.</p></section></div></section>
<footer>WS2812B • 4 output • Maksimum 300 LED • Seluruh box padam saat reboot. Timer lokal tetap berjalan ketika Wi-Fi terputus.</footer></main><script>

'use strict';
const $=id=>document.getElementById(id);
let config=null, latest=null, polling=false, selected=null, online=false, layoutKey='', busy=false, selectedSlot=null;
sessionStorage.removeItem('warehouse-token');
let loading=false;
function message(text){$('message').textContent=text;$('popup-message').textContent=text;}
async function api(path,method='GET',data){
 const headers={};if(data!==undefined)headers['Content-Type']='application/json';
 const timeout=new AbortController(), timer=setTimeout(()=>timeout.abort(),5000);
 try{const res=await fetch('/api/v1'+path,{method,headers,body:data===undefined?undefined:JSON.stringify(data),signal:timeout.signal});
 const json=await res.json();if(!res.ok)throw new Error(res.status+' '+(json.error||'Request gagal'));return json;
 }finally{clearTimeout(timer);}
}
function integer(id,min,max){const n=Number($(id).value);if(!$(id).value||!Number.isInteger(n)||n<min||n>max)throw new Error(id+' di luar batas');return n;}
function input(value,type='text',min,max){const el=document.createElement('input');el.type=type;el.value=value;if(min!==undefined)el.min=min;if(max!==undefined)el.max=max;el.oninput=()=>{$('config-state').textContent='Draft belum disimpan';};return el;}
function mappingRow(b){
 const row=document.createElement('tr');
 for(const [key,type,min,max] of [['id','text'],['name','text'],['output','number',1,4],['start_led','number',1,75],['led_count','number',1,75],['color','color']]){
 const td=document.createElement('td'),el=input(key==='color'?hex(b.color||[0,255,0]):b[key],type,min,max);el.dataset.field=key;td.append(el);row.append(td);}
 const td=document.createElement('td'),remove=document.createElement('button');remove.textContent='Hapus';remove.onclick=()=>{row.remove();$('mapping-count').textContent=$('mapping').children.length+' box';$('config-state').textContent='Draft belum disimpan';};td.append(remove);row.append(td);$('mapping').append(row);$('mapping-count').textContent=$('mapping').children.length+' box';
}
function showConfig(){
 $('output-inputs').replaceChildren();config.outputs.forEach((count,i)=>{const label=document.createElement('label');label.append('Jumlah LED Rak '+(i+1));const el=input(count,'number',0,75);el.id='output-'+i;label.append(el);$('output-inputs').append(label);});
 $('limit').value=config.brightness_limit;$('ssid').value=config.wifi.ssid;$('wifi-pass').value='';$('replace-pass').checked=false;
 $('mapping').replaceChildren();config.boxes.forEach(mappingRow);$('config-state').textContent='Tersimpan';
}
function draft(){
 const boxes=Array.from($('mapping').children).map(row=>{const b={};row.querySelectorAll('input').forEach(el=>{b[el.dataset.field]=el.type==='number'?Number(el.value):el.type==='color'?channels(el.value):el.value;});return b;});
 const result={schema:1,brightness_limit:integer('limit',0,255),outputs:[0,1,2,3].map(i=>integer('output-'+i,0,75)),boxes,wifi:{ssid:$('ssid').value}};
 if($('replace-pass').checked)result.wifi.password=$('wifi-pass').value;
 return validateMapping(result);
}
function validateMapping(result){
 const boxes=result.boxes;
 if(!boxes.length||boxes.length>100)throw new Error('Mapping memerlukan 1–100 box');
 const ids=new Set(),occupied=new Set();
 for(const b of boxes){
  if(!/^[A-Za-z0-9_-]{1,24}$/.test(b.id))throw new Error('ID box harus 1–24 karakter huruf, angka, _ atau -');
  if(ids.has(b.id))throw new Error('duplicate box ID: '+b.id);ids.add(b.id);
  if(new Blob([b.name]).size>48)throw new Error(b.id+': nama maksimal 48 byte');
  if(!Number.isInteger(b.output)||b.output<1||b.output>4||!Number.isInteger(b.start_led)||b.start_led<1||!Number.isInteger(b.led_count)||b.led_count<1||b.led_count>75||b.start_led+b.led_count-1>result.outputs[b.output-1])throw new Error(b.id+': rentang melebihi LED terpasang');
  for(let led=b.start_led;led<b.start_led+b.led_count;led++){const key=b.output+':'+led;if(occupied.has(key))throw new Error('overlapping box mapping: '+b.id);occupied.add(key);}
 }
 return result;
}
function selectBox(id,slot=null){
 if(!config){message('Mapping belum dimuat. Tekan Muat ulang koneksi dan tunggu status LIVE.');return;}
 selected=id;selectedSlot=slot;
 const box=config?.boxes.find(b=>b.id===id),b=box||{id:slot.id,name:'Rak '+slot.output+' Box '+slot.number,output:slot.output,start_led:(slot.number-1)*3+1,led_count:3};
 for(const [field,key] of [['box-id','id'],['box-name','name'],['box-output','output'],['box-start','start_led'],['box-count','led_count']])$(field).value=b[key];
 $('box-color').value=hex(b.color||[0,255,0]);$('color').value=hex(b.color||[0,255,0]);colorLabel();capacityLabel();$('popup-message').textContent='';$('box-modal').hidden=false;document.body.classList.add('modal-open');$('close-box').focus();$('box-save-state').textContent=box?'Mapping tersimpan. Tes LED tersedia di bawah.':'Belum dikonfigurasi. Simpan mapping sebelum tes LED.';
 cards();
}
function matchMediaMobile(){return window.innerWidth<=760;}
function updateControl(){
 for(const id of ['program-test','program-idle','program-stop','save-outputs'])$(id).disabled=!online||busy;
 const box=config?.boxes.find(b=>b.id===selected), state=latest?.boxes.find(s=>s.id===selected);
 document.querySelectorAll('[data-control]').forEach(b=>b.disabled=!box||!online||busy);
 $('box-editor').hidden=!box&&!selectedSlot;$('save-box').disabled=(!box&&!selectedSlot)||!online||busy;
 $('selected-id').textContent=box?.id||selectedSlot?.id||'Pilih box';$('selected-name').textContent=box?.name||'—';
 $('selected-location').textContent=box?'Rak '+box.output+' / GPIO '+[4,5,6,7][box.output-1]:'—';
 $('selected-leds').textContent=box?box.start_led+'–'+(box.start_led+box.led_count-1)+' ('+box.led_count+' LED)':'—';
 $('selected-state').textContent=!box?(selectedSlot?'Belum dikonfigurasi':'—'):!online?'Data terakhir / terputus':state?.on?(state.mode==='blink'?'Kedip':'Aktif')+' • '+Math.ceil(state.remaining_ms/1000)+'s':state?.program_on?'Program '+latest.program.pattern:'Padam';
 $('led-preview').replaceChildren();if(box)for(let i=0;i<Math.min(box.led_count,12);i++){
  const led=document.createElement('span');led.className='diode';led.textContent=box.start_led+i;
  if(state?.on||state?.program_on){led.style.background=rgb(state.on?state.color:state.program_color);led.style.color='#061020';} $('led-preview').append(led);}
 if(box?.led_count>12){const rest=document.createElement('small');rest.textContent='+'+(box.led_count-12)+' LED';$('led-preview').append(rest);}
}
function rgb(color){return 'rgb('+color.join(',')+')';}
function hex(color){return '#'+color.map(c=>c.toString(16).padStart(2,'0')).join('');}
function channels(value){return [1,3,5].map(i=>parseInt(value.slice(i,i+2),16));}
function capacityLabel(){const r=Number($('box-output').value);$('box-capacity').textContent='Rak '+r+': '+(config?.outputs[r-1]||0)+' LED tersedia. Atur jumlah LED di Konfigurasi → Output hardware.';}
function closeBox(){$('box-modal').hidden=true;document.body.classList.remove('modal-open');document.querySelector('[data-box-id="'+(selected||selectedSlot?.id)+'"]')?.focus();}
$('close-box').onclick=closeBox;
$('box-modal').onclick=e=>{if(e.target===$('box-modal'))closeBox();};
document.addEventListener('keydown',e=>{if($('box-modal').hidden)return;if(e.key==='Escape'){closeBox();return;}if(e.key==='Tab'){const items=Array.from($('control').querySelectorAll('button,input,select')).filter(el=>!el.disabled&&!el.closest('[hidden]'));const first=items[0],last=items.at(-1);if(e.shiftKey&&document.activeElement===first){e.preventDefault();last.focus();}else if(!e.shiftKey&&document.activeElement===last){e.preventDefault();first.focus();}}});
$('go-output-settings').onclick=()=>{closeBox();page('config');$('output-0').focus();};
function cards(){
 const key=JSON.stringify(config?.boxes||[]);
 if(key!==layoutKey||!$('box-grid').children.length){
  layoutKey=key;$('box-grid').replaceChildren();
  for(let r=1;r<=4;r++){
   const rack=document.createElement('section');rack.className='rack panel';rack.dataset.rack=r;
   const heading=document.createElement('h2'),title=document.createElement('span'),meta=document.createElement('small');
   title.textContent='RAK '+String(r).padStart(2,'0');heading.append(title,meta);const grid=document.createElement('div');grid.className='rack-grid';
   const boxes=(config?.boxes||[]).filter(b=>b.output===r).sort((a,b)=>a.start_led-b.start_led);
   const slots=Array(25).fill(null),extra=[];
   for(const b of boxes){const m=b.id.match(new RegExp('^R'+String(r).padStart(2,'0')+'-B(\\d{2})$'));const n=m?Number(m[1]):0;if(n>=1&&n<=25&&!slots[n-1])slots[n-1]=b;else extra.push(b);}
   for(const b of extra){const empty=slots.indexOf(null);if(empty>=0)slots[empty]=b;else slots.push(b);}
   for(let i=0;i<slots.length;i++){
    const box=slots[i],slot={id:'R'+String(r).padStart(2,'0')+'-B'+String(i+1).padStart(2,'0'),output:r,number:i+1},card=document.createElement('button');card.type='button';card.className='box';
    card.dataset.boxId=box?.id||slot.id;card.dataset.placeholder=box?'0':'1';card.disabled=false;
    const id=document.createElement('strong'),name=document.createElement('span'),detail=document.createElement('small'),status=document.createElement('span');
    name.className='box-name';status.className='state-label';id.textContent=box?.id.match(/B[0-9]+$/)?.[0]||box?.id||'B'+String(i+1).padStart(2,'0');
    name.textContent=box?.name||'Belum diatur';detail.textContent=box?'LED '+box.start_led+'–'+(box.start_led+box.led_count-1):'—';
    card.title=box?box.id+' • '+box.name:slot.id+' • Klik untuk konfigurasi';const savedColor=document.createElement('span');savedColor.className='box-color';savedColor.textContent=box?hex(box.color||[0,255,0]).toUpperCase():'';card.style.setProperty('--saved-color',box?rgb(box.color||[0,255,0]):'#39445a');card.append(id,name,detail,savedColor,status);
    card.onclick=()=>selectBox(box?.id||null,box?null:slot);grid.append(card);
   }
   const tester=document.createElement('section');tester.className='divider';
   const titleTest=document.createElement('strong');titleTest.textContent='Tes hardware rak '+r+' • 3 detik';tester.append(titleTest);
   const controls=document.createElement('div');controls.className='row';controls.style.marginTop='10px';
   const brightLabel=document.createElement('label');brightLabel.textContent='Brightness tes rak '+r;
   const bright=document.createElement('input');bright.type='number';bright.min=0;bright.max=255;bright.value=32;bright.id='rack-test-brightness-'+r;brightLabel.append(bright);
   const colorLabel=document.createElement('label');colorLabel.textContent='Warna tes rak '+r;
   const color=document.createElement('input');color.type='color';color.value='#00ff00';color.id='rack-test-color-'+r;colorLabel.append(color);controls.append(brightLabel,colorLabel);tester.append(controls);
   const buttons=document.createElement('div');buttons.className='row';buttons.style.marginTop='10px';
   for(const [key,label,color,mode] of [['red','Merah',[255,0,0],'steady'],['green','Hijau',[0,255,0],'steady'],['blue','Biru',[0,0,255],'steady'],['white','Putih',[255,255,255],'steady'],['custom','Warna pilihan',null,'steady'],['blink','Kedip',null,'blink'],['off','Padam',null,'steady']]){
    const button=document.createElement('button');button.id='rack-test-'+key+'-'+r;button.dataset.rackTest=r;button.textContent=label;button.onclick=()=>testRack(r,key!=='off',color,mode);buttons.append(button);
   }tester.append(buttons);
   const info=document.createElement('p');info.id='rack-test-state-'+r;info.className='note';tester.append(info);rack.append(heading,grid,tester);$('box-grid').append(rack);
  }
 }
 const term=$('search').value.toLowerCase(),filter=$('state-filter').value,rackFilter=Number($('rack-filter').value);
 for(const rack of $('box-grid').children){
  const r=Number(rack.dataset.rack),configured=(config?.boxes||[]).filter(b=>b.output===r);
  const active=configured.filter(b=>latest?.boxes.find(s=>s.id===b.id)?.on).length;
  rack.querySelector('h2 small').textContent='GPIO '+[4,5,6,7][r-1]+(config?' • '+config.outputs[r-1]+' LED • '+active+' aktif':' • Mapping belum tersedia');
  const test=latest?.hardware_tests?.find(t=>t.output===r),count=config?.outputs[r-1]||0;
  $('rack-test-state-'+r).textContent=!config?'Menunggu mapping controller.':!count?'Isi jumlah LED terpasang untuk rak ini di Konfigurasi dahulu.':test?.on?'Tes hardware LED 1–'+count+' • '+Math.ceil(test.remaining_ms/1000)+'s':'Mengikuti setting rak: LED 1–'+count+'. Tes menghentikan box aktif pada rak ini.';
  rack.querySelectorAll('[data-rack-test]').forEach(b=>b.disabled=!online||busy||!count);
  let visible=0;
  for(const card of rack.querySelectorAll('.box')){
   const box=configured.find(b=>b.id===card.dataset.boxId),state=latest?.boxes.find(s=>s.id===box?.id),manual=!!state?.on,on=manual||!!state?.program_on;
   card.className='box'+(!box?' placeholder':'')+(on?' active':'')+((box&&box.id===selected)||(!box&&selectedSlot?.id===card.dataset.boxId)?' selected':'');
   card.style.setProperty('--led',on?rgb(manual?state.color:state.program_color):'#9cacc6');
   card.querySelector('.state-label').textContent=!config?'MEMUAT MAPPING':!box?'BELUM DIATUR':!online?'DATA TERAKHIR':on?(manual?(state.mode==='blink'?'KEDIP':'AKTIF')+' '+Math.ceil(state.remaining_ms/1000)+'s':'PROGRAM'):'PADAM';
   card.hidden=!!(term&&!((box?box.id+' '+box.name:card.dataset.boxId+' Belum diatur').toLowerCase().includes(term)))||filter==='on'&&!on||filter==='off'&&(!box||on)||filter==='unconfigured'&&!!box;
   if(!card.hidden)visible++;
  }rack.hidden=(rackFilter!==0&&rackFilter!==r)||!visible;
 }
 if(selected&&!config?.boxes.some(b=>b.id===selected))selected=null;
 updateControl();
}
async function testRack(output,on,color,mode){
 if(busy)return;busy=true;cards();
 await run(async()=>{if(!online)throw new Error('Status controller belum tersedia');
  const chosen=$('rack-test-color-'+output).value;
  const payload=on?{on:true,color:color||[1,3,5].map(i=>parseInt(chosen.slice(i,i+2),16)),brightness:integer('rack-test-brightness-'+output,0,255),mode,duration_ms:3000}:{on:false};
  const state=await api('/outputs/'+output+'/test','PUT',payload);await refresh();message('Rak '+output+': '+(state.on?'tes hardware LED 1–'+state.led_count+' selama 3 detik':'padam')+'; brightness efektif '+state.brightness);
 });busy=false;cards();
}
function connection(ok){online=ok;$('connection').textContent=ok?'LIVE / TERHUBUNG':'STATUS TIDAK TERSEDIA';$('connection').className='badge'+(ok?'':' stale');updateControl();}
async function refresh(){latest=await api('/status');connection(true);$('active-count').textContent=latest.boxes.filter(b=>b.on||b.program_on).length;const program=latest.program;$('program-state').textContent=program?.on?(program.idle?'IDLE':'TES')+' • '+program.pattern+' • '+(program.idle?'berkelanjutan':Math.ceil(program.remaining_ms/1000)+'s'):'Program padam';$('configured-count').textContent=config.boxes.length;$('led-count').textContent=latest.outputs.reduce((a,b)=>a+b,0);$('global-limit').textContent=Math.round(latest.brightness_limit/255*100)+'%';$('version').textContent='Firmware '+latest.version;
 $('summary').textContent='IP '+latest.ip+' | Router '+(latest.sta_connected?'terhubung':'terputus')+' | AP '+(latest.ap_active?latest.ap_ssid:'mati')+' | LED/output '+latest.outputs.join(', ')+' | Batas brightness '+latest.brightness_limit+' | OTA '+Math.ceil(latest.ota_remaining_ms/ 1000)+' dtk'+(latest.storage_error?' | Penyimpanan: '+latest.storage_error:'');cards();}
async function run(action){try{await action();}catch(e){if(e.name==='AbortError'||e instanceof TypeError||/^401/.test(e.message)) {connection(false);cards();}message(e.name==='AbortError'?'Timeout: hasil perintah belum diketahui. Baca status sebelum mengirim ulang.':e.message);}}
async function command(id,on,override={}){if(busy)return;busy=true;updateControl();await run(async()=>{if(!online)throw new Error('Hubungkan dan baca status dahulu');const c=$('color').value;
 const payload=on?{on:true,color:[1,3,5].map(i=>parseInt(c.slice(i,i+2),16)),brightness:integer('brightness',0,255),mode:$('mode').value,duration_ms:integer('seconds',1,3600)*1000,...override}:{on:false};
 const state=await api('/boxes/'+encodeURIComponent(id)+'/state','PUT',payload);message(id+': '+(state.on?'nyala':'padam')+'; brightness efektif '+state.brightness);await refresh();});busy=false;updateControl();}
async function initialize(){
 if(loading)return;loading=true;
 try{config=await api('/config');showConfig();await refresh();message('Terhubung tanpa token. Klik box untuk konfigurasi atau tes LED.');}
 finally{loading=false;}
}
$('connect').onclick=()=>run(initialize);
$('box-output').onchange=capacityLabel;
for(const id of ['box-id','box-name','box-output','box-start','box-count','box-color'])$(id).addEventListener('input',()=>{$('box-save-state').textContent='Draft box belum disimpan.';});
$('save-box').onclick=()=>run(async()=>{
 if(!config||!online)throw new Error('Hubungkan dan baca status dahulu');
 if($('config-state').textContent==='Draft belum disimpan')throw new Error('Konfigurasi global memiliki draft. Simpan atau buang draft dahulu di Konfigurasi.');
 const original=selected,b={id:$('box-id').value.trim(),name:$('box-name').value,output:integer('box-output',1,4),start_led:integer('box-start',1,75),led_count:integer('box-count',1,75),color:channels($('box-color').value)};
 busy=true;updateControl();
 try{
  const current=await api('/config'),candidate={schema:current.schema,brightness_limit:current.brightness_limit,outputs:[...current.outputs],boxes:current.boxes.map(b=>({...b}))};
  const index=candidate.boxes.findIndex(b=>b.id===original);
  if(original&&index<0)throw new Error('Box berubah pada controller. Muat ulang konfigurasi dahulu.');
  if(index>=0)candidate.boxes[index]=b;else candidate.boxes.push(b);
  validateMapping(candidate);
  config=await api('/config','PUT',candidate);showConfig();selectBox(b.id);await refresh();message(b.id+': konfigurasi tersimpan; seluruh LED padam. Pilih Tes warna untuk menyalakan box ini.');
 }finally{busy=false;updateControl();}
});
$('save-outputs').onclick=()=>run(async()=>{if(!online||busy)throw new Error('Tunggu controller siap');busy=true;try{const current=await api('/config');const mappingDraft=Array.from($('mapping').children).map(row=>{const b={};row.querySelectorAll('input').forEach(el=>b[el.dataset.field]=el.type==='number'?Number(el.value):el.type==='color'?channels(el.value):el.value);return b;});const otherDraft=JSON.stringify(mappingDraft)!==JSON.stringify(current.boxes.map(b=>({...b,color:b.color||[0,255,0]})))||$('ssid').value!==current.wifi.ssid||$('replace-pass').checked;const candidate={schema:current.schema,brightness_limit:integer('limit',0,255),outputs:[0,1,2,3].map(i=>integer('output-'+i,0,75)),boxes:current.boxes};validateMapping(candidate);config=await api('/config','PUT',candidate);if(!otherDraft)showConfig();else $('config-state').textContent='Draft belum disimpan';await refresh();message('Setting rak tersimpan; mapping box tetap. Seluruh program/LED padam.'+(otherDraft?' Draft mapping/jaringan tetap belum disimpan.':''));}finally{busy=false;cards();}});
for(const [id,idle] of [['program-test',false],['program-idle',true],['program-stop',false]])$(id).onclick=()=>run(async()=>{if(!online||busy)throw new Error('Tunggu controller siap');busy=true;try{const on=id!=='program-stop';await api('/program','PUT',on?{on:true,idle,pattern:$('program-pattern').value,brightness:integer('program-brightness',0,255),step_ms:integer('program-step',100,5000),duration_ms:integer('program-seconds',1,3600)*1000}:{on:false});await refresh();message(on?(idle?'Mode idle berjalan.':'Tes otomatis berjalan.'):'Program otomatis dihentikan.');}finally{busy=false;cards();}});
$('search').oninput=cards;$('rack-filter').onchange=cards;$('state-filter').onchange=cards;
$('all-off').onclick=()=>run(async()=>{await api('/all-off','POST');await refresh();message('Semua box padam.');});
$('generate').onclick=()=>run(async()=>{if(!config)throw new Error('Hubungkan dahulu');const racks=integer('rack-count',1,4),boxes=integer('boxes-per-rack',1,25),leds=integer('leds-per-box',1,75);if(boxes*leds>75)throw new Error('Box × LED/box melebihi 75 LED per rak');
 $('mapping').replaceChildren();for(let r=1;r<=4;r++){$('output-'+(r-1)).value=r<=racks?boxes*leds:0;if(r<=racks)for(let b=1;b<=boxes;b++)mappingRow({id:'R'+String(r).padStart(2,'0')+'-B'+String(b).padStart(2,'0'),name:'Rak '+r+' Box '+b,output:r,start_led:(b-1)*leds+1,led_count:leds});}$('config-state').textContent='Draft belum disimpan';message('Draft diganti dengan '+(racks*boxes)+' box. Belum diterapkan; tekan Simpan atau Muat ulang untuk membuang draft.');});
$('add-box').onclick=()=>{if(!config)return message('Hubungkan dahulu');if($('mapping').children.length>=100)return message('Maksimum 100 box');mappingRow({id:'',name:'',output:1,start_led:1,led_count:3});$('config-state').textContent='Draft belum disimpan';};
$('save').onclick=()=>run(async()=>{if(!config)throw new Error('Hubungkan dahulu');config=await api('/config','PUT',draft());showConfig();message('Konfigurasi tersimpan; seluruh LED padam. Jika Wi-Fi berubah, sambungkan ulang.');await refresh();});
$('reload').onclick=()=>run(async()=>{config=await api('/config');showConfig();message('Konfigurasi dimuat; draft sebelumnya dibuang.');});
for(const [id,enabled] of [['ota-on',true],['ota-off',false]])$(id).onclick=()=>run(async()=>{await api('/ota','POST',{enabled});await refresh();message(enabled?'OTA aktif maksimum 10 menit.':'OTA dimatikan.');});
setInterval(async()=>{if(!config||loading||polling||document.hidden)return;polling=true;try{await refresh();}catch(e){connection(false);cards();message('Status tidak tersedia: '+e.message);}finally{polling=false;}},1000);
for(const id of ['limit','ssid','wifi-pass','replace-pass'])$(id).addEventListener('input',()=>{$('config-state').textContent='Draft belum disimpan';});

function page(name){closeBox();$('map-page').hidden=name!=='map';$('config-page').hidden=name!=='config';$('nav-map').classList.toggle('active',name==='map');$('nav-config').classList.toggle('active',name==='config');}
$('nav-map').onclick=()=>page('map');$('nav-config').onclick=()=>page('config');
$('setup-individual').onclick=()=>{if(!config)return message('Hubungkan dahulu');$('rack-count').value=1;$('boxes-per-rack').value=3;$('leds-per-box').value=1;$('generate').click();};
$('setup-three').onclick=()=>{if(!config)return message('Hubungkan dahulu');$('rack-count').value=1;$('boxes-per-rack').value=1;$('leds-per-box').value=3;$('generate').click();};
function colorLabel(){$('color-value').textContent=$('color').value.toUpperCase();}
$('color').oninput=colorLabel;
document.querySelectorAll('[data-color]').forEach(b=>b.onclick=()=>{$('color').value=b.dataset.color;colorLabel();});
document.querySelectorAll('[data-seconds]').forEach(b=>b.onclick=()=>{$('seconds').value=b.dataset.seconds;});
function brightnessLabel(){$('brightness-value').textContent=Math.round(Number($('brightness').value)/255*100)+'%';}
$('brightness-range').oninput=()=>{$('brightness').value=$('brightness-range').value;brightnessLabel();};
$('brightness').oninput=()=>{$('brightness-range').value=$('brightness').value;brightnessLabel();};
$('turn-on').onclick=()=>selected&&command(selected,true);$('turn-off').onclick=()=>selected&&command(selected,false);
$('test-blink').onclick=()=>selected&&command(selected,true,{mode:'blink',duration_ms:3000});
for(const [id,color] of [['test-red',[255,0,0]],['test-green',[0,255,0]],['test-blue',[0,0,255]]])$(id).onclick=()=>selected&&command(selected,true,{color,mode:'steady',duration_ms:3000});
if(matchMediaMobile())$('rack-filter').value='1';updateControl();cards();$('connect').click();
</script></body></html>)HTML";
