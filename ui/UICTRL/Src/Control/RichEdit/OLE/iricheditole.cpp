#include "stdafx.h"
#include "richeditolemgr.h"
#include "Composite\Picture\pictureole.h"
#include "Gif\gifoleobject.h"

using namespace UI;


IREOleBase::IREOleBase(REOleBase* p)
{
    m_pImplBase = p;
}
REOLETYPE  IREOleBase::GetOleType()
{
    return m_pImplBase->GetOleType();
}
void  IREOleBase::AddBizProp(LPCTSTR key, LPCTSTR value)
{
	m_pImplBase->AddBizProp(key, value);
}
void  IREOleBase::ClearBizProp()
{
	m_pImplBase->ClearBizProp();
}
LPCTSTR  IREOleBase::GetBizProp(LPCTSTR key)
{
	return m_pImplBase->GetBizProp(key);
}

IPictureREOle::IPictureREOle(PictureOle* p) : IREOleBase(p)
{
    m_pImpl = p;
}

LPCTSTR  IPictureREOle::GetPath()
{
    return m_pImpl->GetPath();
}
void  IPictureREOle::UpdateStatus(REOleLoadStatus e, LPCTSTR szPath)
{
    // m_pImpl->UpdateStatus(e, szPath);
}

bool  IPictureREOle::GetImageMD5(char buf[256])
{
	return m_pImpl->GetImageMD5(buf);
}

bool  IPictureREOle::SaveImage(LPCTSTR path)
{
	return m_pImpl->SaveImage(path);
}

void UI::IPictureREOle::SetPath(LPCTSTR path)
{
	m_pImpl->SetPath(path);
}

void UI::IPictureREOle::SetCacheImage(const UI::CCacheImage* image)
{
	m_pImpl->SetCacheImage(image);
}

void  IPictureREOle::LoadFile(LPCTSTR path)
{
	m_pImpl->LoadFile(path);
}
void  IPictureREOle::LoadUrl(LPCSTR url)
{
	m_pImpl->LoadUrl(url);
}

// void  IPictureREOle::SetOriginPath(LPCTSTR path)
// {
// 	m_pImpl->SetOriginPath(path);
// }
// LPCTSTR  IPictureREOle::GetOriginPath()
// {
// 	return m_pImpl->GetOriginPath();
// }

IGifOle::IGifOle(GifOle* p) : IREOleBase(p)
{

}