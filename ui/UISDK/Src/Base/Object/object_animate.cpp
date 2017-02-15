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
	LayerAnimateParam defaultParam;
	if (!param)
		param = &defaultParam;
	GetLayerForAnimate()->SetOpacity(alpha, param);
}

void  Object::SetTranslate(float x, float y, float z)
{
	Layer* layer = GetSelfLayer();
	if (layer)
		layer->SetTranslate(x, y, z);
}

void  Object::SetTranslate(float x, float y, float z, LayerAnimateParam* param)
{
	LayerAnimateParam defaultParam;
	if (!param)
		param = &defaultParam;
	GetLayerForAnimate()->SetTranslate(x, y, z, param);
}

void  Object::RotateYTo(float degree, LayerAnimateParam* param)
{
	LayerAnimateParam defaultParam;
	if (!param)
		param = &defaultParam;
	GetLayerForAnimate()->RotateYTo(degree, param);
}

void  Object::RotateYBy(float degree, LayerAnimateParam* param)
{
	LayerAnimateParam defaultParam;
	if (!param)
		param = &defaultParam;
	GetLayerForAnimate()->RotateYBy(degree, param);
}