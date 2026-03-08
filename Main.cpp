// ============================================================
//  Main.cpp  - Entry point + MainForm button implementations
// ============================================================
#pragma warning(disable: 4100)  // unreferenced formal parameter

#include "BankingSystem.h"
#include "MainForm.h"
#include "CreateAccountForm.h"
#include "TransactionForm.h"
#include "ViewAccountForm.h"
#include "AdminForm.h"

#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Windows::Forms;

namespace BankingApp {

    // Global bank engine (defined here, declared extern in MainForm.h)
    BankEngine* g_bank = nullptr;

    // MainForm button implementations
    void MainForm::BtnCreate_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        CreateAccountForm^ f = gcnew CreateAccountForm();
        if (f->ShowDialog(this) == System::Windows::Forms::DialogResult::OK)
            RefreshGrid();
    }
    void MainForm::BtnDeposit_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        TransactionForm^ f = gcnew TransactionForm(TxMode::Deposit);
        if (f->ShowDialog(this) == System::Windows::Forms::DialogResult::OK)
            RefreshGrid();
    }
    void MainForm::BtnWithdraw_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        TransactionForm^ f = gcnew TransactionForm(TxMode::Withdraw);
        if (f->ShowDialog(this) == System::Windows::Forms::DialogResult::OK)
            RefreshGrid();
    }
    void MainForm::BtnTransfer_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        TransactionForm^ f = gcnew TransactionForm(TxMode::Transfer);
        if (f->ShowDialog(this) == System::Windows::Forms::DialogResult::OK)
            RefreshGrid();
    }
    void MainForm::BtnView_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        if (dgvAccounts->SelectedRows->Count == 0) {
            SetStatus(L"Select an account row first.", false); return;
        }
        String^ idStr = dgvAccounts->SelectedRows[0]->Cells["colId"]->Value->ToString();
        int id;
        if (!Int32::TryParse(idStr, id)) { SetStatus(L"Invalid selection.", false); return; }
        ViewAccountForm^ f = gcnew ViewAccountForm(id);
        f->ShowDialog(this);
    }
    void MainForm::BtnInterest_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        std::string result = g_bank->applyInterest();
        MessageBox::Show(gcnew String(result.c_str()), L"Interest Applied",
            MessageBoxButtons::OK, MessageBoxIcon::Information);
        RefreshGrid();
    }
    void MainForm::BtnSave_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        std::string err = g_bank->saveAll();
        if (err.empty())
            SetStatus(String::Format(L"Saved {0} account(s) to {1}.",
                g_bank->accountCount(),
                gcnew String(g_bank->getDataFile().c_str())), true);
        else
            SetStatus(gcnew String(err.c_str()), false);
    }
    void MainForm::BtnLoad_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        std::string err = g_bank->loadAll();
        if (err.empty()) {
            RefreshGrid();
            SetStatus(String::Format(L"Loaded {0} account(s).", g_bank->accountCount()), true);
        } else {
            SetStatus(gcnew String(err.c_str()), false);
        }
    }
    void MainForm::BtnDelete_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        if (dgvAccounts->SelectedRows->Count == 0) {
            SetStatus(L"Select an account row first.", false); return;
        }
        String^ idStr = dgvAccounts->SelectedRows[0]->Cells["colId"]->Value->ToString();
        int id;
        if (!Int32::TryParse(idStr, id)) { SetStatus(L"Invalid selection.", false); return; }
        Account* a = g_bank->findById(id);
        if (!a) { SetStatus(L"Account not found.", false); return; }
        String^ msg = String::Format(L"Delete account #{0} ({1})?\nThis cannot be undone.",
            id, gcnew String(a->getName().c_str()));
        if (MessageBox::Show(msg, L"Confirm Delete",
                MessageBoxButtons::YesNo, MessageBoxIcon::Warning)
            != System::Windows::Forms::DialogResult::Yes) return;
        std::string err = g_bank->deleteAccount(id);
        if (err.empty()) {
            RefreshGrid();
            SetStatus(String::Format(L"Account #{0} deleted.", id), true);
        } else {
            SetStatus(gcnew String(err.c_str()), false);
        }
    }
    void MainForm::BtnAdmin_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        AdminForm^ f = gcnew AdminForm();
        f->ShowDialog(this);
        RefreshGrid();
    }
    void MainForm::BtnRefresh_Click(Object^ /*s*/, EventArgs^ /*e*/) {
        RefreshGrid();
    }

} // namespace BankingApp

// Win32 entry point
[System::STAThread]
int main(cli::array<String^>^ /*args*/) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    BankingApp::g_bank = new BankEngine();
    Application::Run(gcnew BankingApp::MainForm());

    delete BankingApp::g_bank;
    BankingApp::g_bank = nullptr;
    return 0;
}
