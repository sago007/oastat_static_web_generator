/**
 * Score Progression Chart
 * Renders a D3.js line chart showing player scores over time
 *
 * @param {string} dataUrl - URL to the JSON file containing score data
 */
function renderScoreChart(dataUrl) {
	// Load score progression data from JSON file
	d3.json(dataUrl)
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
}
