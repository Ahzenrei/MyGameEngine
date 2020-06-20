#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include <initializer_list>

class Cube
{
public:
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		constexpr float side = 0.5f;

		std::vector<DirectX::XMFLOAT3> vertices;
		vertices.emplace_back(-side, -side, -side); // 0
		vertices.emplace_back(side, -side, -side); // 1
		vertices.emplace_back(-side, side, -side); // 2
		vertices.emplace_back(side, side, -side); // 3
		vertices.emplace_back(-side, -side, side); // 4
		vertices.emplace_back(side, -side, side); // 5
		vertices.emplace_back(-side, side, side); // 6
		vertices.emplace_back(side, side, side); // 7

		std::vector<V> verts(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++)
		{
			verts[i].pos = vertices[i];
		}
		return{
			std::move(verts),{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}
	template <class V>
	static IndexedTriangleList<V> MakeIndependent()
	{

		constexpr float side = 0.5f;

		std::vector <V> vertices(24);
		vertices[0].pos = { -side, -side, -side }; //0 near side
		vertices[1].pos = { side, -side, -side };  // 1
		vertices[2].pos = { -side, side, -side };  // 2
		vertices[3].pos = { side, side, -side };   // 3
		vertices[4].pos = { -side, -side, side };  // 4 far side
		vertices[5].pos = { side, -side, side };   // 5 
		vertices[6].pos = { -side, side, side };   // 6
		vertices[7].pos = { side, side, side };    // 7
		vertices[8].pos = { -side, -side, -side }; // 8 left side
		vertices[9].pos = { -side, side, -side };  // 9
		vertices[10].pos = { -side, -side, side }; // 10
		vertices[11].pos = { -side, side, side };  // 11 
		vertices[12].pos = { side, side, -side };  // 12 right side
		vertices[13].pos = { side, side, side };   // 13
		vertices[14].pos = { side, -side, side };  // 14
		vertices[15].pos = { side, -side, -side }; // 15
		vertices[16].pos = { -side, side, -side }; // 16 top side
		vertices[17].pos = { -side, side, side };  // 17
		vertices[18].pos = { side, side, side };   // 18
		vertices[19].pos = { side, side, -side };  // 19
		vertices[20].pos = { -side, -side, -side };// 20 bottom side
		vertices[21].pos = { -side, -side, side }; // 21
		vertices[22].pos = { side, -side, side };  // 22
		vertices[23].pos = { side, -side, -side }; // 23

		return{
			std::move(vertices),{
				0,2,1, 2,3,1,        //near side
				5,7,4, 7,6,4,        //far side
				10,11,8, 11,9,8,     //left side
				15,12,14, 12,13,14,  //right side
				16,17,19, 17,18,19,  //top side
				21,20,22, 20,23,22   //bottom side
			}
		};
	}
};