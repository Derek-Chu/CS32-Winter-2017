// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#ifndef MYMAP_H
#define MYMAP_H
#include "support.h"

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap();
	~MyMap();
	void clear();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	struct Node
	{
		Node(KeyType newKey, ValueType newValue, Node* newParent) {
			key = newKey;
			value = newValue;
			parent = newParent;
			leftChild = nullptr;
			rightChild = nullptr;
		}
		KeyType key;
		ValueType value;
		Node* parent;
		Node* leftChild;
		Node* rightChild;
	};
	
	Node* root;
	int m_size;
	void clear(Node* curr);
	const ValueType* find(const KeyType& key, Node* curr) const;
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap() {
	root = nullptr;
	m_size = 0;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap() {
	clear();
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear()
{
	if (root == nullptr)
	{
		return;
	}
	clear(root);
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear(Node* curr) {
	if (curr == nullptr) {
		return;
	}
	else {
		clear(curr->leftChild);
		clear(curr->rightChild);
		delete curr;
	}	
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const
{
	return m_size;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	if (root == nullptr) {
		root = new Node(key, value, nullptr);
		m_size++;
		return;
	}
	ValueType* temp = find(key);
	if (temp != nullptr) {
		*temp = value;
		return;
	}
	Node* iter = root;
	for (;;) {
		if (key < iter->key) {
			if (iter->leftChild != nullptr) {
				iter = iter->leftChild;
			}
			else {
				iter->leftChild = new Node(key, value, iter);
				m_size++;
				return;
			}
		}
		else if (key > iter->key) {
			if (iter->rightChild != nullptr) {
				iter = iter->rightChild;
			}
			else {
				iter->rightChild = new Node(key, value, iter);
				m_size++;
				return;
			}
		}
	}
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
	return find(key, root);
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key, Node* curr) const
{
	if (curr == nullptr)
	{
		return nullptr;
	}
	else if (curr->key == key)
	{
		return const_cast<ValueType*>(&(curr->value));
	}
	else if (curr->key > key) {
		return find(key, curr->leftChild);
	}
	else {
		return find(key, curr->rightChild);
	}
}

#endif