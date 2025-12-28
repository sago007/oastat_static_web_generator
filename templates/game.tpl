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
	<script src="../static/js/score-chart.js"></script>
	<title>OpenArena Stats - Game {{GAME_NUMBER}}</title>
</head>
<body>
<div class="main-section">
	<div class="intro">OpenArena Stats - Game {{GAME_NUMBER}}</div>
	{{GAME_MAP}} at {{GAME_SERVERNAME}}
        <div class="levelshot">
            <img src="../static/images/oa640x400/{{GAME_MAP}}.jpg" alt="{{GAME_MAP}}">
        </div>

	<h2>Score Progression Over Time</h2>
	<div id="score-chart" style="margin: 20px 0;"></div>
	<div id="score-legend" class="legend"></div>

	<h2>Final Scores</h2>
	<table>
		<tr>
			<th>Player</th>
			<th>Score</th>
		</tr>
		{{#SCORES_LIST}}
		<tr>
			<td>{{PLAYER_NICKNAME}}</td>
			<td>{{SCORE}}</td>
		</tr>
		{{/SCORES_LIST}}
	</table>

	{{#HAS_KILL_MATRIX}}
	<h2>Kill Matrix</h2>
	<table class="kill-matrix">
		<thead>
		<tr>
			<th>Killer ⟍ Victim</th>
			{{#MATRIX_HEADER}}
			<th>{{PLAYER_NICKNAME}}</th>
			{{/MATRIX_HEADER}}
		</tr>
		</thead>
		<tbody>
		{{#MATRIX_ROWS}}
		<tr>
			<td>{{KILLER_NICKNAME}}</td>
			{{#MATRIX_CELLS}}
			<td{{#HAS_KILLS}} class="has-kills"{{/HAS_KILLS}}{{#IS_DIAGONAL}} class="diagonal"{{/IS_DIAGONAL}}>{{KILL_COUNT}}</td>
			{{/MATRIX_CELLS}}
		</tr>
		{{/MATRIX_ROWS}}
		</tbody>
	</table>
	{{/HAS_KILL_MATRIX}}

	<script>
		// Render score chart with game data
		renderScoreChart("{{GAME_NUMBER}}.json");
	</script>
</div>
</body>
</html>
