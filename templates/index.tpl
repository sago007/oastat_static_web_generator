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
		<link rel="stylesheet" href="static/css/oastat.css" type="text/css">
		<title>OpenArena Stats</title>
	</head>

	<body>
		<div class="main-section">
		<h1>OpenArena Stats</h1>

		<table>
			<tr>
				<th>Name</th>
				<th>Kills</th>
				<th>Deaths</th>
				<th>Last seen</th>
				<th>Is bot</th>
				<th>Gauntlet</th>
				<th>Impressive</th>
				<th>Excellent</th>
				<th>Capture</th>
				<th>Defence</th>
				<th>Assist</th>
			</tr>
			{{#PLAYER_LIST}}
			<tr>
				<td>{{PLAYER_NAME}}</td>
				<td>{{PLAYER_KILLS}}</td>
				<td>{{PLAYER_DEATHS}}</td>
				<td>{{PLAYER_LAST_SEEN}}</td>
				<td>{{PLAYER_IS_BOT}}</td>
				<td>{{AWARD_GAUNTLET}}</td>
				<td>{{AWARD_IMPRESSIVE}}</td>
				<td>{{AWARD_EXCELLENT}}</td>
				<td>{{AWARD_CAPTURE}}</td>
				<td>{{AWARD_DEFENCE}}</td>
				<td>{{AWARD_ASSIST}}</td>
			</tr>
			{{/PLAYER_LIST}}
		</table>

		<table>
			<tr>
				<th>Name</th>
				<th>Shotgun</th>
				<th>Gauntlet</th>
				<th>Machinegun</th>
				<th>Grenade</th>
				<th>Rocket</th>
				<th>Plasma</th>
				<th>Railgun</th>
				<th>Lightning</th>
				<th>Nailgun</th>
				<th>Chaingun</th>
				<th>BFG</th>
				<th>Telefrag</th>
				<th>Falling</th>
			</tr>
			{{#PLAYER_WEAPON_LIST}}
			<tr>
				<td>{{PLAYER_NAME}}</td>
				<td>{{WEAPON_SHOTGUN}}</td>
				<td>{{WEAPON_GAUNTLET}}</td>
				<td>{{WEAPON_MACHINEGUN}}</td>
				<td>{{WEAPON_GRENADE}}</td>
				<td>{{WEAPON_ROCKET}}</td>
				<td>{{WEAPON_PLASMA}}</td>
				<td>{{WEAPON_RAILGUN}}</td>
				<td>{{WEAPON_LIGHTNING}}</td>
				<td>{{WEAPON_NAILGUN}}</td>
				<td>{{WEAPON_CHAINGUN}}</td>
				<td>{{WEAPON_BFG}}</td>
				<td>{{WEAPON_TELEFRAG}}</td>
				<td>{{WEAPON_FALLING}}</td>
			</tr>
			{{/PLAYER_WEAPON_LIST}}
		</table>

		<table>
			<tr>
				<th>Map</th>
				<th>Times played</th>
				<th>Last played at</th>
			</tr>
			{{#MAP_LIST}}
			<tr>
				<td>{{MAP_NAME}}</td>
				<td>{{TIMES_PLAYED}}</td>
				<td>{{LAST_PLAYED}}</td>
			</tr>
			{{/MAP_LIST}}
		</table>

		<table>
			<tr>
				<th>Game</th>
				<th>Map</th>
				<th>Played at</th>
				<th>Server name</th>
			</tr>
			{{#RECENT_GAMES}}
			<tr>
				<td><a href="game/{{GAMENUMBER}}.html">{{GAMENUMBER}}</a></td>
				<td>{{MAPNAME}}</td>
				<td>{{TIME}}</td>
				<td>{{SERVERNAME}}</td>
			</tr>
			{{/RECENT_GAMES}}
		</table>

		<p class="update">Last updated on {{GENERATION_DATE}}</p>
		</div>
	</body>

</html>
