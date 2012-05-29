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

  // We must use float pixels so that the averaging operation does not overflow
  typedef itk::RGBPixel<float> RGBFloatPixelType; 
  typedef itk::Image<RGBFloatPixelType> RGBFloatImageType;

  typedef itk::ImageFileReader<RGBFloatImageType> ImageReaderType;
  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(inputImageFileName);
  imageReader->Update();

  //typedef itk::Image<unsigned char, 2> MaskImageType;
  //typedef Mask MaskImageType;
  typedef itk::ImageFileReader<Mask> MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName(inputMaskFileName);
  maskReader->Update();

  std::cout << "There are " << maskReader->GetOutput()->CountHolePixels() << " holes." << std::endl;
  std::cout << "There are " << maskReader->GetOutput()->CountValidPixels() << " valid pixels." << std::endl;

//   SmallHoleFiller<RGBFloatImageType> smallHoleFiller;
//   smallHoleFiller.SetImage(imageReader->GetOutput());
//   smallHoleFiller.SetMask(maskReader->GetOutput());

  SmallHoleFiller<RGBFloatImageType> smallHoleFiller(imageReader->GetOutput(), maskReader->GetOutput());
  smallHoleFiller.Fill();

  {
  typedef  itk::ImageFileWriter<RGBFloatImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName("float.mha");
  writer->SetInput(smallHoleFiller.GetOutput());
  writer->Update();
  }

  typedef itk::RGBPixel<unsigned char> RGBUCharPixelType;
  typedef itk::Image<RGBUCharPixelType> RGBUCharImageType;
  typedef itk::CastImageFilter<RGBFloatImageType, RGBUCharImageType> CastFilterType;
  CastFilterType::Pointer castFilter = CastFilterType::New();
  castFilter->SetInput(smallHoleFiller.GetOutput());
  castFilter->Update();

  typedef  itk::ImageFileWriter< RGBUCharImageType  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputFileName);
  writer->SetInput(castFilter->GetOutput());
  writer->Update();

  return EXIT_SUCCESS;
}
