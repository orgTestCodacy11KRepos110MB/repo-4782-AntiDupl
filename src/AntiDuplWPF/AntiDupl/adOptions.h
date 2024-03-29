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
#ifndef __adOptions_h__
#define __adOptions_h__

#include <list>

#include <limits.h>


#include "adPath.h"

namespace ad
{
    //-------------------------------------------------------------------------
    class TOption
    {
    public:
        TOption(int *pValue, const TChar* section, const TChar* key, int default, 
            int min = INT_MIN, int max = INT_MAX);
        ~TOption(){};

        void SetDefault();
        void Validate();

        void Load(const TChar *fileName);
        bool Save(const TChar *fileName) const;

    private:
        int *m_pValue;
        TString m_key;
        TString m_section;
        int m_default; //�������� �� ���������
        int m_min;
        int m_max;	//������������
    };
    //-------------------------------------------------------------------------
    struct TOptions
    {
        TPathContainer searchPaths;
        TPathContainer ignorePaths;
        TPathContainer validPaths;
        TPathContainer deletePaths;

        adSearchOptions search;
        adCompareOptions compare;
        adDefectOptions defect;
        adAdvancedOptions advanced;

        TOptions();

        void SetDefault();
        void Validate();

        adError Import(adOptionsType optionsType, void* pOptions);
        adError Export(adOptionsType optionsType, void* pOptions) const;

        int GetIgnoreWidthFrame() const;
		const ad::TString GetImageDataBasePath() const;

    private:
        typedef std::list<TOption> TOptionsList;

        TOptionsList m_options;
    };
    //-------------------------------------------------------------------------
}

#endif/*__adOptions_h__*/