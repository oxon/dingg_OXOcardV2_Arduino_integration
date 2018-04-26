#!/usr/bin/env python

import os
import re
import sys
from subprocess import check_output

if sys.version_info < (3,0):
	print("Python3 is required.")
	sys.exit(-1)

if len(sys.argv) < 3:
	print("Please provide input and output file.")
	sys.exit(-1)

css_regex = r'^.*(<link rel="stylesheet" type="text/css" href="(.*\.css)" />).*$'
js_regex = r'^.*(<script src="(.*\.js)"></script>).*$'
svg_media_regex = r'^.*(<!--\sIMG\s([^\s]+\.svg)\s-->).*$'

def ensure_dir(file_path):
	directory = os.path.dirname(file_path)
	if not os.path.exists(directory):
		os.makedirs(directory)

def whitespace(content):
	return content.replace("\t", "").replace("\n", "")

def get_css(filename):
	f = open(filename, 'r')
	output = "<style type=\"text/css\">";
	for line in f:
		output += whitespace(line)
	f.close()
	return output + "</style>"

def get_js(filename):
	import urllib.request, urllib.parse
	f = open(filename, 'r')
	js_code = f.read();
	f.close()
	data = {
		'output_format' : 'text',
		'output_info' : 'compiled_code',
		'compilation_level' : 'SIMPLE_OPTIMIZATIONS',
		'js_code' : js_code
	}
	data = bytes( urllib.parse.urlencode( data ).encode() )
	handler = urllib.request.urlopen( 'https://closure-compiler.appspot.com/compile', data );
	return "<script type=\"text/javascript\">" + handler.read().decode( 'utf-8' ) + "</script>";

def get_svg_media(filename):
	f = open(filename, 'r')
	output = ''
	for line in f:
		output += whitespace(line)
	f.close()
	return output

def compress_html(filename):
	path = os.path.dirname(filename)
	if path == '':
		path = '.'
	path = path + '/'
	f = open(filename, 'r')
	output = "";
	for line in f:
		line = whitespace(line)
		matches = re.match(css_regex, line)
		if matches is not None:
			line = line.replace(matches.group(1), get_css(path + matches.group(2)))
		matches = re.match(js_regex, line)
		if matches is not None:
			line = line.replace(matches.group(1), get_js(path + matches.group(2)))
		matches = re.match(svg_media_regex, line)
		if matches is not None:
			line = line.replace(matches.group(1), get_svg_media(path + matches.group(2)))
		output += line
	f.close()
	return output

def write_file(filename, content):
	if os.path.exists(filename):
		os.remove(filename)
	f = open(filename, 'w')
	f.write(content)
	f.flush()
	f.close()

def process_xxd(filename, content):
	hex_values = ''
	counter = 0
	for buf in content:
		hex_values += str(" 0x" + ('%02x' % buf) + ",")
		counter += 1
		if counter % 12 == 11 and counter > 0:
			hex_values += '\n '
	filename = os.path.basename(filename).replace('.', '_')
	ifdef = '_LIB_' + filename.upper() + '_H_'
	return (
		'#ifndef ' + ifdef + '\n'
		'#define ' + ifdef + ' 1\n'
		'\n'
		'// Generated via build.py\n'
		'unsigned char ' + filename + '[] = {\n'
		' ' + hex_values[:-1] + '\n'
		'};\n\n'
		'unsigned int ' + filename + '_len = ' + str(counter) + ';\n'
		'\n'
		'#endif\n'
	)


if __name__ == "__main__":
	xxd = True
	if len(sys.argv) >= 4:
		xxd = False
	
	compressed = compress_html(sys.argv[1])
	if xxd is True:
		write_file(sys.argv[2], process_xxd(sys.argv[1], compressed.encode('utf-8')))
	else:
		write_file(sys.argv[2], compressed)





