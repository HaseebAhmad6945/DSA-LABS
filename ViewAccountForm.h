#pragma once
// ============================================================
//  ViewAccountForm.h  - Account detail, TX log, monthly summary
// ============================================================
#include "MainForm.h"

namespace BankingApp {

    public ref class ViewAccountForm : public Form {
        int targetId;

    public:
        ViewAccountForm(int id) : targetId(id) { InitializeComponent(); }

    private:
        // Private helper - builds a styled DataGridView (no lambdas in managed)
        DataGridView^ MakeGrid() {
            DataGridView^ g = gcnew DataGridView();
            g->BackgroundColor  = Theme::DarkBg();
            g->GridColor        = Theme::CardBg();
            g->BorderStyle      = System::Windows::Forms::BorderStyle::None;
            g->ColumnHeadersDefaultCellStyle->BackColor  = Theme::CardBg();
            g->ColumnHeadersDefaultCellStyle->ForeColor  = Theme::AccentGold();
            g->ColumnHeadersDefaultCellStyle->Font       = Theme::HeadFont();
            g->DefaultCellStyle->BackColor  = Theme::DarkBg();
            g->DefaultCellStyle->ForeColor  = Theme::LightText();
            g->DefaultCellStyle->Font       = Theme::MonoFont();
            g->AlternatingRowsDefaultCellStyle->BackColor = Color::FromArgb(18,42,90);
            g->RowHeadersVisible    = false;
            g->ReadOnly             = true;
            g->AllowUserToAddRows   = false;
            g->SelectionMode        = DataGridViewSelectionMode::FullRowSelect;
            g->AutoSizeColumnsMode  = DataGridViewAutoSizeColumnsMode::Fill;
            g->EnableHeadersVisualStyles = false;
            g->Dock                 = DockStyle::Fill;
            return g;
        }

        // Add a label + value pair to a panel (replaces lambda to avoid C3498)
        void AddInfo(Panel^ parent, String^ lbl, String^ val, Color vc, int x, int y) {
            Label^ l = gcnew Label();
            l->Text      = lbl;
            l->Font      = Theme::SmallFont();
            l->ForeColor = Theme::SubText();
            l->Location  = Point(x, y);
            l->AutoSize  = true;
            parent->Controls->Add(l);

            Label^ v2 = gcnew Label();
            v2->Text      = val;
            v2->Font      = gcnew System::Drawing::Font("Segoe UI Semibold", 11, FontStyle::Bold);
            v2->ForeColor = vc;
            v2->Location  = Point(x, y + 18);
            v2->AutoSize  = true;
            parent->Controls->Add(v2);
        }

        void InitializeComponent() {
            Account* a = g_bank->findById(targetId);

            this->Text          = String::Format(L"Account Details - {0}", targetId);
            this->Size          = System::Drawing::Size(820, 560);
            this->BackColor     = Theme::NavyBg();
            this->StartPosition = FormStartPosition::CenterParent;
            this->Font          = Theme::BodyFont();

            // Header card
            Panel^ header = gcnew Panel();
            header->Dock      = DockStyle::Top;
            header->Height    = 110;
            header->BackColor = Theme::DarkBg();
            header->Padding   = System::Windows::Forms::Padding(14, 10, 14, 10);

            if (a) {
                AddInfo(header, L"ID",         a->getId().ToString(),                          Theme::AccentGold(), 10,  8);
                AddInfo(header, L"Name",       gcnew String(a->getName().c_str()),              Theme::LightText(), 80,  8);
                AddInfo(header, L"Type",       gcnew String(a->accountType().c_str()),          Theme::AccentCyan(),280, 8);
                AddInfo(header, L"Balance",    String::Format(L"PKR {0:N2}", a->getBalance()),  Theme::SuccessGrn(),400, 8);
                AddInfo(header, L"Overdraft",  String::Format(L"PKR {0:N2}", a->getOverdraft()),Theme::SubText(),  580, 8);
                AddInfo(header, L"Permissions",gcnew String(Perm::describe(a->getPermissions()).c_str()),
                                                                                                Theme::AccentGold(), 10, 62);
            } else {
                Label^ err = gcnew Label();
                err->Text      = L"Account not found.";
                err->ForeColor = Theme::DangerRed();
                err->Font      = Theme::HeadFont();
                err->Location  = Point(20, 20);
                err->AutoSize  = true;
                header->Controls->Add(err);
            }

            // Tabs
            TabControl^ tabs = gcnew TabControl();
            tabs->Dock      = DockStyle::Fill;
            tabs->BackColor = Theme::NavyBg();
            tabs->Font      = Theme::BodyFont();

            // Tab 1: Transaction History
            TabPage^ pgTx = gcnew TabPage(L"Transaction History");
            pgTx->BackColor = Theme::NavyBg();

            DataGridView^ dgTx = MakeGrid();
            dgTx->Columns->Add(MakeCol(L"#",            "cN",   40));
            dgTx->Columns->Add(MakeCol(L"Type",         "cTy",  90));
            dgTx->Columns->Add(MakeCol(L"Amount (PKR)", "cAmt", 130));
            dgTx->Columns->Add(MakeCol(L"Encoded (hex)","cHex", 120));
            dgTx->Columns->Add(MakeCol(L"Dec Type",     "cDT",  90));
            dgTx->Columns->Add(MakeCol(L"Dec Amount",   "cDA",  110));

            if (a) {
                int idx = 1;
                for (double tx : a->getTransactions()) {
                    unsigned int type = (tx >= 0) ? TxCode::DEPOSIT : TxCode::WITHDRAW;
                    unsigned int code = TxCode::encode(type, tx);
                    unsigned int dT;  double dA;
                    TxCode::decode(code, dT, dA);

                    String^ typeStr = (tx >= 0) ? L"DEPOSIT" : L"WITHDRAW";
                    String^ hexStr  = String::Format(L"0x{0:X8}", code);
                    String^ dTStr   = (dT == 1) ? L"DEPOSIT" : (dT == 2 ? L"WITHDRAW" : L"TRANSFER");

                    cli::array<Object^>^ row = gcnew cli::array<Object^>(6);
                    row[0] = idx++;
                    row[1] = typeStr;
                    row[2] = String::Format(L"{0:N2}", Math::Abs(tx));
                    row[3] = hexStr;
                    row[4] = dTStr;
                    row[5] = String::Format(L"{0:N2}", dA);
                    int r = dgTx->Rows->Add(row);
                    dgTx->Rows[r]->DefaultCellStyle->ForeColor =
                        (tx >= 0) ? Theme::SuccessGrn() : Theme::DangerRed();
                }
            }
            pgTx->Controls->Add(dgTx);
            tabs->TabPages->Add(pgTx);

            // Tab 2: Monthly Summary
            TabPage^ pgMo = gcnew TabPage(L"Monthly Summary");
            pgMo->BackColor = Theme::NavyBg();

            DataGridView^ dgMo = MakeGrid();
            dgMo->Columns->Add(MakeCol(L"Month",          "cM",  80));
            dgMo->Columns->Add(MakeCol(L"Deposits (PKR)", "cD",  130));
            dgMo->Columns->Add(MakeCol(L"Withdrawals",    "cW",  130));
            dgMo->Columns->Add(MakeCol(L"Net Change",     "cNet",130));

            // Plain C-style array avoids std::array / cli::array issues
            const wchar_t* months[12] = {
                L"January",L"February",L"March",L"April",L"May",L"June",
                L"July",L"August",L"September",L"October",L"November",L"December"
            };
            if (a) {
                for (int i = 0; i < 12; ++i) {
                    double dep = a->getMonthDeposit(i);
                    double wd  = a->getMonthWithdraw(i);
                    double net = dep - wd;

                    cli::array<Object^>^ row = gcnew cli::array<Object^>(4);
                    row[0] = gcnew String(months[i]);
                    row[1] = String::Format(L"{0:N2}", dep);
                    row[2] = String::Format(L"{0:N2}", wd);
                    row[3] = String::Format(L"{0:N2}", net);
                    int r = dgMo->Rows->Add(row);
                    dgMo->Rows[r]->DefaultCellStyle->ForeColor =
                        (net >= 0) ? Theme::SuccessGrn() : Theme::DangerRed();
                }
            }
            pgMo->Controls->Add(dgMo);
            tabs->TabPages->Add(pgMo);

            // Tab 3: TX Compression Demo
            TabPage^ pgComp = gcnew TabPage(L"TX Compression Demo");
            pgComp->BackColor = Theme::NavyBg();

            DataGridView^ dgC = MakeGrid();
            dgC->Columns->Add(MakeCol(L"Type",         "ct",  90));
            dgC->Columns->Add(MakeCol(L"Amount",       "ca", 110));
            dgC->Columns->Add(MakeCol(L"Encoded (hex)","ch", 120));
            dgC->Columns->Add(MakeCol(L"Bit Layout",   "cb", 200));
            dgC->Columns->Add(MakeCol(L"Dec Type",     "cdt", 90));
            dgC->Columns->Add(MakeCol(L"Dec Amount",   "cda",110));

            // Plain C arrays - no local struct, no std::array
            unsigned int demoTypes[3]   = { TxCode::DEPOSIT, TxCode::WITHDRAW, TxCode::TRANSFER };
            double       demoAmounts[3] = { 15000.50, 2500.75, 8999.99 };
            const char*  tnames[4]      = { "", "DEPOSIT", "WITHDRAW", "TRANSFER" };

            for (int i = 0; i < 3; ++i) {
                unsigned int code = TxCode::encode(demoTypes[i], demoAmounts[i]);
                unsigned int dT;  double dA;
                TxCode::decode(code, dT, dA);

                String^ bits = String::Format(L"[{0} | {1}]",
                    (code >> 28) & 0xF, code & 0x0FFFFFFF);

                cli::array<Object^>^ row = gcnew cli::array<Object^>(6);
                row[0] = gcnew String(tnames[demoTypes[i]]);
                row[1] = String::Format(L"{0:N2}", demoAmounts[i]);
                row[2] = String::Format(L"0x{0:X8}", code);
                row[3] = bits;
                row[4] = gcnew String(tnames[dT]);
                row[5] = String::Format(L"{0:N2}", dA);
                dgC->Rows->Add(row);
            }
            pgComp->Controls->Add(dgC);
            tabs->TabPages->Add(pgComp);

            // Close button
            Button^ btnClose = MakeBtn(L"Close", Theme::DangerRed(), Theme::LightText(), 700, 6, 100, 30);
            btnClose->Anchor = AnchorStyles::Top | AnchorStyles::Right;
            btnClose->Click += gcnew EventHandler(this, &ViewAccountForm::BtnClose_Click);

            Controls->Add(tabs);
            Controls->Add(header);
            Controls->Add(btnClose);
        }

        void BtnClose_Click(Object^ /*s*/, EventArgs^ /*e*/) { this->Close(); }
    };

} // namespace BankingApp
