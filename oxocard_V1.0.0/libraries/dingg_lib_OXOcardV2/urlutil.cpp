/*
 * \license LGPL-V2.1
 * Copyright (c) 2017 OXON AG. All rights reserved.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see 'http://www.gnu.org/licenses/'
 */

#include <string.h>
#include "urlutil.h"

const char *URLUtil::encode(const char *input){
	size_t countSpecialChars = 0;

	for(size_t i=0; i<strlen(input); i++){
		if(input[i] == '\0') break;
		if(URLUtil::isURIReserved(input[i])) countSpecialChars ++;
	}

	size_t encodedLength = strlen(input)+countSpecialChars*2;
	char *encoded = (char *)malloc(encodedLength + 1);

	size_t input_i = 0;
	for(size_t i=0; i<encodedLength && input_i<strlen(input); i++, input_i++){
		if(URLUtil::isURIReserved(input[input_i])){
			encoded[i] = '%'; i++;
			sprintf(encoded+i, "%02X", input[input_i]);
			i++;
		}else{
			encoded[i] = input[input_i];
		}
	}
	encoded[encodedLength] = '\0';
	return encoded;
}

int8_t URLUtil::isURIReserved(char c){
	if(c == '-' || c == '_' || c == '.' || c == '~') return 0;
	if(c>47 && c<58) return 0;
	if(c>64 && c<91) return 0;
	if(c>96 && c<123) return 0;
	return 1;
}
