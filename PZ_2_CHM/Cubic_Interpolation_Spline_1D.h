#pragma once
#ifndef Cubic_Interpolation_Spline_1D_h
#define Cubic_Interpolation_Spline_1D_h

#include "Spline.h"

namespace Com_Methods
{
	//���������� ���������������� ������
	class Cubic_Interpolation_Spline_1D : public Spline
	{
	private:
		//����� �����
		std::vector<Point> Points;
		//������������ �������
		std::vector<double> a, b, c, d;
	public:
		//�������� ������
		void Update_Spline(const std::vector<Point> &Points, const std::vector<double> &F_Value) override;
		//��������� �������� ������� � ����� P
		void Get_Value(const Point &P, double * Res)const override;
	};
}

#endif
