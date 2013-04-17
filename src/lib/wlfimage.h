/**
 * @file wlfimage.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef WLF_IMAGE_H
#define WLF_IMAGE_H

#include <opencv2/core/core.hpp>

struct WlfParams
{
	WlfParams() : dwtLevels(2) { }

	int dwtLevels;			/// num of dwt levels 
};

cv::Mat wlfImageRead(const char* file);

void wlfImageWrite(const char* file, const cv::Mat& img, const WlfParams& params = WlfParams());

#endif // !WLF_IMAGE_H
