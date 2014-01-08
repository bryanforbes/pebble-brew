/* global Pebble */

// This gets replaced during build
var config_html = '__CONFIG_HTML__';

function mix(destination, source) {
	for (var key in source) {
		if (source.hasOwnProperty(key)) {
			destination[key] = source[key];
		}
	}

	return destination;
}
var config = mix(Object.create(null), {
	backgroundColor: 0,
	handStrokeColor: 1,
	handFillColor: 0
});

function getStorageInt(key) {
	var value = localStorage.getItem(key);
	if (value != null) {
		return parseInt(value, 10);
	}
	return value;
}

Pebble.addEventListener('ready', function (e) {
	console.log('Ready!');

	for (var key in config) {
		var value = getStorageInt(key);
		if (value != null) {
			config[key] = value;
		}
	}
});

Pebble.addEventListener('appmessage', function (e) {
	if (e.payload.config) {
		Pebble.sendAppMessage(config);
	}
});

Pebble.addEventListener('showConfiguration', function (e) {
	console.log('showConfigurationEvent');

	var html = config_html.replace('__CONFIG__', JSON.stringify(config));
	Pebble.openURL('data:text/html,' + encodeURIComponent(html + '<!--.html'));
});

Pebble.addEventListener('webviewclosed', function (e) {
	var configuration;
	try {
		configuration = JSON.parse(e.response);
		console.log(configuration);
	}
	catch (e) {
		return;
	}

	Pebble.sendAppMessage(mix(config, configuration));

	for (var key in config) {
		localStorage.setItem(key, config[key]);
	}
});
