#pragma once
// ============================================================
//  AdminForm.h  - Admin Mode (password-protected)
// ============================================================
#include "MainForm.h"

namespace BankingApp {

    public ref class AdminForm : public Form {
        Panel^    pnlLogin;
        Panel^    pnlAdmin;
        TextBox^  txtPass;
        ComboBox^ cboAccount;
        CheckBox^ chkW;
        CheckBox^ chkD;
        CheckBox^ chkT;
        CheckBox^ chkV;
        CheckBox^ chkA;
        CheckBox^ chkEncrypt;
        Label^    lblMsg;

        literal String^ ADMIN_PASS = L"admin123";

    public:
        AdminForm() { InitializeComponent(); }

    private:
        Label^ MkLbl(String^ t, int x, int y, Color c) {
            Label^ l = gcnew Label();
            l->Text      = t;
            l->Location  = Point(x, y);
            l->AutoSize  = true;
            l->ForeColor = c;
            l->Font      = Theme::BodyFont();
            return l;
        }
        // No default params in managed methods
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
        TextBox^ MkPwdTxt(int x, int y, int w) {
            TextBox^ t = MkTxt(x, y, w);
            t->PasswordChar = '*';
            return t;
        }
        CheckBox^ MkChk(String^ t, int x, int y) {
            CheckBox^ c = gcnew CheckBox();
            c->Text      = t;
            c->Location  = Point(x, y);
            c->ForeColor = Theme::LightText();
            c->Font      = Theme::BodyFont();
            return c;
        }

        void InitializeComponent() {
            this->Text            = L"Admin Mode";
            this->Size            = System::Drawing::Size(500, 560);
            this->BackColor       = Theme::NavyBg();
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox     = false;
            this->StartPosition   = FormStartPosition::CenterParent;
            this->Font            = Theme::BodyFont();

            // Login Panel
            pnlLogin = gcnew Panel();
            pnlLogin->Dock      = DockStyle::Fill;
            pnlLogin->BackColor = Theme::NavyBg();

            Label^ lh = gcnew Label();
            lh->Text      = L"ADMIN MODE";
            lh->Font      = Theme::TitleFont();
            lh->ForeColor = Theme::DangerRed();
            lh->TextAlign = ContentAlignment::MiddleCenter;
            lh->Location  = Point(100, 80);
            lh->Size      = System::Drawing::Size(280, 50);

            Label^ lsub = gcnew Label();
            lsub->Text      = L"Enter admin password:";
            lsub->ForeColor = Theme::SubText();
            lsub->Font      = Theme::BodyFont();
            lsub->Location  = Point(140, 150);
            lsub->AutoSize  = true;

            txtPass = MkPwdTxt(140, 176, 200);
            txtPass->KeyDown += gcnew KeyEventHandler(this, &AdminForm::OnPassKey);

            Button^ btnLogin = MakeBtn(L"Login", Theme::DangerRed(), Theme::LightText(), 140, 216, 200, 42);
            btnLogin->Click += gcnew EventHandler(this, &AdminForm::BtnLogin_Click);

            Label^ lhint = gcnew Label();
            lhint->Text      = L"Default: admin123";
            lhint->ForeColor = Theme::SubText();
            lhint->Font      = Theme::SmallFont();
            lhint->Location  = Point(180, 268);
            lhint->AutoSize  = true;

            Label^ lmsg2 = gcnew Label();
            lmsg2->Name      = L"lmsg2";
            lmsg2->Location  = Point(100, 300);
            lmsg2->Size      = System::Drawing::Size(280, 24);
            lmsg2->TextAlign = ContentAlignment::MiddleCenter;
            lmsg2->Font      = Theme::SmallFont();

            pnlLogin->Controls->Add(lh);
            pnlLogin->Controls->Add(lsub);
            pnlLogin->Controls->Add(txtPass);
            pnlLogin->Controls->Add(btnLogin);
            pnlLogin->Controls->Add(lhint);
            pnlLogin->Controls->Add(lmsg2);

            // Admin Panel
            pnlAdmin = gcnew Panel();
            pnlAdmin->Dock      = DockStyle::Fill;
            pnlAdmin->BackColor = Theme::NavyBg();
            pnlAdmin->Visible   = false;

            Label^ ah = gcnew Label();
            ah->Text      = L"Admin Panel";
            ah->Font      = Theme::HeadFont();
            ah->ForeColor = Theme::AccentGold();
            ah->Location  = Point(14, 14);
            ah->AutoSize  = true;
            pnlAdmin->Controls->Add(ah);

            pnlAdmin->Controls->Add(MkLbl(L"Select Account:", 14, 52, Theme::SubText()));

            cboAccount = gcnew ComboBox();
            cboAccount->Location       = Point(14, 70);
            cboAccount->Size           = System::Drawing::Size(450, 28);
            cboAccount->BackColor      = Theme::CardBg();
            cboAccount->ForeColor      = Theme::LightText();
            cboAccount->Font           = Theme::BodyFont();
            cboAccount->DropDownStyle  = ComboBoxStyle::DropDownList;
            for each (Account* a in g_bank->getAccounts())
                cboAccount->Items->Add(String::Format(L"{0} - {1} ({2})",
                    a->getId(),
                    gcnew String(a->getName().c_str()),
                    gcnew String(a->accountType().c_str())));
            if (cboAccount->Items->Count > 0) cboAccount->SelectedIndex = 0;
            cboAccount->SelectedIndexChanged +=
                gcnew EventHandler(this, &AdminForm::OnAccountSelected);
            pnlAdmin->Controls->Add(cboAccount);

            Label^ lp = gcnew Label();
            lp->Text      = L"PERMISSIONS";
            lp->Font      = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold);
            lp->ForeColor = Theme::AccentCyan();
            lp->Location  = Point(14, 108);
            lp->AutoSize  = true;
            pnlAdmin->Controls->Add(lp);

            chkW = MkChk(L"Withdraw (1)",  14,  130);
            chkD = MkChk(L"Deposit  (2)",  160, 130);
            chkT = MkChk(L"Transfer (4)",  14,  158);
            chkV = MkChk(L"VIP      (8)",  160, 158);
            chkA = MkChk(L"Admin   (16)",  14,  186);
            pnlAdmin->Controls->Add(chkW);
            pnlAdmin->Controls->Add(chkD);
            pnlAdmin->Controls->Add(chkT);
            pnlAdmin->Controls->Add(chkV);
            pnlAdmin->Controls->Add(chkA);

            Button^ btnApply = MakeBtn(L"Apply Permissions", Theme::SuccessGrn(), Color::Black, 14, 220, 200, 38);
            btnApply->Click += gcnew EventHandler(this, &AdminForm::BtnApplyPerm_Click);
            pnlAdmin->Controls->Add(btnApply);

            Label^ sep = gcnew Label();
            sep->Text      = L"FILE SETTINGS";
            sep->Font      = gcnew System::Drawing::Font("Segoe UI", 9, FontStyle::Bold);
            sep->ForeColor = Theme::AccentCyan();
            sep->Location  = Point(14, 278);
            sep->AutoSize  = true;
            pnlAdmin->Controls->Add(sep);

            chkEncrypt = gcnew CheckBox();
            chkEncrypt->Text      = L"Enable XOR File Encryption (key=0xA5)";
            chkEncrypt->Location  = Point(14, 300);
            chkEncrypt->ForeColor = Theme::AccentGold();
            chkEncrypt->Font      = Theme::BodyFont();
            chkEncrypt->Checked   = g_bank->getEncrypt();
            chkEncrypt->CheckedChanged +=
                gcnew EventHandler(this, &AdminForm::ChkEncrypt_Changed);
            pnlAdmin->Controls->Add(chkEncrypt);

            lblMsg = gcnew Label();
            lblMsg->Location  = Point(14, 350);
            lblMsg->Size      = System::Drawing::Size(450, 24);
            lblMsg->Font      = Theme::SmallFont();
            lblMsg->TextAlign = ContentAlignment::MiddleLeft;
            pnlAdmin->Controls->Add(lblMsg);

            Button^ btnClose = MakeBtn(L"Close", Theme::DangerRed(), Theme::LightText(), 14, 420, 455, 42);
            btnClose->Click += gcnew EventHandler(this, &AdminForm::BtnClose_Click);
            pnlAdmin->Controls->Add(btnClose);

            if (g_bank->accountCount() > 0) LoadPermState();

            Controls->Add(pnlLogin);
            Controls->Add(pnlAdmin);
        }

        int SelectedId() {
            if (cboAccount->SelectedIndex < 0) return -1;
            int i = 0;
            for each (Account* a in g_bank->getAccounts()) {
                if (i == cboAccount->SelectedIndex) return a->getId();
                ++i;
            }
            return -1;
        }

        void LoadPermState() {
            int id = SelectedId();
            if (id < 0) return;
            Account* a = g_bank->findById(id);
            if (!a) return;
            unsigned int p = a->getPermissions();
            chkW->Checked = (p & Perm::WITHDRAW) != 0;
            chkD->Checked = (p & Perm::DEPOSIT)  != 0;
            chkT->Checked = (p & Perm::TRANSFER) != 0;
            chkV->Checked = (p & Perm::VIP)      != 0;
            chkA->Checked = (p & Perm::ADMIN)    != 0;
        }

        void OnAccountSelected(Object^ /*s*/, EventArgs^ /*e*/) { LoadPermState(); }

        void OnPassKey(Object^ /*s*/, KeyEventArgs^ e) {
            if (e->KeyCode == Keys::Enter) BtnLogin_Click(nullptr, nullptr);
        }

        void BtnLogin_Click(Object^ /*s*/, EventArgs^ /*e*/) {
            if (txtPass->Text == ADMIN_PASS) {
                pnlLogin->Visible = false;
                pnlAdmin->Visible = true;
            } else {
                for each (Control^ c in pnlLogin->Controls) {
                    if (c->Name == L"lmsg2") {
                        c->ForeColor = Theme::DangerRed();
                        c->Text      = L"Incorrect password.";
                        break;
                    }
                }
            }
        }

        void BtnApplyPerm_Click(Object^ /*s*/, EventArgs^ /*e*/) {
            int id = SelectedId();
            if (id < 0) { ShowMsg(L"No account selected.", false); return; }

            unsigned int perm = 0;
            if (chkW->Checked) perm |= Perm::WITHDRAW;
            if (chkD->Checked) perm |= Perm::DEPOSIT;
            if (chkT->Checked) perm |= Perm::TRANSFER;
            if (chkV->Checked) perm |= Perm::VIP;
            if (chkA->Checked) perm |= Perm::ADMIN;

            g_bank->revokePermission(id, Perm::ALL);
            g_bank->grantPermission(id, perm);
            ShowMsg(String::Format(L"Permissions updated for account {0}.", id), true);
        }

        void ChkEncrypt_Changed(Object^ /*s*/, EventArgs^ /*e*/) {
            g_bank->setEncrypt(chkEncrypt->Checked);
            ShowMsg(String::Format(L"Encryption is now {0}.",
                chkEncrypt->Checked ? L"ON" : L"OFF"), true);
        }

        void BtnClose_Click(Object^ /*s*/, EventArgs^ /*e*/) { this->Close(); }

        void ShowMsg(String^ msg, bool ok) {
            lblMsg->ForeColor = ok ? Theme::SuccessGrn() : Theme::DangerRed();
            lblMsg->Text = msg;
        }
    };

} // namespace BankingApp
