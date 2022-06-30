#pragma once

namespace AirEngine::Utils
{
	class CrossLinkableNode
	{
		friend class CrossLinkableColHead;
		friend class CrossLinkableRowHead;
	private:
		CrossLinkableNode* _left;
		CrossLinkableNode* _right;
		CrossLinkableNode* _top;
		CrossLinkableNode* _bottom;
		CrossLinkableColHead* _colHead;
		CrossLinkableRowHead* _rowHead;
	protected:
		CrossLinkableNode();
		virtual ~CrossLinkableNode();
		CrossLinkableColHead* ColHead();
		CrossLinkableRowHead* RowHead();
		void RemoveSelf();
	};
	class CrossLinkableColHead final
	{
	private:
		CrossLinkableNode _head;
		CrossLinkableNode* _end;
	public:
		class Iterator final
		{
			friend class CrossLinkableColHead;
		private:
			CrossLinkableNode* _node;
			Iterator(CrossLinkableNode* node);
			Iterator();
		public:
			~Iterator();
			bool IsValid();
			Iterator operator++();
			Iterator operator++(int);
			Iterator operator--();
			Iterator operator--(int);
			CrossLinkableNode* Node();
		};
		CrossLinkableColHead();
		virtual ~CrossLinkableColHead();

		void Add(CrossLinkableNode* node);
		void Remove(CrossLinkableNode* node);
		Iterator Remove(Iterator iterator);
		bool HaveNode();
		Iterator GetIterator();
	};
	class CrossLinkableRowHead final
	{
	private:
		CrossLinkableNode _head;
		CrossLinkableNode* _end;
	public:
		class Iterator final
		{
			friend class CrossLinkableRowHead;
		private:
			CrossLinkableNode* _node;
			Iterator(CrossLinkableNode* node);
		public:
			Iterator();
			~Iterator();
			bool IsValid();
			Iterator operator++();
			Iterator operator++(int);
			Iterator operator--();
			Iterator operator--(int);
			CrossLinkableNode* Node();
		};
		CrossLinkableRowHead();
		virtual ~CrossLinkableRowHead();

		bool HaveNode();
		void Add(CrossLinkableNode* node);
		void Remove(CrossLinkableNode* node);
		Iterator Remove(Iterator iterator);
		Iterator GetIterator();
	};
}
