// Execute the real embedded HTML/JS in an isolated DOM with a synthetic HTTP fixture.
// This is a UI test, not ESP32 WebServer or hardware validation.
const {JSDOM} = require('../_sandbox/ui-tests/node_modules/jsdom');
const fs = require('node:fs');
const path = require('node:path');
const assert = require('node:assert/strict');
const source = fs.readFileSync(path.join(__dirname, '../02_Firmware/WarehouseLed/web_ui.h'), 'utf8');
const html = source.split('R"HTML(')[1].split(')HTML"')[0];
assert.doesNotMatch(html, /â|Ã|Â/, 'UTF-8 labels must not contain mojibake');
const clone = obj => JSON.parse(JSON.stringify(obj));
let config = {schema:1, brightness_limit:128, outputs:[3,0,0,0],
  boxes:[{id:'R01-B01',name:'Rak 1 Box 1',output:1,start_led:1,led_count:3}],wifi:{ssid:'',password_configured:false}};
let states = new Map(), calls = [], poll, failStatus=false;
const dom = new JSDOM(html, {url:'http://warehouse-fixture.test/', runScripts:'dangerously', pretendToBeVisual:true,
  beforeParse(window) {
    window.AbortController = global.AbortController;
    window.setInterval = callback => { poll = callback; return 1; };
    window.fetch = async (url, options) => {
      const payload = options.body ? JSON.parse(options.body) : undefined;
      calls.push({url,method:options.method,payload});
      if(options.headers.Authorization !== 'Bearer '+ 'a'.repeat(64))
        return {ok:false,status:401,json:async()=>({error:'invalid token'})};
      let result;
      if(url.endsWith('/config') && options.method==='PUT') {
        const seen=new Set(), occupied=new Set();
        for(const box of payload.boxes) {
          if(seen.has(box.id)) return {ok:false,status:400,json:async()=>({error:'duplicate box ID'})};
          seen.add(box.id);
          for(let i=box.start_led;i<box.start_led+box.led_count;i++) {
            const key=box.output+':'+i;
            if(occupied.has(key)) return {ok:false,status:400,json:async()=>({error:'overlapping box mapping'})};
            occupied.add(key);
          }
        }
        config=clone(payload);config.wifi.password_configured=false;states.clear();result=config;
      } else if(url.endsWith('/config')) result=config;
      else if(url.endsWith('/status')) {if(failStatus)throw new TypeError('Network disconnected');result={version:'v0.3.2',ip:'fixture',sta_connected:true,ap_active:false,
        ap_ssid:'Warehouse-fixture',outputs:config.outputs,brightness_limit:config.brightness_limit,
        ota_remaining_ms:0,storage_error:'',boxes:config.boxes.map(b=>states.get(b.id)||
          {id:b.id,on:false,remaining_ms:0,color:[0,255,0],brightness:128,mode:'steady'})};}
      else if(url.endsWith('/all-off')) {states.clear();result={ok:true};}
      else if(url.endsWith('/state')) {
        const id=url.split('/').at(-2);result={...payload,id,remaining_ms:payload.on?payload.duration_ms:0,
          brightness:Math.min(payload.brightness||128,config.brightness_limit)};
        states.set(id,result);
      } else if(url.endsWith('/ota')) result={remaining_ms:payload.enabled?600000:0};
      else throw new Error('Unexpected test request '+url);
      return {ok:true,status:200,json:async()=>clone(result)};
    };
  }});
const document=dom.window.document, $=id=>document.getElementById(id);
const settle=async()=>{for(let i=0;i<6;i++)await new Promise(resolve=>setImmediate(resolve));};
async function click(id){$(id).click();await settle();}
async function main(){
  $('token').value='b'.repeat(64);await click('connect');assert.match($('message').textContent,/401/);
  $('token').value='a'.repeat(64);await click('connect');
  assert.equal($('mapping').children.length,1);assert.equal($('box-grid').children.length,4);
  assert.equal(document.querySelectorAll('.box').length,100);
  assert.equal(document.querySelectorAll('.box.placeholder:disabled').length,99);
  assert.equal($('turn-on').disabled,true);
  $('rack-count').value=4;$('boxes-per-rack').value=25;await click('generate');
  assert.equal($('mapping').children.length,100);
  assert.deepEqual(Array.from($('output-inputs').querySelectorAll('input')).map(x=>x.value),['75','75','75','75']);
  assert.deepEqual(Array.from($('mapping').lastChild.querySelectorAll('input')).map(x=>x.value),['R04-B25','Rak 4 Box 25','4','73','3']);
  assert.equal(config.boxes.length,1); // Draft generator does not write config.
  await click('save');assert.equal(config.boxes.length,100);assert.equal(document.querySelectorAll('.box:not(.placeholder)').length,100);
  $('search').value='R04-B25';$('search').dispatchEvent(new dom.window.Event('input'));
  assert.equal(document.querySelectorAll('.box:not([hidden])').length,1);
  const card=document.querySelector('[data-box-id="R04-B25"]');
  card.click();await settle();assert.equal($('selected-id').textContent,'R04-B25');
  assert.equal($('selected-leds').textContent,'73–75 (3 LED)');
  $('brightness').value=255;$('seconds').value=5;$('mode').value='blink';$('color').value='#ff0010';
  await click('turn-on');assert.match($('message').textContent,/brightness efektif 128/);
  assert.deepEqual(states.get('R04-B25').color,[255,0,16]);
  assert.equal(states.get('R04-B25').duration_ms,5000);
  assert.equal(states.get('R04-B25').mode,'blink');
  await poll();assert.equal(document.querySelector('[data-box-id="R04-B25"]'),card);
  assert.equal($('selected-id').textContent,'R04-B25');assert.equal($('turn-on').disabled,false);
  for(const [button,color] of [['test-red',[255,0,0]],['test-green',[0,255,0]],['test-blue',[0,0,255]]]){
    await click(button);assert.deepEqual(states.get('R04-B25').color,color);
    assert.equal(states.get('R04-B25').duration_ms,3000);
  }
  await click('test-blink');assert.equal(states.get('R04-B25').mode,'blink');assert.equal(states.get('R04-B25').duration_ms,3000);
  failStatus=true;await poll();assert.equal($('turn-on').disabled,true);
  assert.match($('selected-state').textContent,/Data terakhir/);
  failStatus=false;await poll();assert.equal($('turn-on').disabled,false);
  // A rejected save preserves device config and active state while keeping the visible draft.
  const rows=$('mapping').children;
  rows[1].querySelector('[data-field=id]').value='R01-B01';
  await click('save');assert.match($('message').textContent,/duplicate box ID/);
  assert.equal(config.boxes[1].id,'R01-B02');assert.equal(states.get('R04-B25').on,true);
  await click('all-off');assert.equal(states.size,0);
  await click('reload');assert.equal(rows[1].querySelector('[data-field=id]').value,'R01-B02');
  $('rack-count').value=1;$('boxes-per-rack').value=25;$('leds-per-box').value=4;
  await click('generate');assert.match($('message').textContent,/melebihi 75/);assert.equal($('mapping').children.length,100);
  await click('setup-three');assert.equal($('mapping').children.length,1);assert.equal(config.boxes.length,100);
  await click('save');assert.equal(config.boxes.length,1);assert.equal(document.querySelectorAll('.box.placeholder').length,99);
  await click('setup-individual');assert.equal($('mapping').children.length,3);assert.equal(config.boxes.length,1);
  await click('save');assert.deepEqual(config.boxes.map(b=>[b.start_led,b.led_count]),[[1,1],[2,1],[3,1]]);
  await click('nav-map');assert.equal($('map-page').hidden,false);assert.equal($('config-page').hidden,true);
  $('search').value='';$('search').dispatchEvent(new dom.window.Event('input'));
  $('rack-filter').value='1';$('rack-filter').dispatchEvent(new dom.window.Event('change'));
  assert.equal(document.querySelectorAll('.rack:not([hidden])').length,1);
  await click('logout');assert.equal(dom.window.sessionStorage.length,0);assert.equal($('box-grid').children.length,0);
  const n=calls.length;await poll();assert.equal(calls.length,n);
  dom.window.close();console.log('UI DOM tests passed: auth, four racks/100 slots, selection, manual RGB/blink, stale status, mapping draft/save, limits, logout');
}
main().catch(error=>{console.error(error);dom.window.close();process.exitCode=1;});
