#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include <functional>

using namespace std;

///////////////////////////////////////////////////////////////////////
// Объявление
///////////////////////////////////////////////////////////////////////

template <typename T, int k>
struct vec_k
{
private:
	array <T, k> val;

public:
	vec_k();
	vec_k(array <T, k>);
	T& operator[](int);
};

template <typename T, int k>
bool operator<(vec_k <T, k>, vec_k <T, k>);

// Евклидово расстояние для чисел
template <typename T, int k>
T dist(vec_k <T, k>, vec_k <T, k>);

template <typename T, int k>
struct kd_tree
{
private:
	struct node_k
	{
		vec_k <T, k> val;
		node_k* lhs = nullptr;
		node_k* rhs = nullptr;
	};
	
	node_k* root;
	vector <function <bool(vec_k <T, k>, vec_k <T, k>)>> compare_by;

	node_k* build_node(int, vector <vec_k <T, k>>&);
	node_k* copy_node(node_k*);
	void destroy_node(node_k*);
	void nearest_r(node_k*, int, vec_k <T, k>&, T&, vec_k <T, k>, function <T(vec_k <T, k>, vec_k <T, k>)>) const;

public:
	kd_tree();
	kd_tree(vector <vec_k <T, k>>);
	kd_tree(const kd_tree <T, k>&);
	~kd_tree();
	vec_k <T, k> nearest(vec_k <T, k>, function <T(vec_k <T, k>, vec_k <T, k>)>) const;
	vec_k <T, k> nearest(vec_k <T, k>) const;
	kd_tree <T, k>& operator=(kd_tree<T, k>);
};

///////////////////////////////////////////////////////////////////////
//Реализация
///////////////////////////////////////////////////////////////////////

template <typename T, int k>
vec_k <T, k>::vec_k()
{
	for (int i = 0; i < k; i++)
		val[i] = T();
};

template <typename T, int k>
vec_k <T, k>::vec_k(array <T, k> v)
{
	val = v;
};

template <typename T, int k>
T& vec_k <T, k>::operator[](int i)
{
	return val[i];
};

template <typename T, int k>
bool operator<(vec_k <T, k> lhs, vec_k <T, k> rhs)
{
	for (int i = 0; i < k; i++)
	{
		if (lhs[i] != rhs[i])
			return lhs[i] < rhs[i];
	}
	return false;
}

template <typename T, int k>
T dist(vec_k <T, k> v1, vec_k <T, k> v2)
{
	double r = 0;
	for (int i = 0; i < k; i++)
		r += (v1[i] - v2[i]) * (v1[i] - v2[i]);
	return T(sqrt(r));
}

template <typename T, int k>
kd_tree <T, k>::node_k* kd_tree <T, k>::build_node(int dim, vector <vec_k <T, k>>& v)
{
	int n = v.size();
	if (n == 1)
	{
		kd_tree <T, k>::node_k* node = new kd_tree <T, k>::node_k;
		node->val = v[0];
		return node;
	}
	std::sort(v.begin(), v.end(), kd_tree <T, k>::compare_by[dim]);
	vector <vec_k <T, k>> lhs, rhs;
	for (int i = 0; i < n; i++)
	{
		if (i < n / 2 + n % 2)
			lhs.push_back(v[i]);
		else
			rhs.push_back(v[i]);
	}
	kd_tree <T, k>::node_k* node = new kd_tree <T, k>::node_k;
	node->val = lhs.back();
	node->lhs = build_node((dim + 1) % k, lhs);
	node->rhs = build_node((dim + 1) % k, rhs);
	return node;
}

template <typename T, int k>
kd_tree <T, k>::node_k* kd_tree <T, k>::copy_node(node_k* ptr)
{
	if (ptr == nullptr)
		return nullptr;
	kd_tree <T, k>::node_k* cur = new kd_tree <T, k>::node_k;
	cur->val = ptr->val;
	cur->lhs = copy_node(ptr->lhs);
	cur->rhs = copy_node(ptr->rhs);
	return cur;
}

template <typename T, int k>
void kd_tree <T, k>::destroy_node(node_k* ptr)
{
	if (ptr == nullptr)
		return;
	destroy_node(ptr->lhs);
	destroy_node(ptr->rhs);
	delete ptr;
}

template <typename T, int k>
kd_tree <T, k>::kd_tree()
{
	root = nullptr;
}

template <typename T, int k>
kd_tree <T, k>::kd_tree(vector <vec_k <T, k>> v)
{
	for (int i = 0; i < k; i++)
	{
		auto cmp = [i](vec_k <T, k> lhs, vec_k <T, k> rhs)
		{
			return lhs[i] < rhs[i];
		};
		compare_by.push_back(cmp);
	}
	root = build_node(0, v);
}

template <typename T, int k>
kd_tree <T, k>::kd_tree(const kd_tree <T, k>& tree)
{
	destroy_node(root);
	root = copy_node(tree.root);
}

template <typename T, int k>
kd_tree <T, k>::~kd_tree()
{
	destroy_node(root);
}

template <typename T, int k> 
void kd_tree <T, k>::nearest_r(node_k* ptr, int dim, vec_k <T, k>& res, T& r, 
							   vec_k <T, k> point, function <T(vec_k <T, k>, vec_k <T, k>)> metric) const
{
	if (ptr->lhs == nullptr)
	{
		double cur_r = metric(point, ptr->val);
		if (cur_r < r)
		{
			r = cur_r;
			res = ptr->val;
		}
		return;
	}
	if (point[dim] - r < ptr->val[dim])
		nearest_r(ptr->lhs, (dim + 1) % k, res, r, point, metric);
	if (point[dim] + r > ptr->val[dim])
		nearest_r(ptr->rhs, (dim + 1) % k, res, r, point, metric);
}

template <typename T, int k>
vec_k <T, k> kd_tree <T, k>::nearest(vec_k <T, k> point, function <T(vec_k <T, k>, vec_k <T, k>)> metric) const
{
	kd_tree <T, k>::node_k* cur = root;
	int dim = 0;
	while (cur->lhs != nullptr)
	{
		if (point[dim] < cur->val[dim])
			cur = cur->lhs;
		else
			cur = cur->rhs;
		dim = (dim + 1) % k;
	}
	vec_k <T, k> res = cur->val;
	T r = metric(res, point);
	nearest_r(root, 0, res, r, point, metric);
	return res;
}

template <typename T, int k>
vec_k <T, k> kd_tree <T, k>::nearest(vec_k <T, k> point) const
{
	return kd_tree <T, k>::nearest(point, dist<T, k>);
}

template <typename T, int k>
kd_tree <T, k>& kd_tree <T, k>::operator=(kd_tree <T, k> rhs)
{
	destroy_node(root);
	root = copy_node(rhs.root);
	return *this;
}