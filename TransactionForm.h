#pragma once
// ============================================================
//  TransactionForm.h  - Deposit / Withdraw / Transfer
// ============================================================
#include "MainForm.h"

namespace BankingApp {

    public enum class TxMode { Deposit, Withdraw, Transfer };

    public ref class TransactionForm : public Form {
        TxMode   mode;
        TextBox^ txtFromId;
        TextBox^ txtToId;
        TextBox^ txtAmount;
        Label^   lblToId;
        Label^   lblResult;

    public:
        TransactionForm(TxMode m) : mode(m) { InitializeComponent(); }

    private:
        Label^ MkLbl(String^ text, int x, int y) {
            Label^ l = gcnew Label();
            l->Text      = text;
            l->Location  = Point(x, y);
            l->AutoSize  = true;
            l->ForeColor = Theme::SubText();
            l->Font      = Theme::SmallFont();
            return l;
        }
        // No default params in managed methods - always pass explicit width
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

        void InitializeComponent() {
            String^ title;
            Color   btnFg;
            Color   btnBg;
            switch (mode) {
                case TxMode::Deposit:
                    title = L"Deposit Funds";  btnBg = Theme::SuccessGrn(); btnFg = Color::Black; break;
                case TxMode::Withdraw:
                    title = L"Withdraw Funds"; btnBg = Theme::DangerRed();  btnFg = Theme::LightText(); break;
                default:
                    title = L"Transfer Funds"; btnBg = Color::FromArgb(30,130,200); btnFg = Color::Black; break;
            }

            this->Text            = title;
            this->Size            = System::Drawing::Size(460, 380);
            this->BackColor       = Theme::NavyBg();
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox     = false;
            this->StartPosition   = FormStartPosition::CenterParent;

            Label^ lblH = gcnew Label();
            lblH->Text      = title;
            lblH->Font      = Theme::HeadFont();
            lblH->ForeColor = Theme::AccentGold();
            lblH->Location  = Point(20, 14);
            lblH->AutoSize  = true;

            int y = 56;
            String^ fromLbl = (mode == TxMode::Transfer) ? L"From Account ID:" : L"Account ID:";
            Controls->Add(MkLbl(fromLbl, 20, y));
            txtFromId = MkTxt(20, y + 18, 400);

            y += 56;
            lblToId = MkLbl(L"To Account ID:", 20, y);
            txtToId = MkTxt(20, y + 18, 400);
            lblToId->Visible = (mode == TxMode::Transfer);
            txtToId->Visible = (mode == TxMode::Transfer);

            y += 56;
            Controls->Add(MkLbl(L"Amount (PKR):", 20, y));
            txtAmount = MkTxt(20, y + 18, 400);

            y += 56;
            lblResult = gcnew Label();
            lblResult->Location  = Point(20, y);
            lblResult->Size      = System::Drawing::Size(400, 24);
            lblResult->Font      = Theme::SmallFont();
            lblResult->TextAlign = ContentAlignment::MiddleCenter;

            y += 32;
            Button^ btnOk = MakeBtn(title, btnBg, btnFg, 20, y, 196, 42);
            btnOk->Click += gcnew EventHandler(this, &TransactionForm::BtnOk_Click);

            Button^ btnCancel = MakeBtn(L"Cancel", Theme::DangerRed(), Theme::LightText(), 234, y, 196, 42);
            btnCancel->Click += gcnew EventHandler(this, &TransactionForm::BtnCancel_Click);

            Controls->Add(lblH);
            Controls->Add(txtFromId);
            Controls->Add(lblToId);
            Controls->Add(txtToId);
            Controls->Add(txtAmount);
            Controls->Add(lblResult);
            Controls->Add(btnOk);
            Controls->Add(btnCancel);
        }

        void BtnOk_Click(Object^ /*s*/, EventArgs^ /*e*/) {
            int fromId, toId = 0;
            double amount;
            if (!Int32::TryParse(txtFromId->Text->Trim(), fromId)) {
                ShowMsg(L"Invalid account ID.", false); return;
            }
            if (mode == TxMode::Transfer && !Int32::TryParse(txtToId->Text->Trim(), toId)) {
                ShowMsg(L"Invalid destination ID.", false); return;
            }
            if (!Double::TryParse(txtAmount->Text->Trim(), amount) || amount <= 0) {
                ShowMsg(L"Enter a valid positive amount.", false); return;
            }

            std::string result;
            switch (mode) {
                case TxMode::Deposit:  result = g_bank->deposit (fromId, amount);       break;
                case TxMode::Withdraw: result = g_bank->withdraw(fromId, amount);       break;
                default:               result = g_bank->transfer(fromId, toId, amount); break;
            }

            if (result.empty()) {
                ShowMsg(L"Operation successful!", true);
                System::Threading::Thread::Sleep(600);
                this->DialogResult = System::Windows::Forms::DialogResult::OK;
                this->Close();
            } else {
                ShowMsg(gcnew String(result.c_str()), false);
            }
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
