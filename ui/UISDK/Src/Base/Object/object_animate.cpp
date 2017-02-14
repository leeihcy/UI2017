#include "stdafx.h"
#include "object.h"
#include "Src\Layer\layer.h"
#include "object_layer.h"

void  Object::SetOpacity(const unsigned char alpha)
{
	Layer* layer = GetSelfLayer();
	if (layer)
		layer->SetOpacity(alpha);
}
unsigned char Object::GetOpacity() const
{
	Layer* layer = GetSelfLayer();
	if (!layer)
		return 255;

	return layer->GetOpacity();
}
void  Object::SetOpacity(const unsigned char alpha, LayerAnimateParam* param)
{
	Layer* layer = GetSelfLayer();
	if (!layer)
	{
		EnableLayer(true);
		layer = GetSelfLayer();
		if (!layer)
			return;
	}
	else
	{
		layer->AddRef();
	}

	LayerAnimateParam defaultParam;
	if (!param)
		param = &defaultParam;
	layer->SetOpacity(alpha, param);
}

void  Object::SetTranslate(float x, float y, float z)
{
	Layer* layer = GetSelfLayer();
	if (layer)
		layer->SetTranslate(x, y, z);
}

void  Object::SetTranslate(float x, float y, float z, LayerAnimateParam* param)
{
	Layer* layer = GetSelfLayer();
	if (!layer)
	{
		EnableLayer(true);
		layer = GetSelfLayer();
		if (!layer)
			return;
	}
	else
	{
		layer->AddRef();
	}

	LayerAnimateParam defaultParam;
	if (!param)
		param = &defaultParam;
	layer->SetTranslate(x, y, z, param);
}