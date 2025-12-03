/**
 * Weapon Kills Pie Chart
 * Renders a D3.js pie chart showing weapon kill distribution
 *
 * @param {Array} weaponData - Array of objects with {name: string, value: number}
 */
function renderWeaponPieChart(weaponData) {
	// Set up dimensions
	const width = 500;
	const height = 400;
	const radius = Math.min(width, height) / 2 - 40;

	// Color scale
	const color = d3.scaleOrdinal()
		.domain(weaponData.map(d => d.name))
		.range(d3.schemeCategory10);

	// Create SVG
	const svg = d3.select("#weapon-pie-chart")
		.append("svg")
		.attr("width", width)
		.attr("height", height)
		.append("g")
		.attr("transform", `translate(${width/2},${height/2})`);

	// Create pie layout
	const pie = d3.pie()
		.value(d => d.value)
		.sort(null);

	// Create arc generator
	const arc = d3.arc()
		.innerRadius(0)
		.outerRadius(radius);

	const labelArc = d3.arc()
		.innerRadius(radius * 0.6)
		.outerRadius(radius * 0.6);

	// Create pie slices
	const arcs = svg.selectAll(".arc")
		.data(pie(weaponData))
		.enter()
		.append("g")
		.attr("class", "arc");

	arcs.append("path")
		.attr("class", "pie-slice")
		.attr("d", arc)
		.attr("fill", d => color(d.data.name))
		.append("title")
		.text(d => `${d.data.name}: ${d.data.value} kills`);

	// Add labels for larger slices
	arcs.append("text")
		.attr("class", "pie-label")
		.attr("transform", d => `translate(${labelArc.centroid(d)})`)
		.attr("text-anchor", "middle")
		.style("display", d => {
			// Only show label if slice is large enough
			const percent = (d.endAngle - d.startAngle) / (2 * Math.PI) * 100;
			return percent > 5 ? "block" : "none";
		})
		.text(d => {
			const percent = ((d.endAngle - d.startAngle) / (2 * Math.PI) * 100).toFixed(1);
			return `${percent}%`;
		});

	// Create legend
	const legend = d3.select("#weapon-legend");
	weaponData.forEach(d => {
		const item = legend.append("div")
			.attr("class", "legend-item");

		item.append("div")
			.attr("class", "legend-color")
			.style("background-color", color(d.name));

		item.append("span")
			.text(`${d.name} (${d.value})`);
	});
}
