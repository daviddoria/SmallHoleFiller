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

#ifndef SmallHoleFiller_H
#define SmallHoleFiller_H

// Submodules
#include <Mask/Mask.h>

// ITK
#include "itkImage.h"

template<typename TImage>
class SmallHoleFiller
{
public:
  /** Constructor */
  SmallHoleFiller();

  /** Constructor */
  SmallHoleFiller(TImage* const image, Mask* const mask);

  /** Set the image to fill */
  void SetImage(TImage* const image);

  /** In this class, non-zero pixels indicate valid pixels, while zero pixels indicate pixels to be filled. */
  void SetMask(const Mask* const mask);

  /** Get the mask. */
  Mask* GetMask();

  /** Get the output. */
  TImage* GetOutput();

  /** This is the main loop. It simply calls Iterate() until complete.*/
  void Fill();

  /** This is the core functionality.*/
  void Iterate();

  /** Determine if an image should be written at each iteration. */
  void SetWriteIntermediateOutput(const bool);

  /** Set the radius of the kernel to use for averaging. */
  void SetKernelRadius(const unsigned int kernelRadius);

  /** Set the factor by which to downsample the image before filling the hole. */
  void SetDownsampleFactor(const unsigned int downsampleFactor);

private:

  /** Common functionality for all constructors. */
  void SharedConstructor();

  /** The image to fill. */
  typename TImage::Pointer OriginalImage;

  /** The downsampled image. */
  typename TImage::Pointer Image;

  /** The region in which to fill the image. */
  Mask::Pointer OriginalMaskImage;

  /** The downsampled mask. */
  Mask::Pointer MaskImage;
  
  /** The intermediate and eventually output image.*/
  typename TImage::Pointer Output;

  /** The flag determining if debug images should be written. */
  bool WriteIntermediateOutput;

  /** The current iteration. This is only used to write debug images. */
  unsigned int CurrentIteration;

  /** The radius of the kernel in which to average valid pixels. */
  unsigned int KernelRadius;

  /** The factor by which to downsample the image before filling the hole. */
  unsigned int DownsampleFactor;
};

#include "SmallHoleFiller.hpp"

#endif
