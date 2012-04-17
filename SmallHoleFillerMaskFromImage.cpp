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

// The idea of this executable is to generate a mask based on pixel values in the image
// The implementation is incomplete - do not use!
#include "SmallHoleFiller.h"

// ITK
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int main (int argc, char *argv[])
{
  // Verify arguments
  if(argc != 3)
    {
    std::cerr << "Required arguments: InputFileName OutputFileName" << std::endl;
    return EXIT_FAILURE;
    }

  // Parse arguments
  std::string inputFileName = argv[1];
  std::string outputFileName = argv[2];

  // Output arguments
  std::cout << "Input image: " << inputFileName << std::endl;
  std::cout << "Output image: " << outputFileName << std::endl;
  
  //typedef itk::Image<unsigned char, 2> ImageType;
  
//   typedef itk::VariableLengthVector<float> PixelType;
//     
//   SmallHoleFiller<PixelType> smallHoleFiller;
//   smallHoleFiller.SetImage(reader->GetOutput());
//   
//   PixelType pixel;
//   pixel.SetSize(4);
//   pixel[0] = 0;
//   pixel[1] = 0;
//   pixel[2] = 0;
//   
//   smallHoleFiller.SetHolePixel(pixel);
// 
//   smallHoleFiller.GenerateMaskFromImage();
//   
//   typedef  itk::ImageFileWriter<Mask> WriterType;
//   WriterType::Pointer writer = WriterType::New();
//   writer->SetFileName(outputFileName);
//   writer->SetInput(smallHoleFiller.GetMask());
//   writer->Update();
    
  return EXIT_SUCCESS;
}
