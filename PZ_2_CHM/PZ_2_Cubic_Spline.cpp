#include <iostream>
#include <cmath>
#include <vector>
#include "Point.h"
#include "Cubic_Interpolation_Spline_1D.h"

const double PI = 3.141592653589793238463;

// Генерация равномерной сетки, Num_Segment - число сегментов
std::vector<Com_Methods::Point> Generate_Mesh(double a, double b, int Num_Segment) {
	std::vector<Com_Methods::Point> Mesh;
	double h = 0;
	double Mesh_Node = a;

	h = (b - a) / Num_Segment;

	for (int i = 0; i < Num_Segment + 1; i++) {
		Mesh.push_back(Com_Methods::Point(Mesh_Node, 0.0, 0.0));
		Mesh_Node += h;
	}

	return Mesh;
}

int main() {
	try {
		// Равномерная сетка
		std::vector<Com_Methods::Point> Mesh = Generate_Mesh(0, PI / 2, 3);
		Com_Methods::Cubic_Interpolation_Spline_1D Spline;
		std::vector<double> Func;
		double Res[3];
		std::vector<Com_Methods::Point> Random_Points = { Com_Methods::Point(0.0, 0.0, 0.0),
														  Com_Methods::Point(PI / 25, 0.0, 0.0),
														  Com_Methods::Point(2 * PI / 25, 0.0, 0.0),
														  Com_Methods::Point(3 * PI / 25, 0.0, 0.0),
														  Com_Methods::Point(4 * PI / 25, 0.0, 0.0),
														  Com_Methods::Point(5 * PI / 25, 0.0, 0.0),
														  Com_Methods::Point(6 * PI / 25, 0.0, 0.0),
														  Com_Methods::Point(7 * PI / 25, 0.0, 0.0),
														  Com_Methods::Point(PI / 13, 0.0, 0.0),
														  Com_Methods::Point(PI / 7, 0.0, 0.0),
														  Com_Methods::Point(PI / 11, 0.0, 0.0),
														  Com_Methods::Point(PI / 3, 0.0, 0.0),
														  Com_Methods::Point(PI / 2, 0.0, 0.0) };
		
		// Вычисление значений функции в узловых точках сетки
		for (int i = 0; i < Mesh.size(); i++) {
			Func.push_back(cos(Mesh[i].x()));
		}

		// Построение сплайна
		Spline.Update_Spline(Mesh, Func);

		// Значение сплайна в точках, не совпадающих с узловыми
		for (int i = 0; i < Random_Points.size(); i++) {
			Spline.Get_Value(Com_Methods::Point(Random_Points[i].x(), 0.0, 0.0), Res);
			std::cout << "g(x) = " << Res[0] << "\t" << "g'(x) = " << Res[1] << "\t" << "g''(x) = " << Res[2] << std::endl;
		}
	}
	catch (std::exception& Ex) {
		std::cout << "Error: " << Ex.what() << std::endl;
	}
}
