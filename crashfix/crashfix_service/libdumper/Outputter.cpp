#include "stdafx.h"
#include "Outputter.h"

void COutputter::Init(FILE* f, OUTPUT_FORMAT fmt)
{
	assert(f!=NULL);
	m_fOut = f;
	m_OutFmt = fmt;
	m_pCurSection = NULL;
	m_pCurRow = NULL;
}

void COutputter::BeginDocument(LPCSTR pszTitle)
{
	if(m_OutFmt==OUTPUT_TEXT)
	{
		fprintf(m_fOut, "= %s = \n\n", pszTitle);
	}
	else if(m_OutFmt==OUTPUT_XML)
	{
		TiXmlNode* root = new TiXmlElement("DocumentRoot");
		m_doc.LinkEndChild(root);

		TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
		m_doc.InsertBeforeChild(root, *decl);
		delete decl;
	}
}

void COutputter::EndDocument()
{
	if(m_OutFmt==OUTPUT_TEXT)
	{
		fprintf(m_fOut, "\n== END ==\n");
	}
	else if(m_OutFmt==OUTPUT_XML)
	{
		m_doc.SaveFile(m_fOut);
	}
}

void COutputter::BeginSection(LPCSTR pszTitle, ...)
{
	if(m_OutFmt==OUTPUT_TEXT)
	{
		fprintf(m_fOut, "== ");

		va_list args;
		va_start(args, pszTitle);
		vfprintf(m_fOut, pszTitle, args);

		fprintf(m_fOut, " ==\n\n");
	}
	else if(m_OutFmt==OUTPUT_XML)
	{
		char szBuffer[1024]="";
		va_list args;
		va_start(args, pszTitle);
#ifdef _WIN32
		vsprintf_s(szBuffer, 1024, pszTitle, args);
#else
        vsprintf(szBuffer, pszTitle, args);
#endif
		TiXmlHandle hRoot = m_doc.RootElement();
		TiXmlHandle hElem = new TiXmlElement(szBuffer);
		hRoot.ToNode()->LinkEndChild(hElem.ToNode());
		m_pCurSection = hElem.ToElement();
	}
}

//! Emits section footer.
void COutputter::EndSection()
{
	if(m_OutFmt==OUTPUT_TEXT)
	{
		fprintf(m_fOut, "\n\n");
	}
	else if(m_OutFmt==OUTPUT_XML)
	{
		m_pCurSection = NULL;
	}
}

//! Emits a table row.
void COutputter::BeginTableRow()
{
	if(m_OutFmt==OUTPUT_TEXT)
	{
	}
	else if(m_OutFmt==OUTPUT_XML)
	{
		TiXmlHandle hElem = new TiXmlElement("Row");
		m_pCurSection->LinkEndChild(hElem.ToNode());
		m_pCurRow = hElem.ToElement();
	}
}

//! Emits a table row footer.
void COutputter::EndTableRow()
{
	if(m_OutFmt==OUTPUT_TEXT)
	{
	}
	else if(m_OutFmt==OUTPUT_XML)
	{
		m_pCurRow = NULL;
	}
}

//! Emits a record.
void COutputter::PutRecord(LPCSTR pszName, LPCSTR pszValue, ...)
{
	if(m_OutFmt==OUTPUT_TEXT)
	{
		va_list args;
		va_start(args, pszValue);
		fprintf(m_fOut, "%s = ", pszName);
		vfprintf(m_fOut, pszValue, args);
		fprintf(m_fOut, "\n");
	}
	else if(m_OutFmt==OUTPUT_XML)
	{
		char szBuffer[1024]="";
		va_list args;
		va_start(args, pszValue);
#ifdef _WIN32
		vsprintf_s(szBuffer, 1024, pszValue, args);
#else
        vsprintf(szBuffer, pszValue, args);
#endif

		TiXmlHandle hElem = new TiXmlElement(pszName);
		m_pCurSection->LinkEndChild(hElem.ToNode());
		TiXmlText* text = new TiXmlText(szBuffer);
		hElem.ToElement()->LinkEndChild(text);
	}
}

//! Emits a table cell.
void COutputter::PutTableCell(int width, bool bLastInRow, LPCSTR szFormat, ...)
{
	if(m_OutFmt==OUTPUT_TEXT)
	{
		va_list args;
		va_start(args, szFormat);

		char szBuffer[1024]="";
#ifdef _WIN32
		vsprintf_s(szBuffer, 1024, szFormat, args);
#else
		vsprintf(szBuffer, szFormat, args);
#endif

		char szFormat2[32]="";
#ifdef _WIN32
		sprintf_s(szFormat2, 32, "%%-%ds", width);
#else
		sprintf(szFormat2, "%%-%ds", width);
#endif

		fprintf(m_fOut, szFormat2, szBuffer);

		if(bLastInRow)
			fprintf(m_fOut, "\n");
		else
			fprintf(m_fOut, " ");
	}
	else if(m_OutFmt==OUTPUT_XML)
	{
		const int BUFF_SIZE = 1024*10; /*10 KB*/
		char szBuffer[BUFF_SIZE+1]="";
		va_list args;
		va_start(args, szFormat);
#ifdef _WIN32
		vsnprintf_s(szBuffer, BUFF_SIZE, szFormat, args);
#else
        vsnprintf(szBuffer, BUFF_SIZE, szFormat, args);
#endif
		szBuffer[BUFF_SIZE]='\0'; // ensure zero-terminated

		TiXmlHandle hElem = new TiXmlElement("Cell");
		m_pCurRow->LinkEndChild(hElem.ToNode());
		hElem.ToElement()->SetAttribute("val", szBuffer);
	}
}
