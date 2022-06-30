#include "Utils/CrossLinkableNode.h"
#include "Utils/Log.h"

AirEngine::Utils::CrossLinkableNode::CrossLinkableNode()
	: _left(nullptr)
	, _right(nullptr)
	, _top(nullptr)
	, _bottom(nullptr)
	, _colHead(nullptr)
	, _rowHead(nullptr)
{
}

AirEngine::Utils::CrossLinkableNode::~CrossLinkableNode()
{
}

AirEngine::Utils::CrossLinkableColHead* AirEngine::Utils::CrossLinkableNode::ColHead()
{
	return _colHead;
}

AirEngine::Utils::CrossLinkableRowHead* AirEngine::Utils::CrossLinkableNode::RowHead()
{
	return _rowHead;
}

void AirEngine::Utils::CrossLinkableNode::RemoveSelf()
{
	_colHead->Remove(this);
	_rowHead->Remove(this);
}

AirEngine::Utils::CrossLinkableColHead::CrossLinkableColHead()
	: _head()
	, _end(nullptr)
{
	_head._colHead = this;
	_end = &_head;
}

AirEngine::Utils::CrossLinkableColHead::~CrossLinkableColHead()
{
}

void AirEngine::Utils::CrossLinkableColHead::Add(CrossLinkableNode* node)
{
	_end->_bottom = node;
	node->_top = _end;
	node->_bottom = nullptr;

	_end = node;

	node->_colHead = this;
}

void AirEngine::Utils::CrossLinkableColHead::Remove(CrossLinkableNode* node)
{
	if (node->_colHead != this)
	{
		Utils::Log::Exception("The cross linked node do not blong to this head.");
	}

	if (node != _end)
	{
		node->_top->_bottom = node->_bottom;
		node->_bottom->_top = node->_top;
	}
	else
	{
		node->_top->_bottom = nullptr;

		_end = node->_top;
	}

	node->_top = nullptr;
	node->_bottom = nullptr;
	node->_colHead = nullptr;
}

AirEngine::Utils::CrossLinkableColHead::Iterator AirEngine::Utils::CrossLinkableColHead::Remove(Iterator iterator)
{
	if (!iterator.IsValid())
	{
		Utils::Log::Exception("This Iterator is not valid.");
	}
	auto node = iterator._node;
	Iterator next = Iterator();
	if (node->_colHead != this)
	{
		Utils::Log::Exception("The cross linked node do not blong to this head.");
	}

	if (node != _end)
	{
		next._node = node->_bottom;

		node->_top->_bottom = node->_bottom;
		node->_bottom->_top = node->_top;
	}
	else
	{
		next._node = &node->_colHead->_head;

		node->_top->_bottom = nullptr;

		_end = node->_top;
	}

	node->_top = nullptr;
	node->_bottom = nullptr;
	node->_colHead = nullptr;
	return next;
}

bool AirEngine::Utils::CrossLinkableColHead::HaveNode()
{
	return _end != &_head;
}

AirEngine::Utils::CrossLinkableColHead::Iterator AirEngine::Utils::CrossLinkableColHead::GetIterator()
{
	return Iterator(_head._bottom);
}

AirEngine::Utils::CrossLinkableColHead::Iterator::Iterator()
	: Iterator(nullptr)
{
}

AirEngine::Utils::CrossLinkableColHead::Iterator::Iterator(CrossLinkableNode* node)
	: _node(node)
{
}

AirEngine::Utils::CrossLinkableColHead::Iterator::~Iterator()
{
}

bool AirEngine::Utils::CrossLinkableColHead::Iterator::IsValid()
{
	return _node && _node != &_node->_colHead->_head;
}

AirEngine::Utils::CrossLinkableColHead::Iterator AirEngine::Utils::CrossLinkableColHead::Iterator::operator++()
{
	_node = _node->_bottom;
	return *this;
}

AirEngine::Utils::CrossLinkableColHead::Iterator AirEngine::Utils::CrossLinkableColHead::Iterator::operator++(int)
{
	Utils::CrossLinkableColHead::Iterator t = *this;
	_node = _node->_bottom;
	return t;
}

AirEngine::Utils::CrossLinkableColHead::Iterator AirEngine::Utils::CrossLinkableColHead::Iterator::operator--()
{
	_node = _node->_top;
	return *this;
}

AirEngine::Utils::CrossLinkableColHead::Iterator AirEngine::Utils::CrossLinkableColHead::Iterator::operator--(int)
{
	Utils::CrossLinkableColHead::Iterator t = *this;
	_node = _node->_top;
	return t;
}

AirEngine::Utils::CrossLinkableNode* AirEngine::Utils::CrossLinkableColHead::Iterator::Node()
{
	return _node;
}

AirEngine::Utils::CrossLinkableRowHead::CrossLinkableRowHead()
	: _head()
	, _end(nullptr)
{
	_head._rowHead = this;
	_end = &_head;
}

AirEngine::Utils::CrossLinkableRowHead::~CrossLinkableRowHead()
{
}

bool AirEngine::Utils::CrossLinkableRowHead::HaveNode()
{
	return _end != &_head;
}

void AirEngine::Utils::CrossLinkableRowHead::Add(CrossLinkableNode* node)
{
	_end->_right = node;
	node->_left = _end;
	node->_right = nullptr;

	_end = node;

	node->_rowHead = this;
}

void AirEngine::Utils::CrossLinkableRowHead::Remove(CrossLinkableNode* node)
{
	if (node->_rowHead != this)
	{
		Utils::Log::Exception("The cross linked node do not blong to this head.");
	}

	if (node != _end)
	{
		node->_left->_right = node->_right;
		node->_right->_left = node->_left;
	}
	else
	{
		node->_left->_right = nullptr;

		_end = node->_left;
	}

	node->_left = nullptr;
	node->_right = nullptr;
	node->_rowHead = nullptr;
}

AirEngine::Utils::CrossLinkableRowHead::Iterator AirEngine::Utils::CrossLinkableRowHead::Remove(Iterator iterator)
{
	if (!iterator.IsValid())
	{
		Utils::Log::Exception("This Iterator is not valid.");
	}
	auto node = iterator._node;
	Iterator next = Iterator();
	if (node->_rowHead != this)
	{
		Utils::Log::Exception("The cross linked node do not blong to this head.");
	}

	if (node != _end)
	{
		next._node = node->_right;

		node->_left->_right = node->_right;
		node->_right->_left = node->_left;
	}
	else
	{
		next._node = &node->_rowHead->_head;

		node->_left->_right = nullptr;

		_end = node->_left;
	}

	node->_left = nullptr;
	node->_right = nullptr;
	node->_rowHead = nullptr;
	return next;
}

AirEngine::Utils::CrossLinkableRowHead::Iterator AirEngine::Utils::CrossLinkableRowHead::GetIterator()
{
	return Iterator(_head._right);
}

AirEngine::Utils::CrossLinkableRowHead::Iterator::Iterator()
	: Iterator(nullptr)
{
}

AirEngine::Utils::CrossLinkableRowHead::Iterator::Iterator(CrossLinkableNode* node)
	: _node(node)
{
}

AirEngine::Utils::CrossLinkableRowHead::Iterator::~Iterator()
{
}

bool AirEngine::Utils::CrossLinkableRowHead::Iterator::IsValid()
{
	return _node && _node != &_node->_rowHead->_head;
}

AirEngine::Utils::CrossLinkableRowHead::Iterator AirEngine::Utils::CrossLinkableRowHead::Iterator::operator++()
{
	_node = _node->_right;
	return *this;
}

AirEngine::Utils::CrossLinkableRowHead::Iterator AirEngine::Utils::CrossLinkableRowHead::Iterator::operator++(int)
{
	Utils::CrossLinkableRowHead::Iterator t = *this;
	_node = _node->_right;
	return t;
}

AirEngine::Utils::CrossLinkableRowHead::Iterator AirEngine::Utils::CrossLinkableRowHead::Iterator::operator--()
{
	_node = _node->_left;
	return *this;
}

AirEngine::Utils::CrossLinkableRowHead::Iterator AirEngine::Utils::CrossLinkableRowHead::Iterator::operator--(int)
{
	Utils::CrossLinkableRowHead::Iterator t = *this;
	_node = _node->_left;
	return t;
}

AirEngine::Utils::CrossLinkableNode* AirEngine::Utils::CrossLinkableRowHead::Iterator::Node()
{
	return _node;
}
