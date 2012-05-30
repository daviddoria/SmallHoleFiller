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

#include "SmallHoleFillerWidget.h"

// Custom
#include "SmallHoleFiller.h"

// Submodules
#include "ITKQtHelpers/ITKQtHelpers.h"

// ITK
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// Qt
#include <QIcon>
#include <QFileDialog>
#include <QGraphicsPixmapItem>

// Called by all constructors
void SmallHoleFillerWidget::SharedConstructor()
{
  std::cout << "SmallHoleFillerWidget::SharedConstructor()" << std::endl;
  this->setupUi(this);

  this->progressBar->setMinimum(0);
  this->progressBar->setMaximum(0);
  this->progressBar->hide();

  this->Image = ImageType::New();
  this->MaskImage = Mask::New();
  this->Result = ImageType::New();

  this->Scene = new QGraphicsScene;
  this->graphicsView->setScene(this->Scene);

  this->ImagePixmapItem = NULL;
  this->MaskImagePixmapItem = NULL;
  this->ResultPixmapItem = NULL;
}

// Default constructor
SmallHoleFillerWidget::SmallHoleFillerWidget()
{
  std::cout << "PoissonEditingWidget()" << std::endl;
  SharedConstructor();
};

SmallHoleFillerWidget::SmallHoleFillerWidget(const std::string& imageFileName, const std::string& maskFileName)
{
  std::cout << "PoissonEditingWidget(string, string)" << std::endl;
  SharedConstructor();
  this->SourceImageFileName = imageFileName;
  this->MaskImageFileName = maskFileName;
  OpenImageAndMask(this->SourceImageFileName, this->MaskImageFileName);
}

void SmallHoleFillerWidget::showEvent(QShowEvent* event)
{
  if(this->ImagePixmapItem)
    {
    this->graphicsView->fitInView(this->ImagePixmapItem, Qt::KeepAspectRatio);
    }
}

void SmallHoleFillerWidget::resizeEvent(QResizeEvent* event)
{
  if(this->ImagePixmapItem)
    {
    this->graphicsView->fitInView(this->ImagePixmapItem, Qt::KeepAspectRatio);
    }
}

void SmallHoleFillerWidget::on_btnFill_clicked()
{
  SmallHoleFiller<ImageType> smallHoleFiller(Image.GetPointer(), MaskImage.GetPointer());
  smallHoleFiller.Fill();

  ITKHelpers::DeepCopy(smallHoleFiller.GetOutput(), Result.GetPointer());

  ITKHelpers::WriteImage(Result.GetPointer(), "result.mha");

  QImage qimageImage = ITKQtHelpers::GetQImageColor(this->Result.GetPointer());
  this->ResultPixmapItem = this->Scene->addPixmap(QPixmap::fromImage(qimageImage));
  this->graphicsView->fitInView(this->ResultPixmapItem, Qt::KeepAspectRatio);
  this->ResultPixmapItem->setVisible(this->chkShowOutput->isChecked());
}

void SmallHoleFillerWidget::on_actionSaveResult_activated()
{
  // Get a filename to save
  QString fileName = QFileDialog::getSaveFileName(this, "Save File", ".", "Image Files (*.jpg *.jpeg *.bmp *.png *.mha)");

  if(fileName.toStdString().empty())
    {
    std::cout << "Filename was empty." << std::endl;
    return;
    }

  ITKHelpers::WriteImage(this->Result.GetPointer(), fileName.toStdString());
  //Helpers::WriteRGBImage<ImageType>(this->Result, fileName.toStdString() + ".png");
  this->statusBar()->showMessage("Saved result.");
}

void SmallHoleFillerWidget::OpenImage(const std::string& imageFileName)
{
  // Load and display image
  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(imageFileName);
  imageReader->Update();

  ITKHelpers::DeepCopy(imageReader->GetOutput(), this->Image.GetPointer());

  QImage qimageImage = ITKQtHelpers::GetQImageColor(this->Image.GetPointer());
  this->ImagePixmapItem = this->Scene->addPixmap(QPixmap::fromImage(qimageImage));
  this->graphicsView->fitInView(this->ImagePixmapItem);
  this->ImagePixmapItem->setVisible(this->chkShowInput->isChecked());
}

void SmallHoleFillerWidget::OpenMask(const std::string& maskFileName)
{
  // Load and display mask
  typedef itk::ImageFileReader<Mask> MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName(maskFileName);
  maskReader->Update();

  ITKHelpers::DeepCopy(maskReader->GetOutput(), this->MaskImage.GetPointer());

  QImage qimageMask = ITKQtHelpers::GetQImageScalar(this->MaskImage.GetPointer());
  this->MaskImagePixmapItem = this->Scene->addPixmap(QPixmap::fromImage(qimageMask));
  this->MaskImagePixmapItem->setVisible(this->chkShowMask->isChecked());
}

void SmallHoleFillerWidget::OpenImageAndMask(const std::string& imageFileName, const std::string& maskFileName)
{
  // Load and display image
  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer imageReader = ImageReaderType::New();
  imageReader->SetFileName(imageFileName);
  imageReader->Update();

  ITKHelpers::DeepCopy(imageReader->GetOutput(), this->Image.GetPointer());

  QImage qimageImage = ITKQtHelpers::GetQImageColor(this->Image.GetPointer());
  this->ImagePixmapItem = this->Scene->addPixmap(QPixmap::fromImage(qimageImage));
  this->graphicsView->fitInView(this->ImagePixmapItem);
  this->ImagePixmapItem->setVisible(this->chkShowInput->isChecked());

  // Load and display mask
  typedef itk::ImageFileReader<Mask> MaskReaderType;
  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName(maskFileName);
  maskReader->Update();

  ITKHelpers::DeepCopy(maskReader->GetOutput(), this->MaskImage.GetPointer());

  QImage qimageMask = ITKQtHelpers::GetQImageScalar(this->MaskImage.GetPointer());
  this->MaskImagePixmapItem = this->Scene->addPixmap(QPixmap::fromImage(qimageMask));
  this->MaskImagePixmapItem->setVisible(this->chkShowMask->isChecked());
}

void SmallHoleFillerWidget::on_actionOpenMask_activated()
{
  std::cout << "on_actionOpenMask_activated" << std::endl;

  QString filename = QFileDialog::getOpenFileName(this, "Open Mask", "", "Mask Files (*.mha)");

  if(filename.isEmpty())
    {
    return;
    }

  OpenMask(filename.toStdString());
}

void SmallHoleFillerWidget::on_actionOpenImage_activated()
{
  std::cout << "on_actionOpenImage_activated" << std::endl;

  QString filename = QFileDialog::getOpenFileName(this, "Open Image", "", "Image Files (*.mha)");

  if(filename.isEmpty())
    {
    return;
    }

  OpenImage(filename.toStdString());
}

void SmallHoleFillerWidget::on_chkShowInput_clicked()
{
  if(!this->ImagePixmapItem)
    {
    return;
    }
  this->ImagePixmapItem->setVisible(this->chkShowInput->isChecked());
}

void SmallHoleFillerWidget::on_chkShowOutput_clicked()
{
  if(!this->ResultPixmapItem)
    {
    return;
    }
  this->ResultPixmapItem->setVisible(this->chkShowOutput->isChecked());
}

void SmallHoleFillerWidget::on_chkShowMask_clicked()
{
  if(!this->MaskImagePixmapItem)
    {
    return;
    }
  this->MaskImagePixmapItem->setVisible(this->chkShowMask->isChecked());
}

void SmallHoleFillerWidget::slot_StartProgressBar()
{
  this->progressBar->show();
}

void SmallHoleFillerWidget::slot_StopProgressBar()
{
  this->progressBar->hide();
}

void SmallHoleFillerWidget::slot_IterationComplete()
{
  QImage qimage = ITKQtHelpers::GetQImageColor(this->Result.GetPointer());
  this->ResultPixmapItem = this->Scene->addPixmap(QPixmap::fromImage(qimage));
  this->ResultPixmapItem->setVisible(this->chkShowOutput->isChecked());
}
