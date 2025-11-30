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
	
	<script>
		// Load score progression data from JSON file
		d3.json("{{GAME_NUMBER}}.json")
			.then(function(scoreData) {
				console.log("Loaded score data:", scoreData);
				
				if (!scoreData || scoreData.length === 0) {
					console.log("No score data available");
					return;
				}
				
				// Set up dimensions
			const margin = {top: 20, right: 120, bottom: 50, left: 60};
			const width = 800 - margin.left - margin.right;
			const height = 400 - margin.top - margin.bottom;
			
			// Create SVG
			const svg = d3.select("#score-chart")
				.append("svg")
				.attr("width", width + margin.left + margin.right)
				.attr("height", height + margin.top + margin.bottom)
				.append("g")
				.attr("transform", `translate(${margin.left},${margin.top})`);
			
			// Find max values for scales
			const maxSecond = d3.max(scoreData, d => d3.max(d.data, p => p.second));
			const maxScore = d3.max(scoreData, d => d3.max(d.data, p => p.score));
			
			// Create scales
			const xScale = d3.scaleLinear()
				.domain([0, maxSecond])
				.range([0, width]);
			
			const yScale = d3.scaleLinear()
				.domain([0, maxScore])
				.range([height, 0]);
			
			// Create axes
			const xAxis = d3.axisBottom(xScale);
			const yAxis = d3.axisLeft(yScale);
			
			// Add axes to SVG
			svg.append("g")
				.attr("transform", `translate(0,${height})`)
				.call(xAxis)
				.append("text")
				.attr("x", width / 2)
				.attr("y", 35)
				.attr("fill", "#000")
				.style("text-anchor", "middle")
				.text("Time (seconds)");
			
			svg.append("g")
				.call(yAxis)
				.append("text")
				.attr("transform", "rotate(-90)")
				.attr("y", -45)
				.attr("x", -height / 2)
				.attr("fill", "#000")
				.style("text-anchor", "middle")
				.text("Score");
			
			// Color scale
			const color = d3.scaleOrdinal()
				.domain(scoreData.map(d => d.player))
				.range(d3.schemeCategory10);
			
			// Line generator
			const line = d3.line()
				.x(d => xScale(d.second))
				.y(d => yScale(d.score));
			
			// Draw lines for each player
			scoreData.forEach(playerData => {
				svg.append("path")
					.datum(playerData.data)
					.attr("fill", "none")
					.attr("stroke", color(playerData.player))
					.attr("stroke-width", 2)
					.attr("d", line);
				
				// Add dots at data points
				svg.selectAll(`.dot-${playerData.playerid}`)
					.data(playerData.data)
					.enter()
					.append("circle")
					.attr("class", `dot-${playerData.playerid}`)
					.attr("cx", d => xScale(d.second))
					.attr("cy", d => yScale(d.score))
					.attr("r", 3)
					.attr("fill", color(playerData.player))
					.append("title")
					.text(d => `${playerData.player}: ${d.score} at ${d.second}s`);
			});
			
			// Create legend
			const legend = d3.select("#score-legend");
			scoreData.forEach(playerData => {
				const item = legend.append("div")
					.attr("class", "legend-item");
				
				item.append("div")
					.attr("class", "legend-color")
					.style("background-color", color(playerData.player));
				
				const finalScore = playerData.data[playerData.data.length - 1].score;
				item.append("span")
					.text(`${playerData.player} (${finalScore})`);
			});
		})
		.catch(function(error) {
			console.error("Error loading score data:", error);
		});
	</script>
</div>
</body>
</html>
