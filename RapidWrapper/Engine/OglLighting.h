#pragma once

namespace RWrapper 
{

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class OglLighting:
		public System::Windows::Forms::Form
	{
	private: System::Windows::Forms::NumericUpDown^  matambX;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::NumericUpDown^  matambR;

	private: System::Windows::Forms::NumericUpDown^  matambZ;

	private: System::Windows::Forms::NumericUpDown^  matambY;
	private: System::Windows::Forms::Button^  BtnUpdate;


	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::NumericUpDown^  MatSpcR;

	private: System::Windows::Forms::NumericUpDown^  MatSpcZ;

	private: System::Windows::Forms::NumericUpDown^  MatSpcY;

	private: System::Windows::Forms::NumericUpDown^  MatSpcX;

	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::NumericUpDown^  MatDiffR;

	private: System::Windows::Forms::NumericUpDown^  MatDiffZ;

	private: System::Windows::Forms::NumericUpDown^  MatDiffY;

	private: System::Windows::Forms::NumericUpDown^  MatDiffX;

	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::NumericUpDown^  ModAmbR;

	private: System::Windows::Forms::NumericUpDown^  ModAmbZ;

	private: System::Windows::Forms::NumericUpDown^  ModAmbY;

	private: System::Windows::Forms::NumericUpDown^  ModAmbX;

	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown17;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown18;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown19;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown20;
	private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::NumericUpDown^  lightposR1;

	private: System::Windows::Forms::NumericUpDown^  lightposZ1;


	private: System::Windows::Forms::NumericUpDown^  lightposY1;



	private: System::Windows::Forms::NumericUpDown^  lightposX1;






	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBox_Shine;
	private: System::Windows::Forms::GroupBox^  groupBox7;
	private: System::Windows::Forms::NumericUpDown^  lightposR4;

	private: System::Windows::Forms::NumericUpDown^  lightposZ4;

	private: System::Windows::Forms::NumericUpDown^  lightposY4;

	private: System::Windows::Forms::NumericUpDown^  lightposX4;

	private: System::Windows::Forms::GroupBox^  groupBox8;
	private: System::Windows::Forms::NumericUpDown^  lightposR2;
	private: System::Windows::Forms::NumericUpDown^  lightposZ2;
	private: System::Windows::Forms::NumericUpDown^  lightposY2;
	private: System::Windows::Forms::NumericUpDown^  lightposX2;
	private: System::Windows::Forms::GroupBox^  groupBox9;
	private: System::Windows::Forms::NumericUpDown^  lightposR3;

	private: System::Windows::Forms::NumericUpDown^  lightposZ3;

	private: System::Windows::Forms::NumericUpDown^  lightposY3;
	private: System::Windows::Forms::NumericUpDown^  lightposX3;
	private: System::Windows::Forms::GroupBox^  groupBox10;
	private: System::Windows::Forms::NumericUpDown^  lightposR5;

	private: System::Windows::Forms::NumericUpDown^  lightposZ5;



	private: System::Windows::Forms::NumericUpDown^  lightposY5;

	private: System::Windows::Forms::NumericUpDown^  lightposX5;

	private: System::Windows::Forms::GroupBox^  groupBox12;
	private: System::Windows::Forms::NumericUpDown^  lightposR6;

	private: System::Windows::Forms::NumericUpDown^  lightposZ6;

	private: System::Windows::Forms::NumericUpDown^  lightposY6;

	private: System::Windows::Forms::NumericUpDown^  lightposX6;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
	public:
		OglLighting();
		~OglLighting();
		void InitializeComponent();
	    void BtnUpdate_Click(System::Object^  sender, System::EventArgs^  e);
#pragma endregion

	
		};
}
