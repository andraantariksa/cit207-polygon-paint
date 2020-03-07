#ifndef _EDGE_BUCKET_H_
#define _EDGE_BUCKET_H_

namespace shape
{

	class EdgeBucket
	{
	private:
	public:
		int y_max;
		int x_of_y_min;
		int dx;
		int dy;
		int carry;
		bool next(int pos);
	};

};

#endif //_EDGE_BUCKET_H_
