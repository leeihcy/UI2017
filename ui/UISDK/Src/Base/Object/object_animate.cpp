#include "stdafx.h"
#include "object.h"
#include "Src\Layer\layer.h"
#include "object_layer.h"

void  Object::SetOpacity(const unsigned char alpha)
{
	if (!m_pLayer)
		return;
	if (!m_pLayer->GetLayer())
		return;
	if (m_pLayer->GetLayer()->GetOpacity() == alpha)
		return;
	m_pLayer->GetLayer()->SetOpacity(alpha);
}
unsigned char Object::GetOpacity() const
{
	if (!m_pLayer)
		return 255;
	if (!m_pLayer->GetLayer())
		return 255;
	return m_pLayer->GetLayer()->GetOpacity();
}
void  Object::SetOpacity(const unsigned char alpha, LayerAnimateParam* param)
{
	if (!m_pLayer)
	{
		EnableLayer(true);
		if (!m_pLayer)
			return;
	}
	else
	{
		if (m_pLayer->GetLayer())
			m_pLayer->GetLayer()->AddRef();
	}

	if (!m_pLayer->GetLayer())
		return;

	LayerAnimateParam defaultParam;
	if (!param)
		param = &defaultParam;
	m_pLayer->GetLayer()->SetOpacity(alpha, param);
}


void  Object::SetTranslate(float x, float y, float z)
{
	if (!m_pLayer)
		return;
	if (!m_pLayer->GetLayer())
		return;
	m_pLayer->GetLayer()->SetTranslate(x, y, z);
}

void  Object::SetTranslate(float x, float y, float z, LayerAnimateParam* param)
{
	if (!m_pLayer)
	{
		EnableLayer(true);
		if (!m_pLayer)
			return;
	}
	else
	{
		if (m_pLayer->GetLayer())
			m_pLayer->GetLayer()->AddRef();
	}

	if (!m_pLayer->GetLayer())
		return;

	LayerAnimateParam defaultParam;
	if (!param)
		param = &defaultParam;
	m_pLayer->GetLayer()->SetTranslate(x, y, z, param);
}