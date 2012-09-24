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
#include "Mask/MaskOperations.h"

// ITK
#include "itkImageFileWriter.h" // For intermediate debugging output
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
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
  this->OriginalImage = TImage::New();
  this->Image = TImage::New();
  
  this->Output = TImage::New();

  this->OriginalMaskImage = Mask::New();
  this->MaskImage = Mask::New();

  this->WriteIntermediateOutput = false;

  this->KernelRadius = 2;

  this->DownsampleFactor = 1;
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetImage(TImage* const image)
{
  ITKHelpers::DeepCopy(image, this->OriginalImage.GetPointer());
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetMask(const Mask* const mask)
{
  this->OriginalMaskImage->DeepCopyFrom(mask);
}

template <typename TImage>
TImage* SmallHoleFiller<TImage>::GetOutput()
{
  return this->Output;
}

template <typename TImage>
void SmallHoleFiller<TImage>::Fill()
{
  // Downsample the image
  itk::Size<2> destinationSize;
  destinationSize[0] = this->OriginalImage->GetLargestPossibleRegion().GetSize()[0] / this->DownsampleFactor;
  destinationSize[1] = this->OriginalImage->GetLargestPossibleRegion().GetSize()[1] / this->DownsampleFactor;

  ITKHelpers::ScaleImage(this->OriginalImage.GetPointer(), destinationSize, this->Image.GetPointer());
  ITKHelpers::WriteRGBImage(this->Image.GetPointer(), "Downsampled.png");

  ITKHelpers::ScaleImage(this->OriginalMaskImage.GetPointer(), destinationSize, this->MaskImage.GetPointer());
  this->MaskImage->CopyInformationFrom(this->OriginalMaskImage);
  ITKHelpers::WriteImage(this->MaskImage.GetPointer(), "DownsampledMask.png");

  this->CurrentIteration = 0;
  // Initialize by setting the output image to the input image.
  ITKHelpers::DeepCopy(this->Image.GetPointer(), this->Output.GetPointer());

  while(this->MaskImage->HasHolePixels())
    {
    std::cout << "Iteration " << this->CurrentIteration << "..." << std::endl;
    Iterate();

    if(this->WriteIntermediateOutput)
      {
      std::stringstream ssMHA;
      ssMHA << "intermediate_" << this->CurrentIteration << ".mha";
      ITKHelpers::WriteImage(this->Output.GetPointer(), ssMHA.str());

      std::stringstream ssPNG;
      ssPNG << "intermediate_" << this->CurrentIteration << ".png";
      ITKHelpers::WriteRGBImage(this->Output.GetPointer(), ssPNG.str());

      std::stringstream ssMask;
      ssMask << "intermediateMask_" << this->CurrentIteration << ".png";
      ITKHelpers::WriteImage(this->MaskImage.GetPointer(), ssMask.str());
      }
    this->CurrentIteration++;
    }

  ITKHelpers::WriteRGBImage(this->Output.GetPointer(), "Output.png");

  typename TImage::Pointer tempOutput = TImage::New();
  ITKHelpers::ScaleImage(this->Output.GetPointer(), this->OriginalImage->GetLargestPossibleRegion().GetSize(),
                         tempOutput.GetPointer());
  ITKHelpers::DeepCopy(this->OriginalImage.GetPointer(), this->Output.GetPointer());
  MaskOperations::CopyInHoleRegion(tempOutput.GetPointer(), this->Output.GetPointer(), this->OriginalMaskImage);

  std::cout << "Filling completed in " << this->CurrentIteration << " iterations." << std::endl;
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
  previousMask->DeepCopyFrom(this->MaskImage.GetPointer());

  typename TImage::Pointer currentImage = TImage::New();
  ITKHelpers::DeepCopy(this->Output.GetPointer(), currentImage.GetPointer());

  // Compute the hole-side of the hole boundary
  Mask::BoundaryImageType::Pointer boundaryImage = Mask::BoundaryImageType::New();
  previousMask->CreateBoundaryImage(boundaryImage, Mask::HOLE);

//   std::stringstream ssBoundary;
//   ssBoundary << "boundary_" << this->CurrentIteration << ".png";
//   ITKHelpers::WriteImage(boundaryImage.GetPointer(), ssBoundary.str());

  // Traverse the boundary image. When a pixel is encountered that is a boundary pixel,
  // fill it with the average of its valid neighbors.
  // Do not mark pixels filled on this iteration as known.
  // This would result in a less accurate filling, as it favors the colors
  // of pixels that occur earlier in the raster scan order.

  itk::ImageRegionIteratorWithIndex<Mask::BoundaryImageType>
            boundaryImageIterator(boundaryImage, boundaryImage->GetLargestPossibleRegion());

  while(!boundaryImageIterator.IsAtEnd())
    {
    if(boundaryImageIterator.Get()) // We have encountered a boundary pixel
      {
      itk::Index<2> currentIndex = boundaryImageIterator.GetIndex();

      // There are valid neighbors, so fill the output pixel and mark the mask pixel as filled.
      if(previousMask->HasValidNeighbor(currentIndex))
        {
        itk::ImageRegion<2> surroundingRegion =
               ITKHelpers::GetRegionInRadiusAroundPixel(currentIndex, this->KernelRadius);

        std::vector<itk::Index<2> > validIndicesInRegion = previousMask->GetValidPixelsInRegion(surroundingRegion);

        std::vector<typename TImage::PixelType> validPixelsInRegion =
                ITKHelpers::GetPixelValues(currentImage.GetPointer(), validIndicesInRegion);

        typename TImage::PixelType neighborhoodAverage =
                Statistics::Average(validPixelsInRegion);
        if(neighborhoodAverage[0] > 255)
        {
          std::cout << "There are " << validPixelsInRegion.size() << " validPixelsInRegion." << std::endl;
          for(unsigned int i = 0; i < validPixelsInRegion.size(); ++i)
          {
            std::cout << validPixelsInRegion[i] << std::endl;
          }
          std::cout << "neighborhoodAverage: " << neighborhoodAverage << std::endl;
          throw;
        }

        this->Output->SetPixel(currentIndex, neighborhoodAverage);

        // Mark the pixel as filled
        this->MaskImage->MarkAsValid(currentIndex);

        //std::cout << "Set " << outputIterator.GetIndex() << " to " << pixelAverage << std::endl;
        }
      } // end is boundary

    ++boundaryImageIterator;
    }// end main traversal loop

  if(this->WriteIntermediateOutput)
    {
    std::stringstream ssPNG;
    ssPNG << "iteration_" << this->CurrentIteration << ".png";
    ITKHelpers::WriteRGBImage(this->Output.GetPointer(), ssPNG.str());
    }

}

template <typename TImage>
Mask* SmallHoleFiller<TImage>::GetMask()
{
  return this->MaskImage;
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetKernelRadius(const unsigned int kernelRadius)
{
  if(kernelRadius < 1)
  {
    throw std::runtime_error("Kernel radius must be >= 1 !");
  }
  this->KernelRadius = kernelRadius;
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetDownsampleFactor(const unsigned int downsampleFactor)
{
  if(downsampleFactor < 1)
  {
    throw std::runtime_error("downsampleFactor must be >= 1 !");
  }
  this->DownsampleFactor = downsampleFactor;
}

#endif
