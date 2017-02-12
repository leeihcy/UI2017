// stdafx.cpp : 只包括标准包含文件的源文件
// UIBuilder.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用


long g_lUiBuilderLogCookie = 0;  //_T("ui_builder")

void UIEDITOR_LOG_DEBUG( TCHAR* szContent, ... )
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_DEBUG, szContent );
}
void UIEDITOR_LOG_INFO( TCHAR* szContent, ... )
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_INFO, szContent );
}
void UIEDITOR_LOG_WARN( TCHAR* szContent, ... )
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_WARN, szContent );
}
void UIEDITOR_LOG_ERROR( TCHAR* szContent, ... )
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_ERROR, szContent );
}
void UIEDITOR_LOG_FATAL( TCHAR* szContent, ...)
{
	//UIBUILDER_LOG_LEVEL(LOG_LEVEL_FATAL, szContent );
}

