#pragma once

enum APInterpolationMethodType {
	IMT_Linear = 1,
	IMT_Quad,
	IMT_None
};


class APInterpolator
{
public:
	template<class V, class K> 
	static V interplate(V val1, V val2, K key1, K key2, K key, APInterpolationMethodType type)
	{
		if (key1 == key2) {
			return val1;
		}

		if (key < key1) {
			return val1;
		}
		else if (key > key2) {
			return val2;
		}

		switch (type) {
		case IMT_Linear:
			return linearInterplate(val1, val2, key1, key2, key);
		case IMT_Quad:
			return quadInterplate(val1, val2, key1, key2, key);
		default:
			break;
		}
		return val1;
	}

private:
	template<class V, class K>
	static V linearInterplate(V val1, V val2, K key1, K key2, K key)
	{
		V ret = val1 + (val2 - val1) / (key2 - key1) * (key - key1);
		return ret;
	}

	template<class V, class K>
	static V quadInterplate(V val1, V val2, K key1, K key2, K key)
	{
		V p1 = (val1 - val2) / (key1 * key1 - key2 * key2);
		V p2 = val1 - p1 * (key1 * key1);
		V ret = p1 * key + p2;
		return ret;
	}
};

