#pragma once
// ============================================================
//  MainForm.h  - Dashboard / main window
// ============================================================
#include "BankingSystem.h"

namespace BankingApp {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;

    // Forward declarations
    ref class CreateAccountForm;
    ref class TransactionForm;
    ref class ViewAccountForm;
    ref class AdminForm;

    extern BankEngine* g_bank;

    // -- colour palette -------------------------------------------
    namespace Theme {
        inline Color NavyBg()     { return Color::FromArgb(10,  25,  60);  }
        inline Color DarkBg()     { return Color::FromArgb(15,  35,  80);  }
        inline Color CardBg()     { return Color::FromArgb(20,  50,  110); }
        inline Color AccentGold() { return Color::FromArgb(255, 200, 50);  }
        inline Color AccentCyan() { return Color::FromArgb(0,   210, 220); }
        inline Color SuccessGrn() { return Color::FromArgb(50,  220, 120); }
        inline Color DangerRed()  { return Color::FromArgb(220, 60,  60);  }
        inline Color LightText()  { return Color::FromArgb(220, 235, 255); }
        inline Color SubText()    { return Color::FromArgb(140, 165, 210); }
        inline System::Drawing::Font^ TitleFont() { return gcnew System::Drawing::Font("Segoe UI", 22, FontStyle::Bold); }
        inline System::Drawing::Font^ HeadFont()  { return gcnew System::Drawing::Font("Segoe UI Semibold", 13, FontStyle::Bold); }
        inline System::Drawing::Font^ BodyFont()  { return gcnew System::Drawing::Font("Segoe UI", 10); }
        inline System::Drawing::Font^ SmallFont() { return gcnew System::Drawing::Font("Segoe UI", 9);  }
        inline System::Drawing::Font^ MonoFont()  { return gcnew System::Drawing::Font("Consolas", 9); }
    }

    // -- Global helpers (free functions outside managed class) ----
    inline Button^ MakeBtn(String^ text, Color bg, Color fg, int x, int y, int w, int h) {
        Button^ b    = gcnew Button();
        b->Text      = text;
        b->Location  = Point(x, y);
        b->Size      = System::Drawing::Size(w, h);
        b->BackColor = bg;
        b->ForeColor = fg;
        b->FlatStyle = FlatStyle::Flat;
        b->FlatAppearance->BorderSize = 0;
        b->Font      = gcnew System::Drawing::Font("Segoe UI Semibold", 10, FontStyle::Bold);
        b->Cursor    = Cursors::Hand;
        return b;
    }

    inline DataGridViewTextBoxColumn^ MakeCol(String^ header, String^ name, int w) {
        DataGridViewTextBoxColumn^ c = gcnew DataGridViewTextBoxColumn();
        c->HeaderText = header;
        c->Name       = name;
        c->Width      = w;
        return c;
    }

    public ref class MainForm : public Form {
    public:
        MainForm() { InitializeComponent(); RefreshGrid(); }

    private:
        DataGridView^ dgvAccounts;
        Label^        lblStatus;
        Panel^        pnlSidebar;
        Panel^        pnlMain;
        Panel^        pnlHeader;

        // Helper - no default params allowed in managed methods
        Button^ MakeSideBtn(String^ text, Color bg) {
            return MakeBtn(text, bg, Theme::LightText(), 10, 0, 188, 42);
        }

        void InitializeComponent() {
            this->Text          = L"Bank PR - Secure Banking System";
            this->Size          = System::Drawing::Size(1100, 700);
            this->MinimumSize   = System::Drawing::Size(900,  600);
            this->BackColor     = Theme::NavyBg();
            this->StartPosition = FormStartPosition::CenterScreen;
            this->Font          = Theme::BodyFont();

            // Header
            pnlHeader = gcnew Panel();
            pnlHeader->Dock      = DockStyle::Top;
            pnlHeader->Height    = 80;
            pnlHeader->BackColor = Theme::DarkBg();
            pnlHeader->Paint    += gcnew PaintEventHandler(this, &MainForm::OnHeaderPaint);

            Label^ lblTitle = gcnew Label();
            lblTitle->Text      = L"  BANK PR";
            lblTitle->Font      = Theme::TitleFont();
            lblTitle->ForeColor = Theme::AccentGold();
            lblTitle->AutoSize  = true;
            lblTitle->Location  = Point(12, 10);

            Label^ lblSub = gcnew Label();
            lblSub->Text      = L"  Secure File-Based Banking System";
            lblSub->Font      = gcnew System::Drawing::Font("Segoe UI", 10, FontStyle::Italic);
            lblSub->ForeColor = Theme::AccentCyan();
            lblSub->AutoSize  = true;
            lblSub->Location  = Point(14, 48);

            pnlHeader->Controls->Add(lblTitle);
            pnlHeader->Controls->Add(lblSub);

            // Sidebar
            pnlSidebar = gcnew Panel();
            pnlSidebar->Dock       = DockStyle::Left;
            pnlSidebar->Width      = 210;
            pnlSidebar->AutoScroll = true;
            pnlSidebar->BackColor = Theme::DarkBg();
            pnlSidebar->Paint    += gcnew PaintEventHandler(this, &MainForm::OnSidebarPaint);

            int sy = 16;
            Button^ b1 = MakeSideBtn(L"+ Create Account",  Theme::SuccessGrn());
            b1->Location = Point(10, sy); sy += 50;
            b1->Click   += gcnew EventHandler(this, &MainForm::BtnCreate_Click);

            Button^ b2 = MakeSideBtn(L"Deposit", Color::FromArgb(30,130,80));
            b2->Location = Point(10, sy); sy += 50;
            b2->Click   += gcnew EventHandler(this, &MainForm::BtnDeposit_Click);

            Button^ b3 = MakeSideBtn(L"Withdraw", Color::FromArgb(160,60,30));
            b3->Location = Point(10, sy); sy += 50;
            b3->Click   += gcnew EventHandler(this, &MainForm::BtnWithdraw_Click);

            Button^ b4 = MakeSideBtn(L"Transfer", Color::FromArgb(30,80,160));
            b4->Location = Point(10, sy); sy += 50;
            b4->Click   += gcnew EventHandler(this, &MainForm::BtnTransfer_Click);

            Button^ b5 = MakeSideBtn(L"View Account", Theme::CardBg());
            b5->Location = Point(10, sy); sy += 50;
            b5->Click   += gcnew EventHandler(this, &MainForm::BtnView_Click);

            Button^ b_del = MakeSideBtn(L"Delete Account", Color::FromArgb(140,30,30));
            b_del->Location = Point(10, sy); sy += 50;
            b_del->Click   += gcnew EventHandler(this, &MainForm::BtnDelete_Click);

            Button^ b6 = MakeSideBtn(L"Apply Interest", Color::FromArgb(80,60,10));
            b6->Location = Point(10, sy); sy += 50;
            b6->Click   += gcnew EventHandler(this, &MainForm::BtnInterest_Click);

            Button^ b7 = MakeSideBtn(L"Save to File", Color::FromArgb(60,40,100));
            b7->Location = Point(10, sy); sy += 50;
            b7->Click   += gcnew EventHandler(this, &MainForm::BtnSave_Click);

            Button^ b8 = MakeSideBtn(L"Load from File", Color::FromArgb(40,40,100));
            b8->Location = Point(10, sy); sy += 50;
            b8->Click   += gcnew EventHandler(this, &MainForm::BtnLoad_Click);

            Button^ b9 = MakeSideBtn(L"Admin Mode", Color::FromArgb(100,20,20));
            b9->Location = Point(10, sy); sy += 50;
            b9->Click   += gcnew EventHandler(this, &MainForm::BtnAdmin_Click);

            Button^ b10 = MakeSideBtn(L"Refresh", Color::FromArgb(20,80,80));
            b10->Location = Point(10, sy);
            b10->Click   += gcnew EventHandler(this, &MainForm::BtnRefresh_Click);

            pnlSidebar->Controls->Add(b1);  pnlSidebar->Controls->Add(b2);
            pnlSidebar->Controls->Add(b3);  pnlSidebar->Controls->Add(b4);
            pnlSidebar->Controls->Add(b5);  pnlSidebar->Controls->Add(b_del);
            pnlSidebar->Controls->Add(b6);  pnlSidebar->Controls->Add(b7);
            pnlSidebar->Controls->Add(b8);  pnlSidebar->Controls->Add(b9);
            pnlSidebar->Controls->Add(b10);

            // Main panel + grid
            pnlMain = gcnew Panel();
            pnlMain->Dock      = DockStyle::Fill;
            pnlMain->BackColor = Theme::NavyBg();
            pnlMain->Padding   = System::Windows::Forms::Padding(12);

            dgvAccounts = gcnew DataGridView();
            dgvAccounts->Dock                  = DockStyle::Fill;
            dgvAccounts->BackgroundColor       = Theme::NavyBg();
            dgvAccounts->GridColor             = Theme::CardBg();
            dgvAccounts->BorderStyle           = System::Windows::Forms::BorderStyle::None;
            dgvAccounts->ColumnHeadersDefaultCellStyle->BackColor  = Theme::CardBg();
            dgvAccounts->ColumnHeadersDefaultCellStyle->ForeColor  = Theme::AccentGold();
            dgvAccounts->ColumnHeadersDefaultCellStyle->Font       = Theme::HeadFont();
            dgvAccounts->DefaultCellStyle->BackColor               = Theme::DarkBg();
            dgvAccounts->DefaultCellStyle->ForeColor               = Theme::LightText();
            dgvAccounts->DefaultCellStyle->Font                    = Theme::MonoFont();
            dgvAccounts->DefaultCellStyle->SelectionBackColor      = Theme::CardBg();
            dgvAccounts->DefaultCellStyle->SelectionForeColor      = Theme::AccentGold();
            dgvAccounts->AlternatingRowsDefaultCellStyle->BackColor= Color::FromArgb(18,42,90);
            dgvAccounts->RowHeadersVisible     = false;
            dgvAccounts->ReadOnly              = true;
            dgvAccounts->AllowUserToAddRows    = false;
            dgvAccounts->SelectionMode         = DataGridViewSelectionMode::FullRowSelect;
            dgvAccounts->AutoSizeColumnsMode   = DataGridViewAutoSizeColumnsMode::Fill;
            dgvAccounts->EnableHeadersVisualStyles = false;
            dgvAccounts->CellBorderStyle       = DataGridViewCellBorderStyle::SingleHorizontal;
            dgvAccounts->ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle::Single;

            dgvAccounts->Columns->Add(MakeCol(L"ID",           "colId",   60));
            dgvAccounts->Columns->Add(MakeCol(L"Name",         "colName", 140));
            dgvAccounts->Columns->Add(MakeCol(L"Type",         "colType",  80));
            dgvAccounts->Columns->Add(MakeCol(L"Balance (PKR)","colBal",  120));
            dgvAccounts->Columns->Add(MakeCol(L"Permissions",  "colPerm", 160));
            dgvAccounts->Columns->Add(MakeCol(L"Overdraft",    "colOD",    80));
            dgvAccounts->Columns->Add(MakeCol(L"Transactions", "colTx",    80));

            pnlMain->Controls->Add(dgvAccounts);

            // Status bar
            lblStatus = gcnew Label();
            lblStatus->Dock      = DockStyle::Bottom;
            lblStatus->Height    = 28;
            lblStatus->BackColor = Theme::DarkBg();
            lblStatus->ForeColor = Theme::AccentCyan();
            lblStatus->Font      = Theme::SmallFont();
            lblStatus->TextAlign = ContentAlignment::MiddleLeft;
            lblStatus->Text      = L"  Ready.";
            lblStatus->Padding   = System::Windows::Forms::Padding(6, 0, 0, 0);

            this->Controls->Add(pnlMain);
            this->Controls->Add(pnlSidebar);
            this->Controls->Add(pnlHeader);
            this->Controls->Add(lblStatus);
        }

        void OnHeaderPaint(Object^ /*s*/, PaintEventArgs^ e) {
            Pen^ p = gcnew Pen(Theme::AccentGold(), 2);
            e->Graphics->DrawLine(p, 0, pnlHeader->Height-1, pnlHeader->Width, pnlHeader->Height-1);
        }
        void OnSidebarPaint(Object^ /*s*/, PaintEventArgs^ e) {
            Pen^ p = gcnew Pen(Theme::AccentCyan(), 1);
            e->Graphics->DrawLine(p, pnlSidebar->Width-1, 0, pnlSidebar->Width-1, pnlSidebar->Height);
        }

    public:
        void SetStatus(String^ msg, bool ok) {
            lblStatus->ForeColor = ok ? Theme::SuccessGrn() : Theme::DangerRed();
            lblStatus->Text      = L"  " + msg;
        }

        void RefreshGrid() {
            dgvAccounts->Rows->Clear();
            for each (Account* a in g_bank->getAccounts()) {
                String^ id    = a->getId().ToString();
                String^ nm    = gcnew String(a->getName().c_str());
                String^ type  = gcnew String(a->accountType().c_str());
                String^ bal   = String::Format("{0:N2}", a->getBalance());
                String^ perm  = gcnew String(Perm::describe(a->getPermissions()).c_str());
                String^ od    = String::Format("{0:N2}", a->getOverdraft());
                String^ txCnt = ((int)a->getTransactions().size()).ToString();
                int r = dgvAccounts->Rows->Add(id, nm, type, bal, perm, od, txCnt);
                if (a->hasPermission(Perm::VIP))
                    dgvAccounts->Rows[r]->DefaultCellStyle->ForeColor = Theme::AccentGold();
            }
            SetStatus(String::Format("{0} account(s) loaded.", g_bank->accountCount()), true);
        }

    private:
        void BtnCreate_Click(Object^ sender, EventArgs^ e);
        void BtnDeposit_Click(Object^ sender, EventArgs^ e);
        void BtnWithdraw_Click(Object^ sender, EventArgs^ e);
        void BtnTransfer_Click(Object^ sender, EventArgs^ e);
        void BtnView_Click(Object^ sender, EventArgs^ e);
        void BtnInterest_Click(Object^ sender, EventArgs^ e);
        void BtnSave_Click(Object^ sender, EventArgs^ e);
        void BtnLoad_Click(Object^ sender, EventArgs^ e);
        void BtnDelete_Click(Object^ sender, EventArgs^ e);
        void BtnAdmin_Click(Object^ sender, EventArgs^ e);
        void BtnRefresh_Click(Object^ sender, EventArgs^ e);
    };

} // namespace BankingApp
