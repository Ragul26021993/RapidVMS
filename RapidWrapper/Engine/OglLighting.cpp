#include "StdAfx.h"
#include "RW_MainInterface.h"

RWrapper::OglLighting::OglLighting()
{
	InitializeComponent();
}

RWrapper::OglLighting::~OglLighting()
{
	if (components)
	{
		delete components;
	}
}

void RWrapper::OglLighting::InitializeComponent()
{
	this->matambX = (gcnew System::Windows::Forms::NumericUpDown());
	this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
	this->matambR = (gcnew System::Windows::Forms::NumericUpDown());
	this->matambZ = (gcnew System::Windows::Forms::NumericUpDown());
	this->matambY = (gcnew System::Windows::Forms::NumericUpDown());
	this->BtnUpdate = (gcnew System::Windows::Forms::Button());
	this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
	this->MatSpcR = (gcnew System::Windows::Forms::NumericUpDown());
	this->MatSpcZ = (gcnew System::Windows::Forms::NumericUpDown());
	this->MatSpcY = (gcnew System::Windows::Forms::NumericUpDown());
	this->MatSpcX = (gcnew System::Windows::Forms::NumericUpDown());
	this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
	this->MatDiffR = (gcnew System::Windows::Forms::NumericUpDown());
	this->MatDiffZ = (gcnew System::Windows::Forms::NumericUpDown());
	this->MatDiffY = (gcnew System::Windows::Forms::NumericUpDown());
	this->MatDiffX = (gcnew System::Windows::Forms::NumericUpDown());
	this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
	this->ModAmbR = (gcnew System::Windows::Forms::NumericUpDown());
	this->ModAmbZ = (gcnew System::Windows::Forms::NumericUpDown());
	this->ModAmbY = (gcnew System::Windows::Forms::NumericUpDown());
	this->ModAmbX = (gcnew System::Windows::Forms::NumericUpDown());
	this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
	this->numericUpDown17 = (gcnew System::Windows::Forms::NumericUpDown());
	this->numericUpDown18 = (gcnew System::Windows::Forms::NumericUpDown());
	this->numericUpDown19 = (gcnew System::Windows::Forms::NumericUpDown());
	this->numericUpDown20 = (gcnew System::Windows::Forms::NumericUpDown());
	this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
	this->lightposR1 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposZ1 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposY1 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposX1 = (gcnew System::Windows::Forms::NumericUpDown());
	this->label1 = (gcnew System::Windows::Forms::Label());
	this->textBox_Shine = (gcnew System::Windows::Forms::TextBox());
	this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
	this->lightposR4 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposZ4 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposY4 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposX4 = (gcnew System::Windows::Forms::NumericUpDown());
	this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
	this->lightposR2 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposZ2 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposY2 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposX2 = (gcnew System::Windows::Forms::NumericUpDown());
	this->groupBox9 = (gcnew System::Windows::Forms::GroupBox());
	this->lightposR3 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposZ3 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposY3 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposX3 = (gcnew System::Windows::Forms::NumericUpDown());
	this->groupBox10 = (gcnew System::Windows::Forms::GroupBox());
	this->lightposR5 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposZ5 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposY5 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposX5 = (gcnew System::Windows::Forms::NumericUpDown());
	this->groupBox12 = (gcnew System::Windows::Forms::GroupBox());
	this->lightposR6 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposZ6 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposY6 = (gcnew System::Windows::Forms::NumericUpDown());
	this->lightposX6 = (gcnew System::Windows::Forms::NumericUpDown());
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matambX))->BeginInit();
	this->groupBox1->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matambR))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matambZ))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matambY))->BeginInit();
	this->groupBox2->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatSpcR))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatSpcZ))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatSpcY))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatSpcX))->BeginInit();
	this->groupBox3->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatDiffR))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatDiffZ))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatDiffY))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatDiffX))->BeginInit();
	this->groupBox4->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ModAmbR))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ModAmbZ))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ModAmbY))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ModAmbX))->BeginInit();
	this->groupBox5->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown17))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown18))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown19))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown20))->BeginInit();
	this->groupBox6->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR1))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ1))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY1))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX1))->BeginInit();
	this->groupBox7->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR4))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ4))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY4))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX4))->BeginInit();
	this->groupBox8->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR2))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ2))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY2))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX2))->BeginInit();
	this->groupBox9->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR3))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ3))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY3))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX3))->BeginInit();
	this->groupBox10->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR5))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ5))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY5))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX5))->BeginInit();
	this->groupBox12->SuspendLayout();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR6))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ6))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY6))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX6))->BeginInit();
	this->SuspendLayout();
	// 
	// matambX
	// 
	this->matambX->Location = System::Drawing::Point(20, 38);
	this->matambX->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->matambX->Name = L"matambX";
	this->matambX->Size = System::Drawing::Size(49, 22);
	this->matambX->TabIndex = 0;
	// 
	// groupBox1
	// 
	this->groupBox1->Controls->Add(this->matambR);
	this->groupBox1->Controls->Add(this->matambZ);
	this->groupBox1->Controls->Add(this->matambY);
	this->groupBox1->Controls->Add(this->matambX);
	this->groupBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox1->Location = System::Drawing::Point(24, 61);
	this->groupBox1->Name = L"groupBox1";
	this->groupBox1->Size = System::Drawing::Size(271, 81);
	this->groupBox1->TabIndex = 1;
	this->groupBox1->TabStop = false;
	this->groupBox1->Text = L"Light Ambient";
	// 
	// matambR
	// 
	this->matambR->Location = System::Drawing::Point(200, 37);
	this->matambR->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->matambR->Name = L"matambR";
	this->matambR->Size = System::Drawing::Size(49, 22);
	this->matambR->TabIndex = 3;
	// 
	// matambZ
	// 
	this->matambZ->Location = System::Drawing::Point(139, 37);
	this->matambZ->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->matambZ->Name = L"matambZ";
	this->matambZ->Size = System::Drawing::Size(49, 22);
	this->matambZ->TabIndex = 2;
	// 
	// matambY
	// 
	this->matambY->Location = System::Drawing::Point(78, 37);
	this->matambY->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->matambY->Name = L"matambY";
	this->matambY->Size = System::Drawing::Size(49, 22);
	this->matambY->TabIndex = 1;
	// 
	// BtnUpdate
	// 
	this->BtnUpdate->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->BtnUpdate->Location = System::Drawing::Point(283, 533);
	this->BtnUpdate->Name = L"BtnUpdate";
	this->BtnUpdate->Size = System::Drawing::Size(420, 50);
	this->BtnUpdate->TabIndex = 4;
	this->BtnUpdate->Text = L"Update Graphics";
	this->BtnUpdate->UseVisualStyleBackColor = true;
	this->BtnUpdate->Click += gcnew System::EventHandler(this, &OglLighting::BtnUpdate_Click);
	// 
	// groupBox2
	// 
	this->groupBox2->Controls->Add(this->MatSpcR);
	this->groupBox2->Controls->Add(this->MatSpcZ);
	this->groupBox2->Controls->Add(this->MatSpcY);
	this->groupBox2->Controls->Add(this->MatSpcX);
	this->groupBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox2->Location = System::Drawing::Point(337, 61);
	this->groupBox2->Name = L"groupBox2";
	this->groupBox2->Size = System::Drawing::Size(271, 81);
	this->groupBox2->TabIndex = 4;
	this->groupBox2->TabStop = false;
	this->groupBox2->Text = L"Light Specular";
	// 
	// MatSpcR
	// 
	this->MatSpcR->Location = System::Drawing::Point(200, 37);
	this->MatSpcR->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->MatSpcR->Name = L"MatSpcR";
	this->MatSpcR->Size = System::Drawing::Size(49, 22);
	this->MatSpcR->TabIndex = 3;
	// 
	// MatSpcZ
	// 
	this->MatSpcZ->Location = System::Drawing::Point(139, 37);
	this->MatSpcZ->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->MatSpcZ->Name = L"MatSpcZ";
	this->MatSpcZ->Size = System::Drawing::Size(49, 22);
	this->MatSpcZ->TabIndex = 2;
	// 
	// MatSpcY
	// 
	this->MatSpcY->Location = System::Drawing::Point(78, 37);
	this->MatSpcY->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->MatSpcY->Name = L"MatSpcY";
	this->MatSpcY->Size = System::Drawing::Size(49, 22);
	this->MatSpcY->TabIndex = 1;
	// 
	// MatSpcX
	// 
	this->MatSpcX->Location = System::Drawing::Point(20, 38);
	this->MatSpcX->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->MatSpcX->Name = L"MatSpcX";
	this->MatSpcX->Size = System::Drawing::Size(49, 22);
	this->MatSpcX->TabIndex = 0;
	// 
	// groupBox3
	// 
	this->groupBox3->Controls->Add(this->MatDiffR);
	this->groupBox3->Controls->Add(this->MatDiffZ);
	this->groupBox3->Controls->Add(this->MatDiffY);
	this->groupBox3->Controls->Add(this->MatDiffX);
	this->groupBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox3->Location = System::Drawing::Point(646, 61);
	this->groupBox3->Name = L"groupBox3";
	this->groupBox3->Size = System::Drawing::Size(271, 81);
	this->groupBox3->TabIndex = 5;
	this->groupBox3->TabStop = false;
	this->groupBox3->Text = L"Light Diffuse";
	// 
	// MatDiffR
	// 
	this->MatDiffR->Location = System::Drawing::Point(200, 37);
	this->MatDiffR->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->MatDiffR->Name = L"MatDiffR";
	this->MatDiffR->Size = System::Drawing::Size(49, 22);
	this->MatDiffR->TabIndex = 3;
	// 
	// MatDiffZ
	// 
	this->MatDiffZ->Location = System::Drawing::Point(139, 37);
	this->MatDiffZ->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->MatDiffZ->Name = L"MatDiffZ";
	this->MatDiffZ->Size = System::Drawing::Size(49, 22);
	this->MatDiffZ->TabIndex = 2;
	// 
	// MatDiffY
	// 
	this->MatDiffY->Location = System::Drawing::Point(78, 37);
	this->MatDiffY->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->MatDiffY->Name = L"MatDiffY";
	this->MatDiffY->Size = System::Drawing::Size(49, 22);
	this->MatDiffY->TabIndex = 1;
	// 
	// MatDiffX
	// 
	this->MatDiffX->Location = System::Drawing::Point(20, 38);
	this->MatDiffX->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->MatDiffX->Name = L"MatDiffX";
	this->MatDiffX->Size = System::Drawing::Size(49, 22);
	this->MatDiffX->TabIndex = 0;
	// 
	// groupBox4
	// 
	this->groupBox4->Controls->Add(this->ModAmbR);
	this->groupBox4->Controls->Add(this->ModAmbZ);
	this->groupBox4->Controls->Add(this->ModAmbY);
	this->groupBox4->Controls->Add(this->ModAmbX);
	this->groupBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox4->Location = System::Drawing::Point(27, 400);
	this->groupBox4->Name = L"groupBox4";
	this->groupBox4->Size = System::Drawing::Size(271, 81);
	this->groupBox4->TabIndex = 6;
	this->groupBox4->TabStop = false;
	this->groupBox4->Text = L"Model Ambient";
	// 
	// ModAmbR
	// 
	this->ModAmbR->Location = System::Drawing::Point(200, 37);
	this->ModAmbR->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->ModAmbR->Name = L"ModAmbR";
	this->ModAmbR->Size = System::Drawing::Size(49, 22);
	this->ModAmbR->TabIndex = 3;
	// 
	// ModAmbZ
	// 
	this->ModAmbZ->Location = System::Drawing::Point(139, 37);
	this->ModAmbZ->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->ModAmbZ->Name = L"ModAmbZ";
	this->ModAmbZ->Size = System::Drawing::Size(49, 22);
	this->ModAmbZ->TabIndex = 2;
	// 
	// ModAmbY
	// 
	this->ModAmbY->Location = System::Drawing::Point(78, 37);
	this->ModAmbY->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->ModAmbY->Name = L"ModAmbY";
	this->ModAmbY->Size = System::Drawing::Size(49, 22);
	this->ModAmbY->TabIndex = 1;
	// 
	// ModAmbX
	// 
	this->ModAmbX->Location = System::Drawing::Point(20, 38);
	this->ModAmbX->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->ModAmbX->Name = L"ModAmbX";
	this->ModAmbX->Size = System::Drawing::Size(49, 22);
	this->ModAmbX->TabIndex = 0;
	// 
	// groupBox5
	// 
	this->groupBox5->Controls->Add(this->numericUpDown17);
	this->groupBox5->Controls->Add(this->numericUpDown18);
	this->groupBox5->Controls->Add(this->numericUpDown19);
	this->groupBox5->Controls->Add(this->numericUpDown20);
	this->groupBox5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox5->Location = System::Drawing::Point(337, 400);
	this->groupBox5->Name = L"groupBox5";
	this->groupBox5->Size = System::Drawing::Size(271, 81);
	this->groupBox5->TabIndex = 7;
	this->groupBox5->TabStop = false;
	this->groupBox5->Text = L"Nothing";
	// 
	// numericUpDown17
	// 
	this->numericUpDown17->Location = System::Drawing::Point(200, 37);
	this->numericUpDown17->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->numericUpDown17->Name = L"numericUpDown17";
	this->numericUpDown17->Size = System::Drawing::Size(49, 22);
	this->numericUpDown17->TabIndex = 3;
	// 
	// numericUpDown18
	// 
	this->numericUpDown18->Location = System::Drawing::Point(139, 37);
	this->numericUpDown18->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->numericUpDown18->Name = L"numericUpDown18";
	this->numericUpDown18->Size = System::Drawing::Size(49, 22);
	this->numericUpDown18->TabIndex = 2;
	// 
	// numericUpDown19
	// 
	this->numericUpDown19->Location = System::Drawing::Point(78, 37);
	this->numericUpDown19->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->numericUpDown19->Name = L"numericUpDown19";
	this->numericUpDown19->Size = System::Drawing::Size(49, 22);
	this->numericUpDown19->TabIndex = 1;
	// 
	// numericUpDown20
	// 
	this->numericUpDown20->Location = System::Drawing::Point(20, 38);
	this->numericUpDown20->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->numericUpDown20->Name = L"numericUpDown20";
	this->numericUpDown20->Size = System::Drawing::Size(49, 22);
	this->numericUpDown20->TabIndex = 0;
	// 
	// groupBox6
	// 
	this->groupBox6->Controls->Add(this->lightposR1);
	this->groupBox6->Controls->Add(this->lightposZ1);
	this->groupBox6->Controls->Add(this->lightposY1);
	this->groupBox6->Controls->Add(this->lightposX1);
	this->groupBox6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox6->Location = System::Drawing::Point(27, 177);
	this->groupBox6->Name = L"groupBox6";
	this->groupBox6->Size = System::Drawing::Size(271, 81);
	this->groupBox6->TabIndex = 8;
	this->groupBox6->TabStop = false;
	this->groupBox6->Text = L"Light Position1";
	// 
	// lightposR1
	// 
	this->lightposR1->Location = System::Drawing::Point(200, 37);
	this->lightposR1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposR1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposR1->Name = L"lightposR1";
	this->lightposR1->Size = System::Drawing::Size(49, 22);
	this->lightposR1->TabIndex = 3;
	// 
	// lightposZ1
	// 
	this->lightposZ1->Location = System::Drawing::Point(139, 37);
	this->lightposZ1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposZ1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposZ1->Name = L"lightposZ1";
	this->lightposZ1->Size = System::Drawing::Size(49, 22);
	this->lightposZ1->TabIndex = 2;
	// 
	// lightposY1
	// 
	this->lightposY1->Location = System::Drawing::Point(78, 37);
	this->lightposY1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposY1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposY1->Name = L"lightposY1";
	this->lightposY1->Size = System::Drawing::Size(49, 22);
	this->lightposY1->TabIndex = 1;
	// 
	// lightposX1
	// 
	this->lightposX1->Location = System::Drawing::Point(20, 38);
	this->lightposX1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposX1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposX1->Name = L"lightposX1";
	this->lightposX1->Size = System::Drawing::Size(49, 22);
	this->lightposX1->TabIndex = 0;
	// 
	// label1
	// 
	this->label1->AutoSize = true;
	this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->label1->Location = System::Drawing::Point(24, 506);
	this->label1->Name = L"label1";
	this->label1->Size = System::Drawing::Size(75, 16);
	this->label1->TabIndex = 9;
	this->label1->Text = L"Shininess";
	// 
	// textBox_Shine
	// 
	this->textBox_Shine->Location = System::Drawing::Point(166, 502);
	this->textBox_Shine->Name = L"textBox_Shine";
	this->textBox_Shine->Size = System::Drawing::Size(100, 20);
	this->textBox_Shine->TabIndex = 10;
	this->textBox_Shine->Text = L"0";
	// 
	// groupBox7
	// 
	this->groupBox7->Controls->Add(this->lightposR4);
	this->groupBox7->Controls->Add(this->lightposZ4);
	this->groupBox7->Controls->Add(this->lightposY4);
	this->groupBox7->Controls->Add(this->lightposX4);
	this->groupBox7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox7->Location = System::Drawing::Point(27, 285);
	this->groupBox7->Name = L"groupBox7";
	this->groupBox7->Size = System::Drawing::Size(271, 81);
	this->groupBox7->TabIndex = 9;
	this->groupBox7->TabStop = false;
	this->groupBox7->Text = L"Light Position4";
	// 
	// lightposR4
	// 
	this->lightposR4->Location = System::Drawing::Point(200, 37);
	this->lightposR4->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposR4->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposR4->Name = L"lightposR4";
	this->lightposR4->Size = System::Drawing::Size(49, 22);
	this->lightposR4->TabIndex = 3;
	// 
	// lightposZ4
	// 
	this->lightposZ4->Location = System::Drawing::Point(139, 37);
	this->lightposZ4->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposZ4->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposZ4->Name = L"lightposZ4";
	this->lightposZ4->Size = System::Drawing::Size(49, 22);
	this->lightposZ4->TabIndex = 2;
	// 
	// lightposY4
	// 
	this->lightposY4->Location = System::Drawing::Point(78, 37);
	this->lightposY4->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposY4->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposY4->Name = L"lightposY4";
	this->lightposY4->Size = System::Drawing::Size(49, 22);
	this->lightposY4->TabIndex = 1;
	// 
	// lightposX4
	// 
	this->lightposX4->Location = System::Drawing::Point(20, 38);
	this->lightposX4->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposX4->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposX4->Name = L"lightposX4";
	this->lightposX4->Size = System::Drawing::Size(49, 22);
	this->lightposX4->TabIndex = 0;
	// 
	// groupBox8
	// 
	this->groupBox8->Controls->Add(this->lightposR2);
	this->groupBox8->Controls->Add(this->lightposZ2);
	this->groupBox8->Controls->Add(this->lightposY2);
	this->groupBox8->Controls->Add(this->lightposX2);
	this->groupBox8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox8->Location = System::Drawing::Point(337, 177);
	this->groupBox8->Name = L"groupBox8";
	this->groupBox8->Size = System::Drawing::Size(271, 81);
	this->groupBox8->TabIndex = 9;
	this->groupBox8->TabStop = false;
	this->groupBox8->Text = L"Light Position2";
	// 
	// lightposR2
	// 
	this->lightposR2->Location = System::Drawing::Point(200, 37);
	this->lightposR2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposR2->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposR2->Name = L"lightposR2";
	this->lightposR2->Size = System::Drawing::Size(49, 22);
	this->lightposR2->TabIndex = 3;
	// 
	// lightposZ2
	// 
	this->lightposZ2->Location = System::Drawing::Point(139, 37);
	this->lightposZ2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposZ2->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposZ2->Name = L"lightposZ2";
	this->lightposZ2->Size = System::Drawing::Size(49, 22);
	this->lightposZ2->TabIndex = 2;
	// 
	// lightposY2
	// 
	this->lightposY2->Location = System::Drawing::Point(78, 37);
	this->lightposY2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposY2->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposY2->Name = L"lightposY2";
	this->lightposY2->Size = System::Drawing::Size(49, 22);
	this->lightposY2->TabIndex = 1;
	// 
	// lightposX2
	// 
	this->lightposX2->Location = System::Drawing::Point(20, 38);
	this->lightposX2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposX2->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposX2->Name = L"lightposX2";
	this->lightposX2->Size = System::Drawing::Size(49, 22);
	this->lightposX2->TabIndex = 0;
	// 
	// groupBox9
	// 
	this->groupBox9->Controls->Add(this->lightposR3);
	this->groupBox9->Controls->Add(this->lightposZ3);
	this->groupBox9->Controls->Add(this->lightposY3);
	this->groupBox9->Controls->Add(this->lightposX3);
	this->groupBox9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox9->Location = System::Drawing::Point(646, 177);
	this->groupBox9->Name = L"groupBox9";
	this->groupBox9->Size = System::Drawing::Size(271, 81);
	this->groupBox9->TabIndex = 9;
	this->groupBox9->TabStop = false;
	this->groupBox9->Text = L"Light Position3";
	// 
	// lightposR3
	// 
	this->lightposR3->Location = System::Drawing::Point(200, 37);
	this->lightposR3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposR3->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposR3->Name = L"lightposR3";
	this->lightposR3->Size = System::Drawing::Size(49, 22);
	this->lightposR3->TabIndex = 3;
	// 
	// lightposZ3
	// 
	this->lightposZ3->Location = System::Drawing::Point(139, 37);
	this->lightposZ3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposZ3->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposZ3->Name = L"lightposZ3";
	this->lightposZ3->Size = System::Drawing::Size(49, 22);
	this->lightposZ3->TabIndex = 2;
	// 
	// lightposY3
	// 
	this->lightposY3->Location = System::Drawing::Point(78, 37);
	this->lightposY3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposY3->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposY3->Name = L"lightposY3";
	this->lightposY3->Size = System::Drawing::Size(49, 22);
	this->lightposY3->TabIndex = 1;
	// 
	// lightposX3
	// 
	this->lightposX3->Location = System::Drawing::Point(20, 38);
	this->lightposX3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposX3->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposX3->Name = L"lightposX3";
	this->lightposX3->Size = System::Drawing::Size(49, 22);
	this->lightposX3->TabIndex = 0;
	// 
	// groupBox10
	// 
	this->groupBox10->Controls->Add(this->lightposR5);
	this->groupBox10->Controls->Add(this->lightposZ5);
	this->groupBox10->Controls->Add(this->lightposY5);
	this->groupBox10->Controls->Add(this->lightposX5);
	this->groupBox10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox10->Location = System::Drawing::Point(337, 285);
	this->groupBox10->Name = L"groupBox10";
	this->groupBox10->Size = System::Drawing::Size(271, 81);
	this->groupBox10->TabIndex = 9;
	this->groupBox10->TabStop = false;
	this->groupBox10->Text = L"Light Position5";
	// 
	// lightposR5
	// 
	this->lightposR5->Location = System::Drawing::Point(200, 37);
	this->lightposR5->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposR5->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposR5->Name = L"lightposR5";
	this->lightposR5->Size = System::Drawing::Size(49, 22);
	this->lightposR5->TabIndex = 3;
	// 
	// lightposZ5
	// 
	this->lightposZ5->Location = System::Drawing::Point(139, 37);
	this->lightposZ5->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposZ5->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposZ5->Name = L"lightposZ5";
	this->lightposZ5->Size = System::Drawing::Size(49, 22);
	this->lightposZ5->TabIndex = 2;
	// 
	// lightposY5
	// 
	this->lightposY5->Location = System::Drawing::Point(78, 37);
	this->lightposY5->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposY5->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposY5->Name = L"lightposY5";
	this->lightposY5->Size = System::Drawing::Size(49, 22);
	this->lightposY5->TabIndex = 1;
	// 
	// lightposX5
	// 
	this->lightposX5->Location = System::Drawing::Point(20, 38);
	this->lightposX5->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposX5->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposX5->Name = L"lightposX5";
	this->lightposX5->Size = System::Drawing::Size(49, 22);
	this->lightposX5->TabIndex = 0;
	// 
	// groupBox12
	// 
	this->groupBox12->Controls->Add(this->lightposR6);
	this->groupBox12->Controls->Add(this->lightposZ6);
	this->groupBox12->Controls->Add(this->lightposY6);
	this->groupBox12->Controls->Add(this->lightposX6);
	this->groupBox12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(0)));
	this->groupBox12->Location = System::Drawing::Point(646, 285);
	this->groupBox12->Name = L"groupBox12";
	this->groupBox12->Size = System::Drawing::Size(271, 81);
	this->groupBox12->TabIndex = 9;
	this->groupBox12->TabStop = false;
	this->groupBox12->Text = L"Light Position6";
	// 
	// lightposR6
	// 
	this->lightposR6->Location = System::Drawing::Point(200, 37);
	this->lightposR6->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposR6->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposR6->Name = L"lightposR6";
	this->lightposR6->Size = System::Drawing::Size(49, 22);
	this->lightposR6->TabIndex = 3;
	// 
	// lightposZ6
	// 
	this->lightposZ6->Location = System::Drawing::Point(139, 37);
	this->lightposZ6->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposZ6->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposZ6->Name = L"lightposZ6";
	this->lightposZ6->Size = System::Drawing::Size(49, 22);
	this->lightposZ6->TabIndex = 2;
	// 
	// lightposY6
	// 
	this->lightposY6->Location = System::Drawing::Point(78, 37);
	this->lightposY6->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposY6->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposY6->Name = L"lightposY6";
	this->lightposY6->Size = System::Drawing::Size(49, 22);
	this->lightposY6->TabIndex = 1;
	// 
	// lightposX6
	// 
	this->lightposX6->Location = System::Drawing::Point(20, 38);
	this->lightposX6->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, 0});
	this->lightposX6->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {255, 0, 0, System::Int32::MinValue});
	this->lightposX6->Name = L"lightposX6";
	this->lightposX6->Size = System::Drawing::Size(49, 22);
	this->lightposX6->TabIndex = 0;
	// 
	// OglLighting
	// 
	this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	this->ClientSize = System::Drawing::Size(957, 632);
	this->Controls->Add(this->groupBox12);
	this->Controls->Add(this->groupBox10);
	this->Controls->Add(this->groupBox7);
	this->Controls->Add(this->groupBox9);
	this->Controls->Add(this->groupBox8);
	this->Controls->Add(this->textBox_Shine);
	this->Controls->Add(this->label1);
	this->Controls->Add(this->groupBox6);
	this->Controls->Add(this->groupBox5);
	this->Controls->Add(this->groupBox4);
	this->Controls->Add(this->groupBox3);
	this->Controls->Add(this->groupBox2);
	this->Controls->Add(this->BtnUpdate);
	this->Controls->Add(this->groupBox1);
	this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
	this->MaximizeBox = false;
	this->Name = L"OglLighting";
	this->Text = L"Rapid-I";
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matambX))->EndInit();
	this->groupBox1->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matambR))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matambZ))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->matambY))->EndInit();
	this->groupBox2->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatSpcR))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatSpcZ))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatSpcY))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatSpcX))->EndInit();
	this->groupBox3->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatDiffR))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatDiffZ))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatDiffY))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MatDiffX))->EndInit();
	this->groupBox4->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ModAmbR))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ModAmbZ))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ModAmbY))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ModAmbX))->EndInit();
	this->groupBox5->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown17))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown18))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown19))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown20))->EndInit();
	this->groupBox6->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR1))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ1))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY1))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX1))->EndInit();
	this->groupBox7->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR4))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ4))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY4))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX4))->EndInit();
	this->groupBox8->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR2))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ2))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY2))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX2))->EndInit();
	this->groupBox9->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR3))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ3))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY3))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX3))->EndInit();
	this->groupBox10->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR5))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ5))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY5))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX5))->EndInit();
	this->groupBox12->ResumeLayout(false);
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposR6))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposZ6))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposY6))->EndInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->lightposX6))->EndInit();
	this->ResumeLayout(false);
	this->PerformLayout();

}


void RWrapper::OglLighting::BtnUpdate_Click(System::Object^  sender, System::EventArgs^  e)
{
	double matx = (double)matambX->Value/255, maty = (double)matambY->Value/255, matz = (double)matambZ->Value/255, matr = (double)matambR->Value/255;
	GRAFIX->SetMaterial_Ambient(matx, maty, matz, matr);

	double matspx = (double)MatSpcX->Value/255, matspy = (double)MatSpcY->Value/255, matspz = (double)MatSpcZ->Value/255, matspr = (double)MatSpcR->Value/255;
	GRAFIX->SetMaterial_Specular(matspx, matspy, matspz, matspr);

	double matdiffx = (double)MatDiffZ->Value/255, matdiffy = (double)MatDiffY->Value/255, matdiffz = (double)MatDiffZ->Value/255, matdiffr = (double)MatDiffR->Value/255;
	GRAFIX->SetMaterial_Diffuse(matdiffx, matdiffy, matdiffz, matdiffr);

	double modx = (double)ModAmbX->Value/255, mody = (double)ModAmbY->Value/255, modz = (double)ModAmbZ->Value/255, modr = (double)ModAmbR->Value/255;
	GRAFIX->SetModel_Ambient(modx, mody, modz, modr);

	/*double Lposx1 = (double)lightposX1->Value/255, Lposy1 = (double)lightposY1->Value/255, Lposz1 = (double)lightposZ1->Value/255, Lposr1 = (double)lightposR1->Value/255;
	GRAFIX->SetLight_Position1(Lposx1, Lposy1, Lposz1, Lposr1);

	double Lposx2 = (double)lightposX2->Value/255, Lposy2 = (double)lightposY2->Value/255, Lposz2 = (double)lightposZ2->Value/255, Lposr2 = (double)lightposR2->Value/255;
	GRAFIX->SetLight_Position2(Lposx2, Lposy2, Lposz2, Lposr2);

	double Lposx3 = (double)lightposX3->Value/255, Lposy3 = (double)lightposY3->Value/255, Lposz3 = (double)lightposZ3->Value/255, Lposr3 = (double)lightposR3->Value/255;
	GRAFIX->SetLight_Position3(Lposx3, Lposy3, Lposz3, Lposr3);

	double Lposx4 = (double)lightposX4->Value/255, Lposy4 = (double)lightposY4->Value/255, Lposz4 = (double)lightposZ4->Value/255, Lposr4 = (double)lightposR4->Value/255;
	GRAFIX->SetLight_Position4(Lposx4, Lposy4, Lposz4, Lposr4);

	double Lposx5 = (double)lightposX5->Value/255, Lposy5 = (double)lightposY5->Value/255, Lposz5 = (double)lightposZ5->Value/255, Lposr5 = (double)lightposR5->Value/255;
	GRAFIX->SetLight_Position5(Lposx5, Lposy5, Lposz5, Lposr5);

	double Lposx6 = (double)lightposX6->Value/255, Lposy6 = (double)lightposY6->Value/255, Lposz6 = (double)lightposZ6->Value/255, Lposr6 = (double)lightposR6->Value/255;
	GRAFIX->SetLight_Position6(Lposx6, Lposy6, Lposz6, Lposr6);*/

	double Lposx1 = (double)lightposX1->Value, Lposy1 = (double)lightposY1->Value, Lposz1 = (double)lightposZ1->Value, Lposr1 = (double)lightposR1->Value/255;
	GRAFIX->SetLight_Position1(Lposx1, Lposy1, Lposz1, Lposr1);

	double Lposx2 = (double)lightposX2->Value, Lposy2 = (double)lightposY2->Value, Lposz2 = (double)lightposZ2->Value, Lposr2 = (double)lightposR2->Value/255;
	GRAFIX->SetLight_Position2(Lposx2, Lposy2, Lposz2, Lposr2);

	double Lposx3 = (double)lightposX3->Value, Lposy3 = (double)lightposY3->Value, Lposz3 = (double)lightposZ3->Value, Lposr3 = (double)lightposR3->Value/255;
	GRAFIX->SetLight_Position3(Lposx3, Lposy3, Lposz3, Lposr3);

	double Lposx4 = (double)lightposX4->Value, Lposy4 = (double)lightposY4->Value, Lposz4 = (double)lightposZ4->Value, Lposr4 = (double)lightposR4->Value/255;
	GRAFIX->SetLight_Position4(Lposx4, Lposy4, Lposz4, Lposr4);

	double Lposx5 = (double)lightposX5->Value, Lposy5 = (double)lightposY5->Value, Lposz5 = (double)lightposZ5->Value, Lposr5 = (double)lightposR5->Value/255;
	GRAFIX->SetLight_Position5(Lposx5, Lposy5, Lposz5, Lposr5);

	double Lposx6 = (double)lightposX6->Value, Lposy6 = (double)lightposY6->Value, Lposz6 = (double)lightposZ6->Value, Lposr6 = (double)lightposR6->Value/255;
	GRAFIX->SetLight_Position6(Lposx6, Lposy6, Lposz6, Lposr6);

	double Shnes = System::Convert::ToDouble(textBox_Shine->Text);
	GRAFIX->SetMaterial_Shininess(Shnes);
	MAINDllOBJECT->Shape_Updated();
}