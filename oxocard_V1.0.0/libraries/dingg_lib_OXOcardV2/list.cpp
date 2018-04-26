/*******************************************************************************
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
*******************************************************************************/

#include <esp_log.h>
#include "list.h"

template <typename ElementType>
size_t List<ElementType>::getSize(){
	return this->elements.size();
}

template <typename ElementType>
void List<ElementType>::add(ElementType element){
	this->elements.push_back(element);
}

template <>
rgbColor_t List<rgbColor_t>::get(size_t index){
	size_t currentIndex = 0;
	for(auto element = this->elements.begin(); element != this->elements.end(); ++element) {
		if(currentIndex == index) return (rgbColor_t)*element;
		currentIndex++;
	}
	return rgb(0,0,0);
}

template <typename ElementType>
ElementType List<ElementType>::get(size_t index){
	size_t currentIndex = 0;
	for(auto element = this->elements.begin(); element != this->elements.end(); ++element) {
		if(currentIndex == index)
			return (ElementType)*element;
		currentIndex++;
	}
	return static_cast<ElementType>(0);
}

template <typename ElementType>
int8_t List<ElementType>::remove(size_t index){
	size_t currentIndex = 0;
	for(auto element = this->elements.begin(); element != this->elements.end(); ++element) {
		if(currentIndex == index){
			this->elements.erase(element);
			return 0;
		}
		currentIndex++;
	}
	return -1;
}

// Explicit instantiation so linker finds functions
template class List<int32_t>;
template class List<rgbColor_t>;
