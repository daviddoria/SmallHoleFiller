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

#ifndef SmallHoleFiller_HPP
#define SmallHoleFiller_HPP

#include "SmallHoleFiller.h" // Appease syntax parser

// Submodules
#include "Mask/ITKHelpers/ITKHelpers.h"

// ITK
#include "itkImageFileWriter.h" // For intermediate debugging output
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkNumericTraits.h"

template <typename TImage>
SmallHoleFiller<TImage>::SmallHoleFiller()
{
  SharedConstructor();
}

template <typename TImage>
SmallHoleFiller<TImage>::SmallHoleFiller(TImage* const image, Mask* const mask)
{
  SharedConstructor();
  SetImage(image);
  SetMask(mask);
}

template <typename TImage>
void SmallHoleFiller<TImage>::SharedConstructor()
{
  //this->HolePixel = itk::NumericTraits< typename TImage::PixelType >::Zero;
  this->Image = NULL;
  this->Output = NULL;

//   this->Mask = NULL;
//   this->OriginalMask = NULL;
  this->MaskImage = Mask::New();
  this->OriginalMask = Mask::New();

  this->WriteIntermediateOutput = false;
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetImage(TImage* const image)
{
  this->Image = image;
  this->Output = TImage::New();
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetMask(const Mask* const mask)
{
  ITKHelpers::DeepCopy(mask, this->OriginalMask.GetPointer());
  //Helpers::DeepCopy(mask, this->MaskImage.GetPointer());
  this->MaskImage->DeepCopyFrom(mask);
}


template <typename TImage>
TImage* SmallHoleFiller<TImage>::GetOutput()
{
  return this->Output;
}

template <typename TImage>
void SmallHoleFiller<TImage>::Fill()
{
  if(!this->Image)
    {
    throw std::runtime_error("No image provided!");
    }

  // Initialize by setting the output image to the input image.
  ITKHelpers::DeepCopy(this->Image.GetPointer(), this->Output.GetPointer());
  unsigned int numberOfIterations = 0;
  while(HasEmptyPixels())
    {
    std::cout << "Iteration " << numberOfIterations << "..." << std::endl;
    Iterate();
    numberOfIterations++;
  
    if(this->WriteIntermediateOutput)
      {
      typedef itk::ImageFileWriter<TImage> WriterType;
      typename WriterType::Pointer writer = WriterType::New();
      std::stringstream ss;
      ss << "intermediate_" << numberOfIterations << ".mha";
      writer->SetFileName(ss.str());
      writer->SetInput(this->Output);
      writer->Update();

      typedef  itk::ImageFileWriter<Mask> MaskWriterType;
      typename MaskWriterType::Pointer maskWriter = MaskWriterType::New();
      std::stringstream ssMask;
      ssMask << "intermediateMask_" << numberOfIterations << ".png";
      maskWriter->SetFileName(ssMask.str());
      maskWriter->SetInput(this->MaskImage);
      maskWriter->Update();
      ssMask << ".mha";
      maskWriter->SetFileName(ssMask.str());
      maskWriter->Update();
      }
    }

  std::cout << "Filling completed in " << numberOfIterations << " iterations." << std::endl;
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetWriteIntermediateOutput(bool writeIntermediateOutput)
{
  this->WriteIntermediateOutput = writeIntermediateOutput;
}

template <typename TImage>
void SmallHoleFiller<TImage>::Iterate()
{
  // Make a copy of the current mask. We use this to determine which pixels were holes
  // at the beginning of this iteration.
  Mask::Pointer previousMask = Mask::New();
  //Helpers::DeepCopy(this->MaskImage.GetPointer(), previousMask.GetPointer());
  previousMask->DeepCopyFrom(this->MaskImage.GetPointer());
  
  // Traverse the image. When a pixel is encountered that is a hole, fill it with the average of its non-hole neighbors.
  // Do not mark pixels filled on this iteration as known. This will result in a less accurate filling, as it favors the colors
  // of pixels that occur earlier in the raster scan order.

  itk::Size<2> radius;
  radius.Fill(1);
  
  itk::NeighborhoodIterator<Mask> previousMaskNeighborhoodIterator(radius,
                                                                            previousMask, previousMask->GetLargestPossibleRegion());
  
  // This iterator is used to get neighbor values and set the output pixels.
  itk::NeighborhoodIterator<TImage> outputNeighborhoodIterator(radius, this->Output,
                                                               this->Output->GetLargestPossibleRegion());

  // This iterator is used to update the mask.
  itk::ImageRegionIterator<Mask> maskIterator(this->MaskImage, this->MaskImage->GetLargestPossibleRegion());
    
  while(!previousMaskNeighborhoodIterator.IsAtEnd())
    {
    if(MaskImage->IsHoleValue(previousMaskNeighborhoodIterator.GetCenterPixel()))
      {
      // Set the initial sum to zero
      typedef typename itk::NumericTraits<typename TImage::PixelType>::FloatType FloatPixelType;
      FloatPixelType pixelSum =
             itk::NumericTraits<FloatPixelType>::ZeroValue(
             Image->GetPixel(previousMaskNeighborhoodIterator.GetIndex()));

      // Loop over the 8-neighorhood
      unsigned int numberOfValidPixels = 0;
      for(unsigned int i = 0; i < 9; i++)
	{
	if(i == 4) // this is the center (current) pixel, so skip it
	  {
	  continue;
	  }
	bool isInBounds = false;
	Mask::PixelType currentPixelValidity = previousMaskNeighborhoodIterator.GetPixel(i, isInBounds);
	if(isInBounds)
	  {
	  if(MaskImage->IsValidValue(currentPixelValidity))
	    {
	    numberOfValidPixels++;
	    pixelSum += outputNeighborhoodIterator.GetPixel(i);
	    }
	  }
	} // end 8-connected neighbor for

      // There were valid neighbors, so fill the output pixel and mark the mask pixel as filled.
      if(numberOfValidPixels > 0)
        {
        //typename TImage::PixelType pixelAverage = static_cast<typename TImage::PixelType>(pixelSum / validPixels);
        // Multiply by the reciprocal because operator/ is not defined for all types.
        typename TImage::PixelType pixelAverage =
          static_cast<typename TImage::PixelType>(pixelSum * (1.0/ static_cast<float>(numberOfValidPixels)));

        outputNeighborhoodIterator.SetCenterPixel(pixelAverage);

        // Mark the pixel as filled
        MaskImage->MarkAsValid(maskIterator.GetIndex());

	//std::cout << "Set " << outputIterator.GetIndex() << " to " << pixelAverage << std::endl;
	}
      } // end "fill this pixel?" conditional

    ++previousMaskNeighborhoodIterator;
    ++outputNeighborhoodIterator;
    ++maskIterator;
    } // end main traversal loop
}

template <typename TImage>
bool SmallHoleFiller<TImage>::HasEmptyPixels()
{
  itk::ImageRegionConstIteratorWithIndex<Mask> maskIterator(this->MaskImage,
                                                            this->MaskImage->GetLargestPossibleRegion());
 
  while(!maskIterator.IsAtEnd())
    {
    if(MaskImage->IsHole(maskIterator.GetIndex()))
      {
      return true;
      }
    ++maskIterator;
    }
  return false;
}

template <typename TImage>
Mask* SmallHoleFiller<TImage>::GetMask()
{
  return this->MaskImage;
}

template <typename TImage>
void SmallHoleFiller<TImage>::GenerateMaskFromImage(const typename TImage::PixelType& holePixel)
{
  this->MaskImage->SetRegions(this->Image->GetLargestPossibleRegion());
  this->MaskImage->Allocate();
  this->MaskImage->FillBuffer(0);
  
  itk::ImageRegionConstIterator<TImage> imageIterator(this->Image, this->Image->GetLargestPossibleRegion());
  itk::ImageRegionIterator<Mask> maskIterator(this->MaskImage, this->MaskImage->GetLargestPossibleRegion());

  while(!imageIterator.IsAtEnd())
    {
    if(imageIterator.Get() == holePixel)
      {
      maskIterator.Set(MaskImage->GetHoleValue());
      }
    else
      {
      maskIterator.Set(MaskImage->GetValidValue());
      }
    ++imageIterator;
    ++maskIterator;
    }
}

#endif
