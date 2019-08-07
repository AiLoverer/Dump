#pragma once
#include "stdafx.h"
#include "tinyxml.h"

//! Output format type
enum OUTPUT_FORMAT
{
	OUTPUT_TEXT,    //!< Text format
	OUTPUT_XML      //!< XML format
};

//! \class COutputter
//! This class is used for generating the content of the resulting file.
//! Currently text and XML formats are supported.
class COutputter
{
public:

    //! Opens document for writing.
    //! @param[in] f Handle to file opened for writing.
    //! @param[in] fmt Output format.
	void Init(FILE* f, OUTPUT_FORMAT fmt);
	
    //! Emits document header.
	//! @param[in] pszTitle Document title. In XML mode has is ignored.
	void BeginDocument(LPCSTR pszTitle);
	
    //! Emits document footer.
	void EndDocument();
	
    //! Emits section header.
	//! @param[in] pszTitle Section title. In XML mode, this should not contain spaces (otherwise XML will be invalid).
	void BeginSection(LPCSTR pszTitle, ...);
	
    //! Emits section footer.
	void EndSection();
	
    //! Emits a table row.
	void BeginTableRow();
	
    //! Emits a table row footer.
	void EndTableRow();
	
    //! Emits a record.
	//! @param[in] pszName Record name. In XML mode, this should not contain spaces (otherwise XML will be invalid).
	void PutRecord(LPCSTR pszName, LPCSTR pszValue, ...);
	
    //! Emits a table cell.
	void PutTableCell(int width, bool bLastInRow, LPCSTR szFormat, ...);	

private:

	FILE* m_fOut;                  //!< Handle to file.
	OUTPUT_FORMAT m_OutFmt;        //!< Output format
	TiXmlDocument m_doc;           //!< Handle to XML document.
	TiXmlElement* m_pCurSection;   //!< Current section.
	TiXmlElement* m_pCurRow;       //!< Current row.
};

