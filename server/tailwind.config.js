// @ts-check
import { join } from 'path';

// 1. Import the Skeleton plugin
import { skeleton } from '@skeletonlabs/tw-plugin';

const colors = require('tailwindcss/colors');

/** @type {import('tailwindcss').Config} */
export default {
	// 2. Opt for dark mode to be handled via the class method
	darkMode: 'class',
	content: [
		'./src/**/*.{html,js,svelte,ts}',
		// 3. Append the path to the Skeleton package
		join(require.resolve(
			'@skeletonlabs/skeleton'),
			'../**/*.{html,js,svelte,ts}'
		)
	],
	theme: {
		extend: {
      colors: {
        'firewall-red': '#e7191f',
      },
      fontFamily: {
        DM: ["DM Sans", "sans-serif"],
        kollektif: ["Kollektif", 'sans-serif'],
      },
    },
	},
	plugins: [
		// 4. Append the Skeleton plugin (after other plugins)
		skeleton({
      themes: { preset: [ "crimson" ] }
    })
	]
}
