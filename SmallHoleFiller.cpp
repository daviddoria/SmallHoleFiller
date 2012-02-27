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

// This executable will fill all channels of a multi-channel image separately

#include "SmallHoleFiller.h"

// ITK
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToVectorImageFilter.h"
#include "itkVectorImage.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

std::string GetExtension(const std::string& filename);

typedef itk::Image<unsigned char, 2> MaskImageType;

template <typename TVectorImage, typename TScalarImage>
void Fill(const std::string& inputFilename, const std::string& outputFilename, MaskImageType::Pointer mask);

int main (int argc, char *argv[])
{
  // Verify arguments
  if(argc != 4)
    {
    std::cerr << "Required arguments: InputImageFileName MaskFileName OutputFileName" << std::endl;
    return EXIT_FAILURE;
    }

  // Parse arguments
  std::string inputFileName = argv[1];
  std::string maskFileName = argv[2];
  std::string outputFileName = argv[3];

  std::string extension = GetExtension(inputFileName);
  std::cout << "File extension: " << extension << std::endl;
  
  // Output arguments
  std::cout << "Input image: " << inputFileName << std::endl;
  std::cout << "Mask image: " << maskFileName << std::endl;
  std::cout << "Output image: " << outputFileName << std::endl;
  std::cout << "Extension: " << extension << std::endl;

  typedef itk::ImageFileReader<MaskImageType> MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName(maskFileName);
  maskReader->Update();
  
  typedef itk::VectorImage<float, 2> FloatVectorImageType;
  typedef itk::Image<float, 2> FloatScalarImageType;
  typedef itk::VectorImage<unsigned char, 2> UnsignedCharVectorImageType;
  typedef itk::Image<unsigned char, 2> UnsignedCharScalarImageType;

  if(extension.compare("png") == 0)
    {
    Fill<UnsignedCharVectorImageType, UnsignedCharScalarImageType>(inputFileName, outputFileName, maskReader->GetOutput());
    }
  else
    {
    Fill<FloatVectorImageType, FloatScalarImageType>(inputFileName, outputFileName, maskReader->GetOutput());
    }

  return EXIT_SUCCESS;
}

std::vector<std::string> split(const std::string &s, char delim) 
{
  std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}
 
std::string GetExtension(const std::string& filename)
{
  std::vector<std::string> words = split(filename, '.');
  return words[words.size()-1];
}

template <typename TVectorImage, typename TScalarImage>
void Fill(const std::string& inputFileName, const std::string& outputFileName, MaskImageType::Pointer mask)
{
  
  typedef itk::ImageFileReader<TVectorImage> ImageReaderType;
  typename ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(inputFileName);
  imageReader->Update();
  
  typedef itk::VectorIndexSelectionCastImageFilter<TVectorImage, TScalarImage> DisassemblerType;
  
  typedef itk::ImageToVectorImageFilter<TScalarImage> ReassemblerType;
  typename ReassemblerType::Pointer reassembler = ReassemblerType::New();
  
  // Perform the filling on each channel independently
  //std::vector<PoissonEditing<TScalarImage> > poissonFilters;//(imageReader->GetOutput()->GetNumberOfComponentsPerPixel());
    
  for(unsigned int component = 0; component < imageReader->GetOutput()->GetNumberOfComponentsPerPixel(); component++)
    {
    std::cout << "Component " << component << std::endl;
    // Disassemble the image into its components
    typename DisassemblerType::Pointer sourceDisassembler = DisassemblerType::New();
    sourceDisassembler->SetIndex(component);
    sourceDisassembler->SetInput(imageReader->GetOutput());
    sourceDisassembler->Update();
  
    SmallHoleFiller<TScalarImage> smallHoleFiller;
    smallHoleFiller.SetImage(sourceDisassembler->GetOutput());
    smallHoleFiller.SetMask(mask);
    smallHoleFiller.Fill();

    reassembler->SetNthInput(component, smallHoleFiller.GetOutput());
    /*
    // Write this channel just for testing:
    std::cout << "Writing test image..." << std::endl;
    typedef  itk::ImageFileWriter< FloatScalarImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();
    std::stringstream ss;
    ss << "test_" << component << ".mhd";
    writer->SetFileName(ss.str());
    writer->SetInput(poissonFilters[component].GetOutput());
    writer->Update();*/
    }
  
  reassembler->Update();
  

  typedef  itk::ImageFileWriter< TVectorImage  > WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputFileName);
  writer->SetInput(reassembler->GetOutput());
  writer->Update();
}
