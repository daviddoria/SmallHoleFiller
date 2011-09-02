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
