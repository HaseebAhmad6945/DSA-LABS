#pragma once
// ============================================================
//  CreateAccountForm.h
// ============================================================
#include "MainForm.h"
#include <msclr/marshal_cppstd.h>

namespace BankingApp {

    public ref class CreateAccountForm : public Form {
    public:
        CreateAccountForm() { InitializeComponent(); }

    private:
        RadioButton^ rbSavings;
        RadioButton^ rbCurrent;
        TextBox^     txtName;
        TextBox^     txtBalance;
        TextBox^     txtExtra;
        TextBox^     txtOverdraft;
        CheckBox^    chkVip;
        CheckBox^    chkWithdraw;
        CheckBox^    chkDeposit;
        CheckBox^    chkTransfer;
        Label^       lblExtra;
        Label^       lblResult;

        // No default params in managed type member functions
        Label^ MkLbl(String^ text, int x, int y) {
            Label^ l = gcnew Label();
            l->Text      = text;
            l->Location  = Point(x, y);
            l->AutoSize  = true;
            l->ForeColor = Theme::SubText();
            l->Font      = Theme::SmallFont();
            return l;
        }
        TextBox^ MkTxt(int x, int y, int w) {
            TextBox^ t = gcnew TextBox();
            t->Location    = Point(x, y);
            t->Size        = System::Drawing::Size(w, 28);
            t->BackColor   = Theme::CardBg();
            t->ForeColor   = Theme::LightText();
            t->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            t->Font        = Theme::BodyFont();
            return t;
        }
        CheckBox^ MkChk(String^ text, int x, int y) {
            CheckBox^ c = gcnew CheckBox();
            c->Text      = text;
            c->Location  = Point(x, y);
            c->ForeColor = Theme::LightText();
            c->Checked   = true;
            return c;
        }

        void InitializeComponent() {
            this->Text            = L"Create New Account";
            this->Size            = System::Drawing::Size(460, 590);
            this->BackColor       = Theme::NavyBg();
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox     = false;
            this->StartPosition   = FormStartPosition::CenterParent;
            this->Font            = Theme::BodyFont();

            Label^ lblH = gcnew Label();
            lblH->Text      = L"Create New Account";
            lblH->Font      = Theme::HeadFont();
            lblH->ForeColor = Theme::AccentGold();
            lblH->Location  = Point(20, 14);
            lblH->AutoSize  = true;

            Panel^ pnlType = gcnew Panel();
            pnlType->Location  = Point(16, 50);
            pnlType->Size      = System::Drawing::Size(420, 42);
            pnlType->BackColor = Theme::DarkBg();

            rbSavings = gcnew RadioButton();
            rbSavings->Text      = L"Savings Account";
            rbSavings->Location  = Point(10, 10);
            rbSavings->ForeColor = Theme::LightText();
            rbSavings->Checked   = true;
            rbSavings->CheckedChanged += gcnew EventHandler(this, &CreateAccountForm::TypeChanged);

            rbCurrent = gcnew RadioButton();
            rbCurrent->Text      = L"Current Account";
            rbCurrent->Location  = Point(190, 10);
            rbCurrent->ForeColor = Theme::LightText();
            rbCurrent->CheckedChanged += gcnew EventHandler(this, &CreateAccountForm::TypeChanged);

            pnlType->Controls->Add(rbSavings);
            pnlType->Controls->Add(rbCurrent);

            int y = 104;
            Controls->Add(MkLbl(L"Full Name:", 16, y));
            txtName = MkTxt(16, y + 18, 422);

            y += 54;
            Controls->Add(MkLbl(L"Initial Balance (PKR):", 16, y));
            txtBalance = MkTxt(16, y + 18, 180);

            y += 54;
            lblExtra = MkLbl(L"Interest Rate (% p.a.):", 16, y);
            txtExtra  = MkTxt(16, y + 18, 180);
            txtExtra->Text = L"7.5";

            y += 54;
            Controls->Add(MkLbl(L"Overdraft Limit (PKR):", 16, y));
            txtOverdraft = MkTxt(16, y + 18, 180);
            txtOverdraft->Text = L"0";

            y += 60;
            Label^ lblPermHead = gcnew Label();
            lblPermHead->Text      = L"PERMISSIONS";
            lblPermHead->Font      = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold);
            lblPermHead->ForeColor = Theme::AccentCyan();
            lblPermHead->Location  = Point(16, y);
            lblPermHead->AutoSize  = true;

            y += 22;
            chkWithdraw = MkChk(L"Withdraw (1)", 16,  y);
            chkDeposit  = MkChk(L"Deposit (2)",  162, y);
            chkTransfer = MkChk(L"Transfer (4)", 310, y);

            y += 30;
            chkVip = gcnew CheckBox();
            chkVip->Text      = L"VIP Account (8)";
            chkVip->Location  = Point(16, y);
            chkVip->ForeColor = Theme::AccentGold();

            y += 40;
            lblResult = gcnew Label();
            lblResult->Location  = Point(16, y);
            lblResult->Size      = System::Drawing::Size(422, 24);
            lblResult->Font      = Theme::SmallFont();
            lblResult->TextAlign = ContentAlignment::MiddleCenter;

            y += 30;
            Button^ btnCreate = MakeBtn(L"Create Account", Theme::SuccessGrn(), Color::Black, 16, y, 205, 42);
            btnCreate->Click += gcnew EventHandler(this, &CreateAccountForm::BtnCreate_Click);

            Button^ btnCancel = MakeBtn(L"Cancel", Theme::DangerRed(), Theme::LightText(), 237, y, 205, 42);
            btnCancel->Click += gcnew EventHandler(this, &CreateAccountForm::BtnCancel_Click);

            Controls->Add(lblH);
            Controls->Add(pnlType);
            Controls->Add(txtName);
            Controls->Add(txtBalance);
            Controls->Add(lblExtra);
            Controls->Add(txtExtra);
            Controls->Add(txtOverdraft);
            Controls->Add(lblPermHead);
            Controls->Add(chkWithdraw);
            Controls->Add(chkDeposit);
            Controls->Add(chkTransfer);
            Controls->Add(chkVip);
            Controls->Add(lblResult);
            Controls->Add(btnCreate);
            Controls->Add(btnCancel);
        }

        void TypeChanged(Object^ /*s*/, EventArgs^ /*e*/) {
            if (rbSavings->Checked) {
                lblExtra->Text     = L"Interest Rate (% p.a.):";
                txtExtra->Text     = L"7.5";
                txtOverdraft->Text = L"0";
            } else {
                lblExtra->Text     = L"Transaction Fee per Withdrawal (PKR):";
                txtExtra->Text     = L"25";
                txtOverdraft->Text = L"5000";
            }
        }

        void BtnCreate_Click(Object^ /*s*/, EventArgs^ /*e*/) {
            String^ name = txtName->Text->Trim();
            if (String::IsNullOrEmpty(name)) { ShowMsg(L"Please enter a name.", false); return; }

            double bal, extra, od;
            if (!Double::TryParse(txtBalance->Text,   bal)   || bal  < 0 ||
                !Double::TryParse(txtExtra->Text,     extra) || extra < 0 ||
                !Double::TryParse(txtOverdraft->Text, od)    || od   < 0) {
                ShowMsg(L"Invalid numeric value.", false); return;
            }

            unsigned int perm = 0u;
            if (chkWithdraw->Checked) perm |= Perm::WITHDRAW;
            if (chkDeposit->Checked)  perm |= Perm::DEPOSIT;
            if (chkTransfer->Checked) perm |= Perm::TRANSFER;
            if (chkVip->Checked)      perm |= Perm::VIP;

            std::string nm = msclr::interop::marshal_as<std::string>(name);
            int id;
            if (rbSavings->Checked)
                id = g_bank->createSavings(nm, bal, perm, extra, od);
            else
                id = g_bank->createCurrent(nm, bal, perm, extra, od);

            ShowMsg(String::Format(L"Account created! ID: {0}", id), true);
            System::Threading::Thread::Sleep(800);
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }

        void BtnCancel_Click(Object^ /*s*/, EventArgs^ /*e*/) {
            this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->Close();
        }

        void ShowMsg(String^ msg, bool ok) {
            lblResult->ForeColor = ok ? Theme::SuccessGrn() : Theme::DangerRed();
            lblResult->Text      = msg;
        }
    };

} // namespace BankingApp
