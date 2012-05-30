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

#include "SmallHoleFiller.h"

// Custom
#include "Mask/Mask.h"

// ITK
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int main (int argc, char *argv[])
{
  // Verify arguments
  if(argc != 4)
    {
    std::cerr << "Required arguments: InputImageFileName MaskFileName OutputFileName" << std::endl;
    return EXIT_FAILURE;
    }

  // Parse arguments
  std::string inputImageFileName = argv[1];
  std::string inputMaskFileName = argv[2];
  std::string outputFileName = argv[3];

  // Output arguments
  std::cout << "Input image: " << inputImageFileName << std::endl;
  std::cout << "Input mask: " << inputMaskFileName << std::endl;
  std::cout << "Output image: " << outputFileName << std::endl;

  // Always use float pixels internally
  typedef itk::VectorImage<float, 2> InternalImageType;

  // Read the image
  typedef itk::ImageFileReader<InternalImageType> ImageReaderType;
  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(inputImageFileName);
  imageReader->Update();

  // Read the mask
  Mask::Pointer mask = Mask::New();
  mask->Read(inputMaskFileName);

  SmallHoleFiller<InternalImageType> smallHoleFiller(imageReader->GetOutput(), mask);
  //smallHoleFiller.SetWriteIntermediateOutput(true);
  smallHoleFiller.Fill();

  itk::ImageIOBase::IOComponentType pixelType = ITKHelpers::GetPixelTypeFromFile(inputImageFileName);

  if(pixelType == itk::ImageIOBase::UCHAR)
  {
    ITKHelpers::WriteRGBImage(smallHoleFiller.GetOutput(), outputFileName);
  }
  else
  {
    ITKHelpers::WriteImage(smallHoleFiller.GetOutput(), outputFileName);
  }

  return EXIT_SUCCESS;
}