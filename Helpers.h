#ifndef Helpers_H
#define Helpers_H

// ITK
#include "itkVectorImage.h"

namespace Helpers
{
/** This function copies the data from 'input' to 'output' */
template<typename TImage>
void DeepCopy(const TImage* const input, TImage* const output);

template<typename TPixel>
void DeepCopy(const itk::VectorImage<TPixel, 2>* const input, itk::VectorImage<TPixel, 2>* const output);

/** Write an image to a file. */
template<typename TImage>
void WriteImage(const TImage* const input, const std::string& fileName);

}

#include "Helpers.hpp"

#endif
