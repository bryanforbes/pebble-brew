/* global Pebble */
function mix(destination, source) {
	for (var key in source) {
		if (source.hasOwnProperty(key)) {
			destination[key] = source[key];
		}
	}

	return destination;
}
var text,
	config = mix(Object.create(null), {
		text: 'farts'
	});

Pebble.addEventListener('ready', function (e) {
	console.log('Ready!');

	text = localStorage.getItem('text');
	if (text) {
		config.text = text;
	}

	Pebble.sendAppMessage(config);
});

Pebble.addEventListener('showConfiguration', function (e) {
	console.log('showConfigurationEvent');

	Pebble.openURL('http://www.reigndropsfall.net/drop/farts-config.html?config=' + encodeURIComponent(JSON.stringify(config)));
});

Pebble.addEventListener('webviewclosed', function (e) {
	var configuration;
	try {
		configuration = JSON.parse(e.response);
	}
	catch (e) {
		return;
	}

	Pebble.sendAppMessage(mix(config, configuration));

	for (var key in config) {
		localStorage.setItem(key, config[key]);
	}
});
