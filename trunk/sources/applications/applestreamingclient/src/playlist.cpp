/* 
 *  Copyright (c) 2010,
 *  Gavriloaie Eugen-Andrei (shiretu@gmail.com)
 *
 *  This file is part of crtmpserver.
 *  crtmpserver is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  crtmpserver is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with crtmpserver.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "playlist.h"

Playlist::Playlist() {
	Clear();
}

Playlist::~Playlist() {
	Clear();
}

IOBuffer *Playlist::GetBuffer() {
	return &_buffer;
}

void Playlist::Clear() {
	_buffer.IgnoreAll();
	_lastMediaSequence = 0;
	_pLastKeyUri = NULL;
	//_customData.Reset();
	_items.clear();
	_itemMediaSequences.clear();
	_itemKeyUris.clear();
	_itemUris.clear();
	_itemBandwidths.clear();
	_meanDuration = 0;
	_meanDurationCount = 0;
	_startTime = -1;
	_endTime = -1;
}

void Playlist::SetPlaylistUri(string playlistUri) {
	_playlistUri = playlistUri;
	string dummy;
	splitFileName(_playlistUri, _partialUri, dummy, '/');
	_partialUri += "/";
	//	ASSERT("\nplaylistUri: %s\n_partialUri: %s",
	//			STR(_playlistUri),
	//			STR(_partialUri));
}

string Playlist::GetPlaylistUri() {
	return _playlistUri;
}

void Playlist::Parse(uint32_t skipCount) {
	_lastMediaSequence = 0;
	_pLastKeyUri = NULL;

	uint8_t *pBuffer = GETIBPOINTER(_buffer);
	uint32_t bufferLength = GETAVAILABLEBYTESCOUNT(_buffer);
	uint32_t lineStart = 0;
	uint32_t lineLength = 0;
	char *pLine = NULL;
	vector<char *> item;
	uint32_t itemIndex = 0;

	for (uint32_t i = 0; i < bufferLength; i++) {
		//1. Detect the new-line
		if (pBuffer[i] != 0x0d && pBuffer[i] != 0x0a)
			continue;

		//2. Over-write it with '\0'
		pBuffer[i] = 0;

		//3. Compute the line length and save the pointer
		lineLength = i - lineStart;
		if (lineLength != 0) {
			pLine = (char *) (pBuffer + lineStart);
			//_lines.push_back(pair<char *, uint32_t > (pLine, length));
		}
		lineStart = i + 1;

		//4. Skip this line if length is 0
		if (lineLength == 0)
			continue;

		//5. This line is part of an item.
		item.push_back(pLine);
		if (pLine[0] == '#') {
			//6. Detect EXT-X-MEDIA-SEQUENCE or EXT-X-KEY
			//The string must be at least 22 bytes length
			if (lineLength >= 22) {
				if (pLine[21] == ':') {
					//7. We have a possible winner for EXT-X-MEDIA-SEQUENCE
					//now the offort of strstr is motivated
					if (strstr(pLine, "#EXT-X-MEDIA-SEQUENCE:") == pLine) {
						//8. We have it
						_lastMediaSequence = atoi(pLine + 22);
						//printf("pLine: %s; _mediaSequence: %u\n", pLine, _mediaSequence);
					}
				} else if (pLine[10] == ':') {
					//9. We have a possible winner for EXT-X-KEY
					//now the offort of strstr is motivated
					if (strstr(pLine, "#EXT-X-KEY:") == pLine) {
						//10. We have it
						//printf("pLine: %s; ", pLine);
						_pLastKeyUri = strstr(pLine, "URI=\"") + 5;
						strstr(_pLastKeyUri, "\"")[0] = 0;
						//printf("_pKeyUri: %s\n", _pKeyUri);
					}
				}
			}
		} else {
			if (skipCount == 0) {
				//11. If we get an non-comment
				//that means the item is finished. Save it
				_items[itemIndex] = item;
				_itemMediaSequences[itemIndex] = _lastMediaSequence;
				_itemKeyUris[itemIndex] = _pLastKeyUri;
				_itemUris[itemIndex] = pLine;

				//12. Increment indexes
				itemIndex++;
			} else {
				skipCount--;
			}
			_lastMediaSequence++;

			//13. Clear the current item
			item.clear();
		}
	}

	//    for (map<uint32_t, vector<char *> >::iterator i = _items.begin(); i != _items.end(); i++) {
	//        printf("--\n");
	//        for (uint32_t j = 0; j < MAP_VAL(i).size(); j++) {
	//            printf("%u - %s - %s - %s\n",
	//                    _itemMediaSequences[MAP_KEY(i)],
	//                    _itemUris[MAP_KEY(i)],
	//                    _itemKeyUris[MAP_KEY(i)],
	//                    MAP_VAL(i)[j]);
	//        }
	//    }
}

bool Playlist::ParseBandwidthInfo() {
	for (map<uint32_t, vector<char *> >::iterator i = _items.begin(); i != _items.end(); i++) {
		vector<char *> &comments = MAP_VAL(i);
		bool bandwidthFound = false;
		for (uint32_t j = 0; j < comments.size() - 1; j++) {
			char *pComment = comments[j];
			if (pComment[0] != '#')
				continue;
			char *pBandwidth = strstr(pComment, "BANDWIDTH=");
			if (pBandwidth == NULL)
				continue;
			pBandwidth += 10;
			_itemBandwidths[MAP_KEY(i)] = atoi(pBandwidth);
			bandwidthFound = true;
			break;
		}
		if (!bandwidthFound) {
			FATAL("Item number %u dowsn't have bandwidth info", MAP_KEY(i));
			return false;
		}
	}

	return true;
}

Variant &Playlist::GetCustomData() {
	return _customData;
}

uint32_t Playlist::GetItemsCount() {
	return _items.size();
}

string Playlist::GetItemUri(uint32_t index) {
	if (index >= _itemUris.size())
		return "";
	string result = _itemUris[index];
	if (result == "")
		return result;
	if (result.find("http://") == 0)
		return result;
	if (result[0] != '/')
		return _partialUri + result;
	NYIA;
	return "";
}

string Playlist::GetItemKeyUri(uint32_t index) {
	if (index >= _itemKeyUris.size())
		return "";
	if (_itemKeyUris[index] == NULL)
		return "";
	return _itemKeyUris[index];
}

uint32_t Playlist::GetItemMediaSequence(uint32_t index) {
	if (index >= _itemMediaSequences.size())
		return 0;
	return _itemMediaSequences[index];
}

uint32_t Playlist::GetItemBandwidth(uint32_t index) {
	if (index >= _itemBandwidths.size())
		return 0;
	return _itemBandwidths[index];
}

Variant Playlist::GetItemVariant(uint32_t index) {
	if (index >= _items.size())
		return Variant();
	Variant result;
	result["encryptionKeyUri"] = GetItemKeyUri(index);
	result["itemUri"] = GetItemUri(index);

	return result;
}

void Playlist::ApproximateStartEndTimes(uint32_t index, double start, double end) {
	_meanDuration = _meanDuration * (double) _meanDurationCount + end - start;
	_meanDurationCount++;
	_meanDuration /= (double) _meanDurationCount;
}

void Playlist::GetSeekRange(double &min, double &max) {
	min = 0;
	max = (double) _items.size() * _meanDuration;
}

uint32_t Playlist::GetItemIndex(double seekPoint) {
	if (_meanDuration == 0)
		return 0;
	uint32_t index = (uint32_t) (seekPoint / _meanDuration);
	if (_items.size() <= index)
		return 0;
	return index;
}
