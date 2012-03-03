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

/*
 * This class is a subclass of itkImage that provides the concept of "valid" pixels
 * and "hole" pixels. Pixels that are any other value are never used in computations.
 */

#ifndef MASK_H
#define MASK_H

// ITK
#include "itkImage.h"
#include "itkImageRegionIterator.h"

class Mask : public itk::Image< unsigned char, 2>
{
public:
  /** Standard typedefs. */
  typedef Mask                       Self;
  typedef itk::Image< unsigned char, 2> Superclass;
  typedef itk::SmartPointer< Self >              Pointer;
  typedef itk::SmartPointer< const Self >        ConstPointer;
  typedef itk::WeakPointer< const Self >         ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(Mask, Image);

  /** Dimension of the image. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension);

  /** Types derived from the Superclass */
  typedef typename Superclass::IndexType IndexType;

  typedef typename Superclass::IOPixelType IOPixelType;

  /** Tyepdef for the functor used to access a neighborhood of pixel
  * pointers. */
  typedef itk::NeighborhoodAccessorFunctor< Self >
  NeighborhoodAccessorFunctorType;
  
  /** Return the NeighborhoodAccessor functor. This method is called by the
   * neighborhood iterators. */
  NeighborhoodAccessorFunctorType GetNeighborhoodAccessor()
  { return NeighborhoodAccessorFunctorType(); }

  /** Return the NeighborhoodAccessor functor. This method is called by the
   * neighborhood iterators. */
  const NeighborhoodAccessorFunctorType GetNeighborhoodAccessor() const
  { return NeighborhoodAccessorFunctorType(); }

  bool IsHole(const itk::Index<2>& index) const;
  bool IsHoleValue(const unsigned char value) const;

  bool IsValid(const itk::ImageRegion<2>& region) const;
  bool IsValid(const itk::Index<2>& index) const;
  bool IsValidValue(const unsigned char value) const;

  void Invert();

  void Cleanup();

  unsigned int CountHoles();

  unsigned int CountValidPixels();
  
  void ExpandHole();

  void MarkAsValid(const itk::Index<2>& pixel);
  void MarkAsHole(const itk::Index<2>& pixel);
  
  void SetHoleValue(const unsigned char value);

  void SetValidValue(const unsigned char value);

  unsigned char GetHoleValue() const;

  unsigned char GetValidValue() const;

  void OutputMembers() const;

  void DeepCopyFrom(const Mask* const inputMask);

  template<typename TImage, typename TColor>
  void ApplyToImage(const typename TImage::Pointer image, const TColor& color);

  template<typename TImage, typename TColor>
  void ApplyToVectorImage(const typename TImage::Pointer image, const TColor& color);

  std::vector<itk::Index<2> > GetValidPixelsInRegion(const itk::ImageRegion<2>& region);
  std::vector<itk::Index<2> > GetHolePixelsInRegion(const itk::ImageRegion<2>& region);

protected:
  Mask();

  unsigned char HoleValue; // Pixels with this value will be filled.
  unsigned char ValidValue; // Pixels with this value will not be filled - they are the source region.

private:

  Mask(const Self &);    //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};

#include "Mask.hpp"

#endif
