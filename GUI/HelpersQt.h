/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef HELPERS_QT_H
#define HELPERS_QT_H

// Qt
#include <QImage>
class QGraphicsView;
class QTableWidget;

// ITK
#include "itkImageRegion.h"

// Custom
#include "Mask.h"

namespace HelpersQt
{
// Convert a QColor to an unsigned char[3]
void QColorToUCharColor(const QColor& color, unsigned char outputColor[3]);

// Scale an image so that it fits in a QGraphicsView
QImage FitToGraphicsView(const QImage qimage, const QGraphicsView* gfx);

////////////////////////////////////
///////// Function templates (defined in HelpersQt.hxx) /////////
////////////////////////////////////

/** This function looks at the input image and determins which other HelpersQt function to call. */
template <typename TImage>
QImage GetQImage(const TImage* const image);

template <typename TImage>
QImage GetQImage(const TImage* const image, const itk::ImageRegion<2>& region);

template <typename TImage>
QImage GetQImageRGB(const TImage* const image);

template <typename TImage>
QImage GetQImageRGB(const TImage* const image, const itk::ImageRegion<2>& region);

template <typename TImage>
QImage GetQImageRGBA(const TImage* const image);

template <typename TImage>
QImage GetQImageRGBA(const TImage* const image, const itk::ImageRegion<2>& region);

template <typename TImage>
QImage GetQImageMagnitude(const TImage* const image);

template <typename TImage>
QImage GetQImageMagnitude(const TImage* const image, const itk::ImageRegion<2>& region);

template <typename TImage>
QImage GetQImageScalar(const TImage* const image);

template <typename TImage>
QImage GetQImageScalar(const TImage* const image, const itk::ImageRegion<2>& region);

template <typename TImage>
QImage GetQImageScaled(const TImage* const image);

template <typename TImage>
QImage GetQImageScaled(const TImage* const image, const itk::ImageRegion<2>& region);

template <typename TImage>
QImage GetQImageMasked(const TImage* const image, const Mask* const mask);

template <typename TImage>
QImage GetQImageMasked(const TImage* const image, const Mask* const mask, const itk::ImageRegion<2>& region);

QImage GetQMaskImage(const Mask* const mask);

QImage GetQMaskImage(const Mask* const mask, const itk::ImageRegion<2>& region);

} // end namespace

#include "HelpersQt.hpp"

#endif
