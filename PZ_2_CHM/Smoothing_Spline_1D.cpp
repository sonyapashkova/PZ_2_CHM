#include <functional>
#include "Smoothing_Spline_1D.h"
#include <iostream>

namespace Com_Methods
{
	//�����������: SMOOTH - �������� �����������
	Smoothing_Spline_1D::Smoothing_Spline_1D(const double &SMOOTH)
	{
		this->SMOOTH = SMOOTH;
	}

	//������� �� ������ ������� [-1, 1]: 
	//Seg_Num - ����� ��������, � - ��������, Ksi - ���������� �� ������-��������
	void Smoothing_Spline_1D::Transition_To_Master_Element(int Seg_Num, const double &X, double &Ksi) const
	{
		Ksi = 2.0 * (X - Points[Seg_Num].x()) / (Points[Seg_Num + 1].x() - Points[Seg_Num].x()) - 1.0;
	}

	//�������� ������� �� [-1, 1]:
	//Number - ����� �������, Ksi - ���������� �� ������-��������
	double Smoothing_Spline_1D::Basis_Function(int Number, const double &Ksi) const
	{
		switch (Number)
		{
			case 1:  {return 0.5*(1 - Ksi); break; }
			case 2:  {return 0.5*(1 + Ksi); break; }
			default: {throw std::exception("Error in the basis function number..."); break; }
		}
	}

	//����������� �������� ������� �� [-1, 1]:
	//Number - ����� �������, Ksi - ���������� �� ������-��������
	double Smoothing_Spline_1D::Der_Basis_Function(int Number, const double &Ksi) const
	{
		switch (Number)
		{
			case 1:  {return -0.5; break; }
			case 2:  {return  0.5; break; }
			default: {throw std::exception("Error in the basis function derivative number..."); break; }
		}
	}

	//�������� ������
	void Smoothing_Spline_1D::Update_Spline(const std::vector<Point>  & Points,
											const std::vector<double> & F_Value)
	{
		//���������� ������ ����� �������
		this->Points.clear();
		for (auto & elem : Points) this->Points.push_back(elem);

		//����� �������� ���������
		int Num_Segments = Points.size() - 1;

		//������������ ���������� �� ������
		alpha.resize(Num_Segments + 1);

		//��������� �������
		std::vector <double> a, b, c;
		a.resize(Num_Segments + 1); b.resize(Num_Segments + 1); c.resize(Num_Segments + 1);
		
		//��������� ��� ��������������� ����: 
		//Num_Segment - ����� �������, P - �����, F_Val - �������� ��������� ������� � �����, w - ���  
		std::function<void(int Num_Segment, const Point &P, const double &F_Val, const double &w)> 
		Assembling = [&](int i, const Point &P, const double &F_Val, const double &w)
		{
			double X = P.x(), Ksi;
			//������� �� ������-�������
		    Transition_To_Master_Element(i, X, Ksi);
			//���������� �������� �������� ������� �� ������-��������
			double f1 = Basis_Function(1, Ksi);
			double f2 = Basis_Function(2, Ksi);

			//�������� ������� � ������� ����
			b[i]     += (1.0 - SMOOTH) * w * f1 * f1;
			b[i + 1] += (1.0 - SMOOTH) * w * f2 * f2;
			a[i + 1] += (1.0 - SMOOTH) * w * f1 * f2;
			c[i]	 += (1.0 - SMOOTH) * w * f2 * f1;
			//�������� ������� � ������ ������ ����� ����
			alpha[i]     += (1.0 - SMOOTH) * w * f1 * F_Val;
			alpha[i + 1] += (1.0 - SMOOTH) * w * f2 * F_Val;
		};

		//������ ���� �� �����: ����� ������� �� ������� �������� ���������
		for (int i = 0; i < Num_Segments; i++)
		{
			//���������� ���� ����� � ����
			double W = 1.0;
			Assembling(i, this->Points[i], F_Value[i], W);
			Assembling(i, this->Points[i + 1], F_Value[i + 1], W);

			//����� �� ����������� �� ������ �����������
			double h = Points[i + 1].x() - Points[i].x();
			b[i]	 += 1.0 / h * SMOOTH;
			b[i + 1] += 1.0 / h * SMOOTH;
			a[i + 1] -= 1.0 / h * SMOOTH;
			c[i]	 -= 1.0 / h * SMOOTH;
		}

		//����� ��������: ������ ���
		for (int j = 1; j < Num_Segments + 1; j++)
		{
			//���������
			b[j] -= a[j] / b[j - 1] * c[j - 1];
			//������ �����         
			alpha[j] -= a[j] / b[j - 1] * alpha[j - 1]; 
		}

		//����� ��������: �������� ���
		alpha[Num_Segments] /= b[Num_Segments];
		for (int j = Num_Segments - 1; j >= 0; j--)
			alpha[j] = (alpha[j] - alpha[j + 1] * c[j]) / b[j];
	}

	//��������� �������� ������� � ����� P
	void Smoothing_Spline_1D::Get_Value(const Point &P, double * Res)const
	{
		//�������� ����
		double eps = 1e-7;
		//����� ��������
		int Num_Segments = Points.size() - 1;
		//���������� �����
		double X = P.x();

		//����� �������, �������� ����������� �����
		for (int i = 0; i < Num_Segments; i++)
		{
			if (X > Points[i].x() && X < Points[i + 1].x() ||
				fabs(X - Points[i].x()) < eps ||
				fabs(X - Points[i + 1].x()) < eps)
			{
				//����� �������
				double h = Points[i + 1].x() - Points[i].x();
				//������� �� ������-�������, Ksi - ���������� �� ������-��������
				double Ksi;
				Transition_To_Master_Element(i, X, Ksi);
				//��������� �������� ������� � ����������� �� �������� ��������
				Res[0] = alpha[i]      * Basis_Function(1, Ksi) +
						 alpha[i + 1]  * Basis_Function(2, Ksi);
				Res[1] = (alpha[i]     * Der_Basis_Function(1, Ksi) +
						  alpha[i + 1] * Der_Basis_Function(2, Ksi)) * 2.0 / h;
				Res[2] = 0.0;
				return;
			}
		}
		throw std::exception("The point is not found in the segments...");
	}
}
