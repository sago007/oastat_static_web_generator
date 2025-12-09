/**
 * Player Name Color Codes
 * Handles Quake 3 Arena style color codes in player names
 * Color codes are in the format ^N where N is 0-9
 */

// Quake 3 color code mapping
const Q3_COLORS = {
	'0': '#000000',  // Black
	'1': '#FF0000',  // Red
	'2': '#00FF00',  // Green
	'3': '#FFFF00',  // Yellow
	'4': '#0000FF',  // Blue
	'5': '#00FFFF',  // Cyan
	'6': '#FF00FF',  // Magenta
	'7': '#FFFFFF',  // White
	'8': '#FF8800',  // Orange
	'9': '#888888'   // Gray
};

/**
 * Parse a player name and convert color codes to HTML spans
 * @param {string} name - The raw player name with color codes
 * @returns {string} HTML string with colored spans
 */
function parsePlayerNameColors(name) {
	if (!name) return '';

	// Regular expression to match ^N color codes
	const colorCodeRegex = /\^([0-9])/g;
	let result = '';
	let lastIndex = 0;
	let currentColor = null;
	let match;

	while ((match = colorCodeRegex.exec(name)) !== null) {
		// Add text before this color code
		const textBefore = name.substring(lastIndex, match.index);
		if (textBefore) {
			if (currentColor) {
				const style = getColorStyle(currentColor);
				result += `<span style="${style}">${escapeHtml(textBefore)}</span>`;
			} else {
				result += escapeHtml(textBefore);
			}
		}

		// Set new color
		const colorCode = match[1];
		currentColor = Q3_COLORS[colorCode] || null;

		lastIndex = match.index + match[0].length;
	}

	// Add remaining text
	const remainingText = name.substring(lastIndex);
	if (remainingText) {
		if (currentColor) {
			const style = getColorStyle(currentColor);
			result += `<span style="${style}">${escapeHtml(remainingText)}</span>`;
		} else {
			result += escapeHtml(remainingText);
		}
	}

	return result || escapeHtml(name);
}

/**
 * Get the style string for a color, adding black background for white text
 * @param {string} color - The color hex code
 * @returns {string} CSS style string
 */
function getColorStyle(color) {
	// Add black background for white text to make it visible
	if (color === '#FFFFFF') {
		return `color: ${color}; background-color: #000000; padding: 0 2px;`;
	}
	return `color: ${color}`;
}

/**
 * Escape HTML special characters
 * @param {string} text - Text to escape
 * @returns {string} Escaped text
 */
function escapeHtml(text) {
	const div = document.createElement('div');
	div.textContent = text;
	return div.innerHTML;
}

/**
 * Apply color codes to all player names in tables
 */
function applyPlayerNameColors() {
	// Find all table cells that likely contain player names
	const tables = document.querySelectorAll('table');

	tables.forEach(table => {
		// Look for tables with a "Name" header
		const headers = table.querySelectorAll('th');
		let nameColumnIndex = -1;

		headers.forEach((header, index) => {
			if (header.textContent.trim() === 'Name' || header.textContent.trim() === 'Player') {
				nameColumnIndex = index;
			}
		});

		// If we found a Name column, process those cells
		if (nameColumnIndex !== -1) {
			const rows = table.querySelectorAll('tr');
			rows.forEach(row => {
				const cells = row.querySelectorAll('td');
				if (cells[nameColumnIndex]) {
					const cell = cells[nameColumnIndex];

					// Store original text if not already stored
					if (!cell.hasAttribute('data-original-text')) {
						cell.setAttribute('data-original-text', cell.textContent);
					}

					const originalText = cell.getAttribute('data-original-text');

					// Only process if the name contains color codes
					if (originalText.includes('^')) {
						const coloredHtml = parsePlayerNameColors(originalText);
						cell.innerHTML = coloredHtml;
					}
				}
			});
		}
	});

	// Also process legend items if they exist
	const legendItems = document.querySelectorAll('.legend-item span');
	legendItems.forEach(item => {
		// Store original text if not already stored
		if (!item.hasAttribute('data-original-text')) {
			item.setAttribute('data-original-text', item.textContent);
		}

		const originalText = item.getAttribute('data-original-text');
		if (originalText.includes('^')) {
			// Extract the name part (before the score in parentheses if present)
			const match = originalText.match(/^(.+?)(\s*\(\d+\))?$/);
			if (match) {
				const namePart = match[1];
				const scorePart = match[2] || '';
				const coloredName = parsePlayerNameColors(namePart);
				item.innerHTML = coloredName + escapeHtml(scorePart);
			}
		}
	});
}

/**
 * Remove color codes from all player names (restore plain text)
 */
function removePlayerNameColors() {
	// Find all table cells that likely contain player names
	const tables = document.querySelectorAll('table');

	tables.forEach(table => {
		const headers = table.querySelectorAll('th');
		let nameColumnIndex = -1;

		headers.forEach((header, index) => {
			if (header.textContent.trim() === 'Name' || header.textContent.trim() === 'Player') {
				nameColumnIndex = index;
			}
		});

		if (nameColumnIndex !== -1) {
			const rows = table.querySelectorAll('tr');
			rows.forEach(row => {
				const cells = row.querySelectorAll('td');
				if (cells[nameColumnIndex]) {
					const cell = cells[nameColumnIndex];
					// Restore to plain text (strip color codes but keep original text)
					if (cell.hasAttribute('data-original-text')) {
						const originalText = cell.getAttribute('data-original-text');
						// Remove color codes for display
						cell.textContent = originalText.replace(/\^[0-9]/g, '');
					} else {
						// Fallback: just get text content
						cell.textContent = cell.textContent;
					}
				}
			});
		}
	});

	// Also restore legend items
	const legendItems = document.querySelectorAll('.legend-item span');
	legendItems.forEach(item => {
		if (item.hasAttribute('data-original-text')) {
			const originalText = item.getAttribute('data-original-text');
			// Remove color codes for display
			item.textContent = originalText.replace(/\^[0-9]/g, '');
		} else {
			// Fallback: just get text content
			item.textContent = item.textContent;
		}
	});
}

/**
 * Toggle player name colors on/off
 */
function togglePlayerNameColors() {
	const enabled = localStorage.getItem('playerNameColorsEnabled') === 'true';

	if (enabled) {
		removePlayerNameColors();
		localStorage.setItem('playerNameColorsEnabled', 'false');
	} else {
		applyPlayerNameColors();
		localStorage.setItem('playerNameColorsEnabled', 'true');
	}

	// Update toggle button state if it exists
	updateToggleButtonState();
}

/**
 * Update the toggle button state to match current setting
 */
function updateToggleButtonState() {
	const toggle = document.getElementById('player-color-toggle');
	if (toggle) {
		const enabled = localStorage.getItem('playerNameColorsEnabled') === 'true';
		toggle.checked = enabled;
	}
}

/**
 * Initialize player name coloring on page load
 */
function initializePlayerNameColors() {
	// Check if colors should be enabled (default to false)
	const enabled = localStorage.getItem('playerNameColorsEnabled') === 'true';

	if (enabled) {
		applyPlayerNameColors();
	}

	// Update toggle button state
	updateToggleButtonState();

	// Set up toggle button event listener
	const toggle = document.getElementById('player-color-toggle');
	if (toggle) {
		toggle.addEventListener('change', togglePlayerNameColors);
	}
}

// Initialize when DOM is ready
if (document.readyState === 'loading') {
	document.addEventListener('DOMContentLoaded', initializePlayerNameColors);
} else {
	initializePlayerNameColors();
}
