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

#ifndef SmallHoleFillerWidget_H
#define SmallHoleFillerWidget_H

#include "ui_SmallHoleFillerWidget.h"

// Custom
#include "Mask.h"

// ITK
#include "itkVectorImage.h"

// Qt
#include <QMainWindow>
class QGraphicsPixmapItem;

class SmallHoleFillerWidget : public QMainWindow, public Ui::SmallHoleFillerWidget
{
  Q_OBJECT
public:

  SmallHoleFillerWidget();
  SmallHoleFillerWidget(const std::string& imageFileName, const std::string& maskFileName);

  typedef itk::VectorImage<float,2> ImageType;
  
public slots:

  void on_actionOpenImage_activated();
  void on_actionSaveResult_activated();
  
  void on_btnFill_clicked();
  
  void on_chkShowInput_clicked();
  void on_chkShowOutput_clicked();
  void on_chkShowMask_clicked();

  void slot_StartProgressBar();
  void slot_StopProgressBar();
  void slot_IterationComplete();

private:

  void SharedConstructor();
    
  void showEvent ( QShowEvent * event );
  void resizeEvent ( QResizeEvent * event );

  void OpenImage(const std::string& imageFileName);
  void OpenMask(const std::string& maskFileName);
  void OpenImageAndMask(const std::string& imageFileName, const std::string& maskFileName);

  ImageType::Pointer Result;
  ImageType::Pointer Image;
  Mask::Pointer MaskImage;

  QGraphicsPixmapItem* ImagePixmapItem;
  QGraphicsPixmapItem* MaskImagePixmapItem;
  QGraphicsPixmapItem* ResultPixmapItem;
  
  QGraphicsScene* Scene;

  std::string SourceImageFileName;
  std::string MaskImageFileName;

};

#endif // SmallHoleFillerWidget_H
