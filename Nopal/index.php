<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<title>ESP8266 Dashboard - Roblox Style</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

<style>
*{box-sizing:border-box}
body{
  margin:0;
  background:#0f172a;
  color:#ffffff;
  font-family:'Segoe UI',Tahoma,sans-serif;
  padding:20px;
}

/* HEADER */
h2{
  text-align:center;
  margin-bottom:20px;
  letter-spacing:1px;
}

/* GRID */
.grid-3{
  display:grid;
  grid-template-columns:repeat(3,1fr);
  gap:16px;
}
.grid-2{
  display:grid;
  grid-template-columns:1fr 2fr;
  gap:16px;
  margin-top:16px;
}

/* CARD ROBLOX */
.card{
  background:#1e293b;
  border-radius:10px;
  padding:16px;
  box-shadow:0 6px 0 #020617;
  border:2px solid #334155;
}

/* TITLE */
.card div:first-child{
  font-size:14px;
  opacity:.85;
  margin-bottom:6px;
}

/* VALUE */
.value{
  font-size:34px;
  font-weight:800;
}

/* COLORS */
.temp{color:#ff6b6b}
.hum{color:#4ea8ff}

/* STATUS */
#status{
  font-size:20px;
  font-weight:700;
}
.status-green{color:#22c55e}
.status-yellow{color:#facc15}
.status-red{color:#ef4444}

/* TIME */
#waktu{
  font-size:22px;
  font-weight:700;
  text-align:center;
  padding-top:10px;
}

/* CHART */
canvas{
  background:#0f172a;
  border-radius:8px;
  padding:6px;
}
</style>
</head>

<body>

<h2>ESP8266 Monitoring Dashboard</h2>

<div class="grid-3">
  <div class="card">
    <div>Suhu</div>
    <div id="suhu" class="value temp">--</div>
  </div>

  <div class="card">
    <div>Kelembapan</div>
    <div id="hum" class="value hum">--</div>
  </div>

  <div class="card">
    <div>Status</div>
    <div id="status">--</div>
  </div>
</div>

<div class="grid-2">
  <div class="card">
    <div>Waktu</div>
    <div id="waktu">--</div>
  </div>

  <div class="card">
    <canvas id="chart"></canvas>
  </div>
</div>

<script>
let label=[], suhu=[], hum=[];
const chart=new Chart(document.getElementById("chart"),{
 type:"line",
 data:{labels:label,datasets:[
  {label:"Suhu",data:suhu,borderColor:"#ff6b6b",tension:.4},
  {label:"Kelembapan",data:hum,borderColor:"#4ea8ff",tension:.4}
 ]},
 options:{plugins:{legend:{labels:{color:"white"}}},
 scales:{x:{ticks:{color:"white"}},y:{ticks:{color:"white"}}}}
});

function statusHum(h){
 let el=document.getElementById("status");
 if(h>=30&&h<60){
   el.textContent="AMAN";
   el.className="status-green";
 }
 else if(h>=60&&h<70){
   el.textContent="NORMAL";
   el.className="status-yellow";
 }
 else if(h>=70){
   el.textContent="TINGGI";
   el.className="status-red";
 }
}

function load(){
 fetch("api_get.php")
 .then(r=>r.json())
 .then(d=>{
  if(!d.suhu) return;

  document.getElementById("suhu").textContent=d.suhu+" °C";
  document.getElementById("hum").textContent=d.kelembapan+" %";
  document.getElementById("waktu").textContent=d.waktu;

  statusHum(d.kelembapan);

  label.push(new Date().toLocaleTimeString());
  suhu.push(d.suhu);
  hum.push(d.kelembapan);

  if(label.length>20){
    label.shift();suhu.shift();hum.shift();
  }
  chart.update();
 });
}

setInterval(load,2000);
load();
</script>

</body>
</html>
