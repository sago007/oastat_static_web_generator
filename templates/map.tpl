{{%AUTOESCAPE context="HTML"}}

{{!
  Copyright (c) 2019 Poul Sander

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
}}

<html>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	<meta http-equiv="Content-Language" content="en">
	<meta name="description" content="OpenArena stats by oastats">
	<link rel="stylesheet" href="../static/css/oastat.css" type="text/css">
	<link rel="stylesheet" href="../static/css/oastat_d3.css" type="text/css">
	<script src="https://d3js.org/d3.v7.min.js"></script>
	<script src="../static/js/weapon-pie-chart.js"></script>
	<title>OpenArena Stats - Map {{MAP_NAME}}</title>
</head>
<body>
<div class="main-section">
	<h1>Map: {{MAP_NAME}}</h1>

	<div class="levelshot">
		<img src="../static/images/oa640x400/{{MAP_NAME}}.jpg" alt="{{MAP_NAME}}">
	</div>

	<h2>Weapon Kills on this Map</h2>
	
	<div class="pie-chart">
		<div id="weapon-pie-chart"></div>
	</div>
	<div class="legend" id="weapon-legend"></div>
	
	<table>
		<tr>
			<th>Weapon</th>
			<th>Kills</th>
		</tr>
		{{#WEAPON_KILLS}}
		<tr>
			<td>{{WEAPON_NAME}}</td>
			<td>{{KILL_COUNT}}</td>
		</tr>
		{{/WEAPON_KILLS}}
	</table>
	
	<script>
		// Weapon kills data
		const weaponData = [
			{{#WEAPON_KILLS}}
			{name: "{{WEAPON_NAME}}", value: {{KILL_COUNT}}},
			{{/WEAPON_KILLS}}
		];

		// Render weapon pie chart with data
		renderWeaponPieChart(weaponData);
	</script>

	<h2>Most Recent Matches</h2>
	<table>
		<tr>
			<th>Game</th>
			<th>Played at</th>
			<th>Server name</th>
		</tr>
		{{#RECENT_MATCHES}}
		<tr>
			<td><a href="../game/{{GAMENUMBER}}.html">{{GAMENUMBER}}</a></td>
			<td>{{TIME}}</td>
			<td>{{SERVERNAME}}</td>
		</tr>
		{{/RECENT_MATCHES}}
	</table>

	<p><a href="../index.html">Back to main page</a></p>
	<p class="update">Last updated on {{GENERATION_DATE}}</p>
</div>
</body>
</html>
