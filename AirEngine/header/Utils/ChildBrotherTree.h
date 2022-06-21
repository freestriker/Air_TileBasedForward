#pragma once
namespace AirEngine
{
	namespace Utils
	{
		template<typename T>
		class ChildBrotherTree
		{
		public:
			struct Iterator
			{
				friend class ChildBrotherTree<T>;
			private:
				ChildBrotherTree<T>* _node;
				Iterator(ChildBrotherTree<T>* node);
			public:
				Iterator();
				~Iterator();
				inline bool IsValid();
				inline typename T* Node();
				inline typename Iterator operator++();
			};
		private:
			T* _parent;
			T* _brother;
			T* _child;
		public:
			ChildBrotherTree();
			inline T* Parent();
			inline T* Brother();
			inline T* Child();
			inline void AddChild(T* child);
			inline void AddBrother(T* brother);
			inline T* RemoveSelf();
			inline ChildBrotherTree<T>::Iterator ChildIterator();
			inline ChildBrotherTree<T>::Iterator BrotherIterator();
		};
		template<typename T>
		inline ChildBrotherTree<T>::ChildBrotherTree()
			: _parent(nullptr)
			, _brother(nullptr)
			, _child(nullptr)
		{
		}
		template<typename T>
		inline T* ChildBrotherTree<T>::Parent()
		{
			return _parent;
		}
		template<typename T>
		inline T* ChildBrotherTree<T>::Brother()
		{
			return _brother;
		}
		template<typename T>
		inline T* ChildBrotherTree<T>::Child()
		{
			return _child;
		}
		template<typename T>
		inline void ChildBrotherTree<T>::AddChild(T* child)
		{
			ChildBrotherTree<T>* childNode = static_cast<ChildBrotherTree<T>*>(child);
			childNode->_parent = static_cast<T*>(this);
			if (_child)
			{
				ChildBrotherTree<T>* firstChildNode = static_cast<ChildBrotherTree<T>*>(_child);
				firstChildNode->AddBrother(child);
			}
			else
			{
				_child = child;
			}
		}
		template<typename T>
		inline void ChildBrotherTree<T>::AddBrother(T* brother)
		{
			ChildBrotherTree<T>* brotherNode = static_cast<ChildBrotherTree<T>*>(brother);
			brotherNode->_parent = _parent;
			ChildBrotherTree<T>* b = static_cast<ChildBrotherTree<T>*>(_brother);
			if (b)
			{
				while (b->_brother)
				{
					b = static_cast<ChildBrotherTree<T>*>(b->_brother);
				}
				b->_brother = brother;
			}
			else
			{
				_brother = brother;
			}
		}
		template<typename T>
		inline T* ChildBrotherTree<T>::RemoveSelf()
		{
			ChildBrotherTree<T>* result = nullptr;
			if (_parent)
			{
				ChildBrotherTree<T>* pre = nullptr;
				ChildBrotherTree<T>* o = static_cast<ChildBrotherTree<T>*>(this->_parent);
				for (Iterator iter = o->ChildIterator(); iter.IsValid(); ++iter)
				{
					if (iter._node == this)
					{
						break;
					}
					pre = iter._node;
				}
				if (pre)
				{
					pre->_brother = _brother;
					_parent = nullptr;
					_brother = nullptr;
				}
				else
				{
					static_cast<ChildBrotherTree<T>*>(_parent)->_child = _brother;
				}
				_parent = nullptr;
				_brother = nullptr;
				result = this;
			}
			else
			{
				_parent = nullptr;
				_brother = nullptr;
				result = this;
			}

			return static_cast<T*>(result);
		}
		template<typename T>
		inline typename ChildBrotherTree<T>::Iterator ChildBrotherTree<T>::ChildIterator()
		{
			return ChildBrotherTree<T>::Iterator(this);
		}
		template<typename T>
		inline typename ChildBrotherTree<T>::Iterator ChildBrotherTree<T>::BrotherIterator()
		{
			return ChildBrotherTree<T>::Iterator(static_cast<ChildBrotherTree<T>*>(this->_parent));
		}
		template<typename T>
		inline ChildBrotherTree<T>::Iterator::Iterator(ChildBrotherTree<T>* node)
			: _node(node)
		{
		}
		template<typename T>
		inline ChildBrotherTree<T>::Iterator::Iterator()
			: _node(nullptr)
		{
		}
		template<typename T>
		inline ChildBrotherTree<T>::Iterator::~Iterator()
		{
		}
		template<typename T>
		inline bool ChildBrotherTree<T>::Iterator::IsValid()
		{
			return _node;
		}
		template<typename T>
		inline typename T* ChildBrotherTree<T>::Iterator::Node()
		{
			return static_cast<T*>(_node);
		}
		template<typename T>
		inline typename ChildBrotherTree<T>::Iterator ChildBrotherTree<T>::Iterator::operator++()
		{
			_node = static_cast<ChildBrotherTree<T>*>(_node->_brother);
			return *this;
		}
}
}