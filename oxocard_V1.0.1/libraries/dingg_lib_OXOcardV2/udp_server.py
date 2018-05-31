#!/usr/bin/env python3

#  Copyright 2017 by Malte Janduda
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.


import socket
import datetime
import re
import json
import threading 

UDP_IP = "" #noip for broadcast
UDP_PORT = 1337

print("+============================+")
print("|  ESP32 UDP Logging Server  |")
print("+============================+")
print("Press enter to stop recording.")

def wait_for_input():
	global running
	the_input = input("")
	running = False

sock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
sock.bind( (UDP_IP, UDP_PORT) )
running = True

myThread = threading.Thread(target=wait_for_input)
myThread.start()

out = open('output.json', 'w');
out.write('{"frames":[');
first = True
jsonr = re.compile(".*?\sFB_JSON:\s(.*)\|.*?")

while running:
	data, addr = sock.recvfrom(2048)
	match = jsonr.search(data.decode())
	if match is None:
		continue

	for_out = match.group(1)
	try:
		parsed = json.loads(for_out)
		if len(parsed) == 64:
			if not first:
				out.write(',')
			first = False
			out.write(for_out)
			print("recorded frame.")
	except Exception: 
		pass

out.write(']}')


