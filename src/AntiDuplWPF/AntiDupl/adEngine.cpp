/*
* AntiDupl Dynamic-Link Library.
*
* Copyright (c) 2002-2015 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy 
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
* copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in 
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "adInit.h"
#include "adStatus.h"
#include "adImageInfo.h"
#include "adOptions.h"
#include "adResult.h"
#include "adImageDataStorage.h"
#include "adThreadManagement.h"
#include "adSearcher.h"
#include "adEngine.h"
#include "adPerformance.h"
#include "adLogger.h"
#include "adFileUtils.h"

#include "adImage.h"
#include "adDctHistogramPeaks.h"

#include <thread>
#include <mutex>


namespace ad
{
    TEngine::TEngine()
    {
#ifdef AD_LOGGER_ENABLE
        TLogger::s_logger.SetFileOut(TString(GetApplicationDirectory() + TEXT("\\AntiDupl.log")).c_str(), true);
#endif//AD_LOGGER_ENABLE
        m_pInit = new TInit();
        m_pOptions = new TOptions();
        m_pStatus = new TStatus();
        m_pImageDataStorage = new TImageDataStorage(this);
        m_pResult = new TResultStorage(this);
        m_pImageDataPtrs = new TImageDataPtrs();
        m_pCriticalSection = new TCriticalSection();
        m_pCompareManager = new TCompareManager(this);
        m_pCollectManager = new TCollectManager(this, m_pCompareManager);
        m_pSearcher = new TSearcher(this, m_pImageDataPtrs);
		m_pDctHistogramPeakManager = new TDctHistogramPeakManager(this);
    }

    TEngine::~TEngine()
    {
        delete m_pImageDataStorage;
        delete m_pResult;
        delete m_pImageDataPtrs;
        delete m_pCriticalSection;
        delete m_pInit;
        delete m_pCompareManager;
        delete m_pCollectManager;
		delete m_pDctHistogramPeakManager;
        delete m_pSearcher;
        delete m_pStatus;
        delete m_pOptions;
#ifdef AD_LOGGER_ENABLE
#ifdef AD_PERFORMANCE_TEST
        TLogger::s_logger.SetThreadIdAnnotation(false);
        AD_LOG(TPerformanceMeasurerStorage::s_storage.Statistic());
#endif//AD_PERFORMANCE_TEST
        TLogger::s_logger.ResetOut();
#endif//AD_LOGGER_ENABLE
    }

    void TEngine::Search()
    {
        AD_FUNCTION_PERFORMANCE_TEST
        m_pStatus->ClearStatistic();
        m_pStatus->SetProgress(0, 0);
        m_pResult->Clear();

		//заполняет m_pImageDataPtrs
        m_pSearcher->SearchImages();

        if(m_pOptions->compare.checkOnEquality)
        {
            m_pCompareManager->Start(m_pImageDataPtrs->size());
            m_pCompareManager->SetPriority(THREAD_PRIORITY_LOWEST);
        }
        m_pCollectManager->Start();
        m_pCollectManager->SetPriority(THREAD_PRIORITY_BELOW_NORMAL);

        size_t current = 0, total = m_pImageDataPtrs->size(); 
        for(TImageDataPtrs::iterator it = m_pImageDataPtrs->begin(); 
            it != m_pImageDataPtrs->end() && !m_pStatus->Stopped(); ++it, ++current)
        {
            TImageDataPtr pImageData = *it;
            m_pCollectManager->Add(pImageData);
            m_pStatus->SetProgress(current, total);
        }
        m_pCollectManager->Finish();

        if(m_pOptions->compare.checkOnEquality)
        {
            m_pStatus->SetProgress(current, total);
            m_pStatus->Wait(AD_THREAD_TYPE_MAIN, 0);
            m_pCompareManager->SetPriority(THREAD_PRIORITY_NORMAL);
            m_pCompareManager->Finish();
        }

        m_pImageDataPtrs->clear();
        m_pStatus->Reset();
    }

	/*void TEngine::CalculateHistogramPeaks(adImageInfoW* pPointArray, int size)
    {
		m_pStatus->ClearStatistic();
        m_pStatus->SetProgress(0, 0);

		m_pDctHistogramPeakManager->Start(size);
        m_pDctHistogramPeakManager->SetPriority(THREAD_PRIORITY_BELOW_NORMAL);

		for (size_t i = 0; i < size; i++)
		{
	 	   //m_pDctHistogramPeakManager->Add(pPointArray[i]);
			m_pStatus->SetProgress(i, size);
			//m_pStatus->Process(AD_THREAD_TYPE_COMPARE, Queue()->Id(), pImageData->path.Original().c_str());

			TImageDataPtr imageDataPtr = m_pImageDataStorage->Get(TFileInfo(pPointArray[i].path));
			//if (imageDataPtr->jpegPeaks == 0)
			{
				  m_pDctHistogramPeakManager->Add(imageDataPtr);

				//TImage *pImage = TImage::Load(imageDataPtr->hGlobal);
				//if(pImage)
				//{
				//	//imageDataPtr->height = (TUInt32)pImage->View()->height; 
				//	//imageDataPtr->width = (TUInt32)pImage->View()->width;
				//	//imageDataPtr->type = (TImageType)pImage->Format();

				//	imageDataPtr->jpegPeaks = TDctHistogramPeaks::CalcDctHistogramPeaks(*pImage->View());
				//}
				//m_pImageDataPtrs->push_back(imageDataPtr);
			}
		}

		 m_pDctHistogramPeakManager->Finish();
	 }*/

	void TEngine::CalculateHistogramPeaks(adImageInfoW* pPointArray, int size,  WorkProgressInteropNegotiator & negotiator)
	{
		m_pStatus->ClearStatistic();
        m_pStatus->SetProgress(0, 0);

		size_t progress = 0;

		size_t nthreads = std::thread::hardware_concurrency();
		//size_t nthreads = 1;
		std::vector<std::thread> threads(nthreads);
		std::mutex mutex;
		std::unique_lock<std::mutex> guard(mutex);
		guard.unlock();
		std::mutex progressMutex;
		std::unique_lock<std::mutex> progressGuard(progressMutex);
		progressGuard.unlock();

		TDataCollector* m_pDataCollector = new TDataCollector(this);
		
		const size_t step = size / nthreads;
		for(unsigned char t = 0; t < nthreads; t++)
		{
			const size_t beginIndex = t * step;
			size_t endIndex = size;
			if (t+1 == nthreads)
				endIndex = size;
			else
				endIndex = beginIndex + step;

			threads[t] = std::thread(std::bind(
				[&](const size_t & beginIndex,
					const size_t & endIndex,
					const int t)
			{
				for (size_t i = beginIndex; i < endIndex; i++)
				{
					if (negotiator.cancellationPending()) {
						negotiator.cancel = true; 
						//negotiator.addToLogCallback((L"ComputeHashes thread " + to_wstring(t) + L" stopped").c_str());
						break;
					}
					//if (m_pStatus->Stopped())
					//{
					//	break;
					//}
					
					progressGuard.lock();
					//negotiator.progressCallback(progress, size, (*file).c_str());
					progress++;
					m_pStatus->SetProgress(progress, size);
					progressGuard.unlock();

					guard.lock();
					TImageDataPtr imageDataPtr = m_pImageDataStorage->Get(TFileInfo(pPointArray[i].path));
					guard.unlock();


				/*	if(!imageDataPtr->crc32c)
						SetCrc32c(pImageData);
					if(pImageData->PixelDataFillingNeed(m_pOptions))
						FillPixelData(pImageData);
					if(pImageData->DefectCheckingNeed(m_pOptions))
						CheckOnDefect(pImageData);
					TDefectType defect = imageDataPtr->GetDefect(m_pOptions);
					if(defect > AD_DEFECT_NONE)
						m_pResult->AddDefectImage(imageDataPtr, defect);
					imageDataPtr->FillOther(m_pOptions);
					imageDataPtr->FreeGlobal();*/

					m_pDataCollector->Fill(imageDataPtr);
										
					if (imageDataPtr->jpegPeaks == 0)
					{
						try
						{
							//imageDataPtr->hGlobal = LoadFileToMemory(imageDataPtr->path.Original().c_str());
							//TImage *pImage = TImage::Load(imageDataPtr->hGlobal);
							TImage *pImage = TImage::Load(imageDataPtr->path.Original().c_str());
							if(pImage)
							{
								imageDataPtr->jpegPeaks = pPointArray[i].jpegPeaks = TDctHistogramPeaks::CalcDctHistogramPeaks(*pImage->View());
								m_pImageDataStorage->SetSaveState(true);
								delete pImage;
							}
							/*else
							{
								std::stringstream ss;
								ss << "Image " << pPointArray[i].path << " not loaded!";
								throw std::exception(ss.str().c_str());
							}*/
						}
						catch (std::exception& e)
						{			
							MessageBoxW(NULL, (TString("Error in ") + imageDataPtr->path.Original() + TString("\n\n") + TString(e.what())).c_str(), 
								L"Error in CalcDctHistogramPeaks ", MB_OK|MB_SYSTEMMODAL);
						}
						catch (cv::Exception& e) 
						{
							MessageBoxW(NULL, (TString("OpenCV error in ") + imageDataPtr->path.Original() + TString("\n\n") + TString(e.what())).c_str(), 
								L"OpenCV error in CalcDctHistogramPeaks ", MB_OK|MB_SYSTEMMODAL);
						}
					}
					//else
					//{
					//	std::cout << "loaded" << std::endl;
					//}


				}//end for(auto file = iterator; file != end; ++file) 
			}, beginIndex, endIndex, t));
		};

		std::for_each(threads.begin(), threads.end(), [](std::thread& x){x.join();});


		m_pImageDataPtrs->clear();
        m_pStatus->Reset();

		delete m_pDataCollector;
	}
}
