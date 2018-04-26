var userLang = 'en';
var wifiList = [];
var errorMessage = null;
var successMessage = null;

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

function translate(key){
	var translations = {
		'de' : {
			'please_select'   : 'Bitte wählen...',
			'save_success'    : 'Deine Änderungen wurden erfolgreich gespeichert.',
			'save_failure'    : 'Deine Änderungen konnten nicht gespeichert werden. Probiere es nochmals.',
			'manual_wifi_name': 'Mein WiFi fehlt',
			'delete_success'  : 'Dein WiFi wurde erfolgreich gelöscht.',
			'delete_failure'  : 'Dein Wifi konnte nicht gelöscht werden. Probiere es nochmals.',
			'delete_confirm'  : 'Bist du sicher?',
			'testing_wifi'    : 'Verbindet mit {{NAME}}',
			'please_wait'     : 'Bitte warte',
			'connect_failure' : 'Leider hat das Verbinden nicht geklappt. Stimmt das Passwort?'
		},
		'en' : {
			'please_select'   : 'Please select...',
			'save_success'    : 'Your changes were saved successfully.',
			'save_failure'    : 'Something went wrong. Please try again.',
			'manual_wifi_name': 'My WiFi isn\'t listed',
			'delete_success'  : 'Your WiFi has been deleted.',
			'delete_failure'  : 'Your WiFi couldn\'t be deleted. Please try again.',
			'delete_confirm'  : 'Are you sure?',
			'testing_wifi'    : 'Connecting to {{NAME}}',
			'please_wait'     : 'Please wait',
			'connect_failure' : 'Sadly we coudln\'t connect. Is the password correct?'
		},
	}
	if(!(userLang in translations)) return '';
	if(!(key in translations[userLang])) return '';
	return translations[userLang][key];
}

function switchLanguage() {
	var languages = {
		"de" : document.querySelectorAll('.de'),
		"en" : document.querySelectorAll('.en')
	};
	
	userLang = window.navigator.language || window.navigator.userLanguage;
	userLang = userLang.substr(0,2);
	
	// fix to german for now.
	userLang = 'de';
	
	// default language is de for now
	if(!(userLang in languages))
		userLang = 'de';
	
	for(lang in languages){
		var display = ((lang == userLang) ? 'block' : 'none');
		for(var i=0, l=languages[lang].length; i<l; i++){
			languages[lang][i].style.display = display;
		}
	}
	
}

function initWifiList(list) {
	if (!('availableWifis' in list) || list.availableWifis.length === 0) {
		document.getElementById('wifiList').style.display = 'none';
		document.getElementById('wifi_manual_input').style.display = 'block';
	} else {
		
		var wifiList = list.availableWifis;
		var select = document.getElementById('wifiList');

		var dplist = document.querySelectorAll('.md-select__dropdown > ul')[0];
		dplist.innerHTML = '<li class="label selected">' + translate('please_select') + '</li>';
		var li = document.createElement('li');
		li.dataset.value = '_-_-manual-_-_';
		li.innerHTML = translate('manual_wifi_name');
		dplist.appendChild(li);
		
		for (var i = 0; i < wifiList.length; i++) {
			var li = document.createElement('li');
			li.dataset.value = wifiList[i];
			li.innerHTML = wifiList[i];
			dplist.appendChild(li);
		}
	}
	initDropdown();
	if(!('storedWifis' in list) || list.storedWifis.length === 0){
		document.getElementById('delete_wifi_wrapper').style.display = 'none';
		return;
	}
	document.getElementById('delete_wifi_wrapper').style.display = 'block';
	var elist = document.getElementById('delete_wifi_list');
	elist.innerHTML = '';
	for (var i = 0; i < list.storedWifis.length; i++) {
		var li = document.createElement('li');
		var dbutton = document.createElement('button');
		dbutton.dataset.wifiIndex = i;
		dbutton.innerHTML = 'X';
		dbutton.addEventListener('click', deleteWifi);
		li.appendChild(dbutton);
		var name = document.createElement('span');
		name.innerHTML = list.storedWifis[i];
		li.appendChild(name);
		elist.appendChild(li);
	}
}

function deleteWifi(evt){
	var el = evt.srcElement || evt.originalTarget;
	var data = {'wifiIndex': parseInt(el.dataset.wifiIndex)};
	if (confirm(translate('delete_confirm'))) {
		httpRequest('/delete', sendFormCallback, data);
	}
}

function deleteWifiCallback(response){
	reloadInfo();
	if(data.success == true){
		setMessage(true, 'delete_success');
	}else{
		setMessage(false, 'delete_failure');
	}
	setTimeout(resetMessages, 7000);
}

function initDropdown() {
	var existingCloned = document.getElementById('wifiList_cloned_list');
	var chevronIcon = document.getElementById('wifiList_chevron');
	if(existingCloned !== null){
		existingCloned.parentNode.removeChild(existingCloned);
		chevronIcon.parentNode.removeChild(chevronIcon);
	}
	var dropdown = document.getElementsByClassName('md-select__dropdown')[0];
	var input = dropdown.querySelectorAll('input')[0];
	var list = dropdown.querySelectorAll('ul')[0];
	var mdList = list.cloneNode(true);
	addClass(mdList, 'md-select__dropdown-drop');
	mdList.id = 'wifiList_cloned_list';
	var icon = document.createElement('i');
	addClass(icon, 'chevron');
	icon.id = 'wifiList_chevron';

	dropdown.appendChild(icon);
	dropdown.appendChild(mdList);

	dropdown.addEventListener('click', function () {
		addClass(mdList, 'show')
	});

	document.getElementById('wifi_manual_input').style.display = 'none';
	document.getElementById('wifiSelection').style.display = 'block';

	var lis = mdList.querySelectorAll('li');
	for (var i = 0; i < lis.length; i++) {
		lis[i].addEventListener('click', function (evt) {
		evt.stopPropagation();

		if (!hasClass(this, 'label')) {
			input.value = this.dataset.value;
			if(input.value == '_-_-manual-_-_'){
				document.getElementById('wifi_manual_input').style.display = 'block';
				document.getElementById('wifiSelection').style.display = 'none';
				return;
			}

			mirrorLis = list.querySelectorAll('li');
			for (var i = 0; i < mirrorLis.length; i++) {
				removeClass(mirrorLis[i], 'selected')
			}

			lis = mdList.querySelectorAll('li');
			for (i = 0; i < lis.length; i++) {
				removeClass(lis[i], 'selected')
			}
			addClass(this, 'selected');


			mirrorLi = list.querySelectorAll("[data-value='" + this.dataset.value + "']")[0];
			if (mirrorLi !== undefined) {
				addClass(mirrorLi, 'selected')
			} else {
				var label = document.querySelectorAll('#wifiList .label')[0];
				addClass(label, 'selected');
			}
			}
		})
	}

	document.addEventListener('mouseup', function () {
		removeClass(document.getElementsByClassName("md-select__dropdown-drop")[0], "show");
	});
}


function setMessage(success, translationKeys){
	var message = '';
	if(!Array.isArray(translationKeys)){
		message = translate(translationKeys);
	}else{
		message = '<ul>';
		for(var i=0, l=translationsKeys.length; i<l; i++)
			message += '<li>' + translate(translationsKeys[i]) + '</li>';
		message += '</ul>';
	}
	if(message == '') message = translationKeys;
	
	var element = null;
	if(success){
		element = document.getElementById('positive_message');
		hideElement = document.getElementById('negative_message');
		setMascot('positive');
	}else{
		element = document.getElementById('negative_message');
		hideElement = document.getElementById('positive_message');
		setMascot('negative');
	}
	if(!element || !hideElement) return;
	element.innerHTML = message;
	element.style.display = 'block';
	hideElement.style.display = 'none';
	document.body.scrollTop = document.documentElement.scrollTop = 0;
}

function resetMessages(){
	document.getElementById('positive_message').style.display = 'none';
	document.getElementById('negative_message').style.display = 'none';
	setMascot('positive');
}

function setMascot(state){
	var allowedStates = ['positive', 'negative'];
	for(var i=0, l=allowedStates.length; i<l; i++){
		var mascot = document.querySelectorAll('.mascot > svg.' + allowedStates[i])[0];
		if(!mascot) continue;
		mascot.style.display = (allowedStates[i] == state) ? 'block' : 'none';
	}
}

function addClass(el, className) {
	if (el.classList)
		el.classList.add(className);
	else
		el.className += ' ' + className;
}

function removeClass(el, className) {
	if (el.classList)
		el.classList.remove(className);
	else
		el.className = el.className.replace(new RegExp('(^|\\b)' + className.split(' ').join('|') + '(\\b|$)', 'gi'), ' ');
}

function hasClass(el, className) {
	if (el.classList)
		el.classList.contains(className);
	else
		new RegExp('(^| )' + className + '( |$)', 'gi').test(el.className);
}

function collectFormData(){
	var data = {};
	var inputs = document.querySelectorAll('form input');
	for(var i=0, l=inputs.length; i<l; i++){
		var input = inputs[i];
		if(input.getAttribute('type') == 'submit') continue;
		data[input.getAttribute('name')] = (input.value !== '' && input.value != '_-_-manual-_-_') ? input.value : null;
	}
	return data;
}

function sendForm(){
	var data = collectFormData();
	httpRequest('/save', sendFormCallback, data);
	return false;
}

function sendFormCallback(data){
	reloadInfo();
	document.querySelector('input[name=wifi_password]').value = '';
	if(data.success == true){
		setMessage(true, 'save_success');
	}else{
		setMessage(false, 'save_failure');
	}
	setTimeout(resetMessages, 7000);
}

function testWifiConfiguration(){
	var data = collectFormData();
	var wifiName = data.wifi_name_list;
	if(wifiName == '_-_-manual-_-_')
		wifiName = data.wifi_name_manual;
	
	// We just want to update the name
	if(wifiName == null){
		sendForm();
		return false;
	}
	
	var title = translate('testing_wifi').replace('{{NAME}}', wifiName);
	var content = '<div class="ball-pulse"><div></div><div></div><div></div></div>';
	content += 'Bitte warte';
	showPopup(title, content);
	httpRequest(
		'connectivity',
		testWifiConfigurationCallback,
		{'ssid': wifiName, 'password': data.wifi_password}
	);
	return false;
}

function testWifiConfigurationCallback(data){
	hidePopup();
	if(data.success == true){
		sendForm();
	}else{
		setMessage(false, 'connect_failure');
		setTimeout(resetMessages, 7000);
	}
}

function showPopup(title, content){
	document.getElementById('popup_header').innerHTML = title;
	document.getElementById('popup_content').innerHTML = content;
	addClass(document.getElementById('popup_wrapper'), 'active');
	addClass(document.getElementById('popup_background'), 'active');
}

function hidePopup(){
	removeClass(document.getElementById('popup_wrapper'), 'active');
	removeClass(document.getElementById('popup_background'), 'active');
}

function initDeviceInfo(data){
	var name = document.querySelector('form input[name=oxocard_name]');
	name.value = data.name;
	
	var idElement = document.getElementById('oxocard_id');
	idElement.innerHTML = 'ID: ' + data.id;
}

function reloadInfo(){
	httpRequest('/wifilist', initWifiList);
	httpRequest('/info', initDeviceInfo);
}

function init() {
	setMascot('positive');
	switchLanguage();
	reloadInfo();
}

ready(init);

function ready(fn) {
	if (document.attachEvent ? document.readyState === "complete" : document.readyState !== "loading") {
		fn();
	} else {
		document.addEventListener('DOMContentLoaded', fn);
	}
}
