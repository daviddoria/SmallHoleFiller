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

#include "itkImage.h"

template<typename TImage>
class SmallHoleFiller
{
public:
  // Constructor
  SmallHoleFiller();

  typedef itk::Image<unsigned char, 2> MaskImageType;
  
  // Inputs
  void SetImage(typename TImage::Pointer image);

  // In this class, non-zero pixels indicate valid pixels, while zero pixels indicate pixels to be filled.
  void SetMask(MaskImageType::Pointer mask);
  
  typename MaskImageType::Pointer GetMask();

  void GenerateMaskFromImage();
  void SetHolePixel(typename TImage::PixelType pixel);
  
  // Outputs
  typename TImage::Pointer GetOutput();
  
  // This is the main loop. It simply calls Iterate() until complete.
  void Fill();
  
  // This is the core functionality.
  void Iterate();
  
  // This function returns true if any of the Output pixels match the HolePixel. This indicates there is more work to be done.
  bool HasEmptyPixels();

  void SetWriteIntermediateOutput(bool);

  bool ShouldBeFilled(unsigned char);
  bool IsValid(unsigned char);
  
private:
  // The input image.
  typename TImage::Pointer Image;

  MaskImageType::Pointer Mask;
  MaskImageType::Pointer OriginalMask;
  
  // The intermediate and eventually output image.
  typename TImage::Pointer Output;
  
  // The value of a pixel to be considered a hole (to be filled).
  typename TImage::PixelType HolePixel;
  
  bool WriteIntermediateOutput;
  
};

// This function copies the data from 'input' to 'output'
template<typename TImage>
void DeepCopy(typename TImage::Pointer input, typename TImage::Pointer output);
  
  
#include "SmallHoleFiller.hxx"

#endif
