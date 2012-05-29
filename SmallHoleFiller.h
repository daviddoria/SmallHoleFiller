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
#include "Mask/Mask.h"

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

  Mask* GetMask();

  // Outputs
  TImage* GetOutput();

  /** This is the main loop. It simply calls Iterate() until complete.*/
  void Fill();

  /** This is the core functionality.*/
  void Iterate();

  void SetWriteIntermediateOutput(const bool);

private:

  void SharedConstructor();

  // The input image.
  typename TImage::Pointer Image;

  Mask::Pointer MaskImage;

  /** The intermediate and eventually output image.*/
  typename TImage::Pointer Output;

  bool WriteIntermediateOutput;

  unsigned int CurrentIteration;

};

#include "SmallHoleFiller.hpp"

#endif
