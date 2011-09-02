#include "SmallHoleFiller.h"

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
  std::cout << "Input image: " << inputMaskFileName << std::endl;
  std::cout << "Output image: " << outputFileName << std::endl;

  //typedef itk::Image<unsigned char, 2> ImageType;

  typedef itk::Image<float> ImageType;

  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(inputImageFileName);
  imageReader->Update();

  typedef itk::Image<unsigned char, 2> MaskImageType;
  typedef itk::ImageFileReader<MaskImageType> MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName(inputMaskFileName);
  maskReader->Update();

  SmallHoleFiller<ImageType> smallHoleFiller;
  smallHoleFiller.SetImage(imageReader->GetOutput());
  smallHoleFiller.SetMask(maskReader->GetOutput());

  smallHoleFiller.SetWriteIntermediateOutput(true);
  smallHoleFiller.Fill();

  typedef  itk::ImageFileWriter< ImageType  > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputFileName);
  writer->SetInput(smallHoleFiller.GetOutput());
  writer->Update();

  return EXIT_SUCCESS;
}
