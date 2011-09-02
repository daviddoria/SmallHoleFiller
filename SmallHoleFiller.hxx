#include "itkImageFileWriter.h" // For intermediate debugging output
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkNumericTraits.h"

template <typename TImage>
SmallHoleFiller<TImage>::SmallHoleFiller()
{
  //this->HolePixel = itk::NumericTraits< typename TImage::PixelType >::Zero;
  this->Image = NULL;
  this->Output = NULL;
  this->Mask = NULL;
  this->WriteIntermediateOutput = false;
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetImage(typename TImage::Pointer image)
{
  this->Image = image;
  this->Output = TImage::New();
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetMask(MaskImageType::Pointer mask)
{
  this->Mask = mask;
}

template <typename TImage>
void SmallHoleFiller<TImage>::SetHolePixel(typename TImage::PixelType pixel)
{
  this->HolePixel = pixel;
}

template <typename TImage>
typename TImage::Pointer SmallHoleFiller<TImage>::GetOutput()
{
  return this->Output;
}

template <typename TImage>
void SmallHoleFiller<TImage>::Fill()
{
  if(!this->Image)
    {
    std::cerr << "You must first call SetImage!" << std::endl;
    return;
    }

  // Initialize by setting the output image to the input image.
  DeepCopy<TImage>(this->Image, this->Output);
  unsigned int numberOfIterations = 0;
  while(HasEmptyPixels())
    {
    std::cout << "Iteration " << numberOfIterations << "..." << std::endl;
    Iterate();
    numberOfIterations++;
  
    if(this->WriteIntermediateOutput)
      {
      typedef  itk::ImageFileWriter< TImage  > WriterType;
      typename WriterType::Pointer writer = WriterType::New();
      std::stringstream ss;
      ss << "intermediate_" << numberOfIterations << ".mha";
      writer->SetFileName(ss.str());
      writer->SetInput(this->Output);
      writer->Update();

      typedef  itk::ImageFileWriter< MaskImageType > MaskWriterType;
      typename MaskWriterType::Pointer maskWriter = MaskWriterType::New();
      std::stringstream ssMask;
      ssMask << "intermediateMask_" << numberOfIterations << ".png";
      maskWriter->SetFileName(ssMask.str());
      maskWriter->SetInput(this->Mask);
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
  // Make a copy of the current mask. We use this to determine which pixels were holes at the beginning of this iteration.
  MaskImageType::Pointer previousMask = MaskImageType::New();
  DeepCopy<MaskImageType>(this->Mask, previousMask);
  
  // Traverse the image. When a pixel is encountered that is a hole, fill it with the average of its non-hole neighbors.
  // Do not mark pixels filled on this iteration as known. This will result in a less accurate filling, as it favors the colors
  // of pixels that occur earlier in the raster scan order.

  itk::Size<2> radius;
  radius.Fill(1);
  
  itk::NeighborhoodIterator<MaskImageType> previousMaskNeighborhoodIterator(radius, previousMask, previousMask->GetLargestPossibleRegion());
  
  // This iterator is used to get neighbor values and set the output pixels.
  itk::NeighborhoodIterator<TImage> outputNeighborhoodIterator(radius, this->Output, this->Output->GetLargestPossibleRegion());

  // This iterator is used to update the mask.
  itk::ImageRegionIterator<MaskImageType> maskIterator(this->Mask, this->Mask->GetLargestPossibleRegion());
    
  while(!previousMaskNeighborhoodIterator.IsAtEnd())
    {
    if(ShouldBeFilled(previousMaskNeighborhoodIterator.GetCenterPixel()))
      {
      typename TImage::PixelType pixelSum = itk::NumericTraits< typename TImage::PixelType >::Zero;
      // Loop over the 8-neighorhood
      unsigned int validPixels = 0;
      for(unsigned int i = 0; i < 9; i++)
	{
	if(i == 4) // this is the center (current) pixel, so skip it
	  {
	  continue;
	  }
	bool isInBounds = false;
	MaskImageType::PixelType currentPixelValidity = previousMaskNeighborhoodIterator.GetPixel(i, isInBounds);
	if(isInBounds)
	  {
	  if(IsValid(currentPixelValidity))
	    {
	    validPixels++;
	    pixelSum += outputNeighborhoodIterator.GetPixel(i);
	    }
	  }
	} // end 8-connected neighbor for

      // There were valid neighbors, so fill the output pixel and mark the mask pixel as filled.
      if(validPixels > 0)
	{
	//typename TImage::PixelType pixelAverage = static_cast<typename TImage::PixelType>(pixelSum / validPixels);
	typename TImage::PixelType pixelAverage = static_cast<typename TImage::PixelType>(pixelSum * (1.0/ validPixels)); // We multiply by the reciprocal because operator/ is not defined for all types.
	outputNeighborhoodIterator.SetCenterPixel(pixelAverage);
        maskIterator.Set(255); // Mark the pixel as filled
	//std::cout << "Set " << outputIterator.GetIndex() << " to " << pixelAverage << std::endl;
	}
      } // end "fill this pixel?" conditional
      
    ++previousMaskNeighborhoodIterator;
    ++outputNeighborhoodIterator;
    ++maskIterator;
    } // end main traversal loop
    
}

template <typename TImage>
bool SmallHoleFiller<TImage>::ShouldBeFilled(unsigned char value)
{
  if(value == 0)
    {
    return true;
    }

  return false;
}

template <typename TImage>
bool SmallHoleFiller<TImage>::IsValid(unsigned char value)
{
  return !ShouldBeFilled(value);
}

template <typename TImage>
bool SmallHoleFiller<TImage>::HasEmptyPixels()
{
  itk::ImageRegionConstIterator<MaskImageType> maskIterator(this->Mask, this->Mask->GetLargestPossibleRegion());
 
  while(!maskIterator.IsAtEnd())
    {
    if(ShouldBeFilled(maskIterator.Get())) // Mask pixel is non-zero
      {
      return true;
      }
    ++maskIterator;
    }
  return false;
}

template <typename TImage>
void SmallHoleFiller<TImage>::GenerateMaskFromImage()
{
  this->Mask->SetRegions(this->Image->GetLargestPossibleRegion());
  this->Mask->Allocate();
  this->Mask->FillBuffer(0);
  
  itk::ImageRegionConstIterator<TImage> imageIterator(this->Image, this->Image->GetLargestPossibleRegion());
  itk::ImageRegionIterator<MaskImageType> maskIterator(this->Mask, this->Mask->GetLargestPossibleRegion());

  while(!imageIterator.IsAtEnd())
    {
    if(imageIterator.Get() == this->HolePixel)
      {
      maskIterator.Set(0);
      }
    ++imageIterator;
    ++maskIterator;
    }
}

///////// This is not a member function /////////////
/** Copy the input to the output*/
template<typename TImage>
void DeepCopy(typename TImage::Pointer input, typename TImage::Pointer output)
{
  output->SetRegions(input->GetLargestPossibleRegion());
  output->Allocate();

  itk::ImageRegionConstIterator<TImage> inputIterator(input, input->GetLargestPossibleRegion());
  itk::ImageRegionIterator<TImage> outputIterator(output, output->GetLargestPossibleRegion());

  while(!inputIterator.IsAtEnd())
    {
    outputIterator.Set(inputIterator.Get());
    ++inputIterator;
    ++outputIterator;
    }
}
