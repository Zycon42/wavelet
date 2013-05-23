/**
 * @file arithmcodec.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "arithmcodec.h"

void StaticDataModel::computeCumulativeFreqs(const std::vector<unsigned>& freqs) {
	cumulativeFreqs.resize(freqs.size());
	
	for (size_t i = 0; i < cumulativeFreqs.size(); ++i) {
		cumulativeFreqs[i] = i > 0 ? (cumulativeFreqs[i - 1] + freqs[i]) : freqs[i];
		// handle overflow
		if (cumulativeFreqs[i] > MAX_FREQ) {
			// divide all frequencies bigger than 1 by two and compute new cumulative frequencies
			auto newFreqs = freqs;
			for (auto& f : newFreqs) {
				if (f > 1)
					f /= 2;
			}
			computeCumulativeFreqs(newFreqs);
			break;
		}
	}
}
