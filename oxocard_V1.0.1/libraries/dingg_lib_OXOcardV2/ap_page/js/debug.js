httpRequest = function(url, callback, data, failureCallback){
	failureCallback = failureCallback || callback;
	data = data || null;
	var http = new XMLHttpRequest();
	if(data)
		http.open('POST', url, true);
	else
		http.open('GET', url, true);
	http.onreadystatechange = function() {
		if(http.readyState == 4 && (http.status==200 || http.status==202) ) {
			var parsedResponse = http.responseText;
			try{
				parsedResponse = JSON.parse(parsedResponse);
			} catch(e) {
				// ignore
			}
			callback(parsedResponse);
		}else if(http.readyState == 4 && http.status > 199 && http.status < 300){
			// we can ignore...
		}else if(http.readyState == 4 ){
			failureCallback(http);
		}
	};

	var contentType = 'text/plain';
	if(typeof data === 'object'){
		data = JSON.stringify(data);
		contentType = 'application/json';
	}

	if(data){
		http.setRequestHeader('Content-type',contentType);
		http.send(data);
	}else{
		http.send();
	}

};

function reloadInfo(){
	httpRequest('/info', reloadInfoCallback);
}

function collectFormData(){
	var data = {};
	var inputs = document.querySelectorAll('form input');
	for(var i=0, l=inputs.length; i<l; i++){
		var input = inputs[i];
		if(input.getAttribute('type') == 'submit') continue;
		var key = input.getAttribute('name');
		var value = (input.value !== '') ? input.value : null;;
		if(key == 'ota_port')
			value = parseInt(value, 10);
		data[key] = value;
	}
	return data;
}

function sendForm(){
	var data = collectFormData();
	console.log(data);
	httpRequest('/savedebug', sendFormCallback, data);
	return false;
}

function sendFactoryReset(){
	var result = window.confirm('Factory resets everything. You sure?');
	if(result)
		httpRequest('/factoryreset', factoryResetCallback);
}

function factoryResetCallback(result){
	console.log(result);
}

function sendFormCallback(result){
	console.log(result);
	reloadInfo();
	if(result.ota_hostname == true && result.ota_path_prefix == true && result.ota_port == true){
		alert('Saved!');
	}else{
		alert('Some fields have not been saved!');
	}
}

function reloadInfoCallback(info){
	var html = '<thead><tr><th style="wdith:200px;">What</th>';
	html += '<th style="width:400px">Value</th></tr></thead>';
	html += '<tbody>';
	var keys = Object.keys(info);
	for(var i=0, l=keys.length; i<l; i++){
		html += '<tr><td>' + keys[i] + '</td>';
		if(keys[i] == 'id' || keys[i] == 'name'){
			html += '<td>' + info[keys[i]] + '</td>';
		}else{
			html += '<td><input type="text" name="' + keys[i] + '" value="' + info[keys[i]] + '" /></td>';
		}
		html += '</tr>';
	}
	html += '<tr><td colspan="2"><input type="submit" name="save" value="Save" /></td></tr>';
	html += '<tr><td colspan="2" style="height:200px"></tr>';
	html += '<tr><td colspan="2"><input type="button" name="factory" value="Factory Reset" style="background:#f00;" onclick="sendFactoryReset();"/></tr>';
	html += '</tbody>';
	document.getElementById('info_table').innerHTML = html;
}

function init() {
	document.getElementsByTagName('main')[0].style.width = '600px';
	reloadInfo();
}

function ready(fn) {
	if (document.attachEvent ? document.readyState === "complete" : document.readyState !== "loading") {
		fn();
	} else {
		document.addEventListener('DOMContentLoaded', fn);
	}
}

ready(init);
