#pragma once

// 用于解析继承关系的数据结构，TODO: 抽象出来使用
template<class T>
class tree
{
public:
	tree()
	{
		parent = NULL;
		first_child = NULL;
		first_neighbour = NULL;
	};

	T         data;
	tree<T>*  parent;
	tree<T>*  first_child;
	tree<T>*  first_neighbour;
};