#include "SmallHoleFiller.h"

// ITK
#include "itkCastImageFilter.h"
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
  
  typedef itk::RGBPixel<float> RGBFloatPixelType; // We must use float pixels so that the averaging operation does not overflow
  typedef itk::Image<RGBFloatPixelType> RGBFloatImageType;
  
  typedef itk::ImageFileReader<RGBFloatImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputFileName);
  reader->Update();
  
  SmallHoleFiller<RGBFloatImageType> smallHoleFiller;
  smallHoleFiller.SetImage(reader->GetOutput());
  
  RGBFloatImageType::PixelType green;
  green.SetRed(0);
  green.SetGreen(255);
  green.SetBlue(0);
  smallHoleFiller.SetHolePixel(green);
  
  smallHoleFiller.Fill();
  
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
