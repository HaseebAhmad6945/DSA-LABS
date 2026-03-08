#pragma once
// ============================================================
//  BankingSystem.h  –  Pure C++ business logic (no CLR)
//  All OOP, bitwise, vectors, file I/O, pointers live here.
// ============================================================
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <stdexcept>

using namespace std;

// ──────────────────────────────────────────────
//  PERMISSION BIT FLAGS
// ──────────────────────────────────────────────
namespace Perm {
    constexpr unsigned int WITHDRAW = 1u;
    constexpr unsigned int DEPOSIT  = 2u;
    constexpr unsigned int TRANSFER = 4u;
    constexpr unsigned int VIP      = 8u;
    constexpr unsigned int ADMIN    = 16u;
    constexpr unsigned int STANDARD = WITHDRAW | DEPOSIT | TRANSFER;
    constexpr unsigned int ALL      = WITHDRAW | DEPOSIT | TRANSFER | VIP | ADMIN;

    inline string describe(unsigned int p) {
        string s;
        if (p & WITHDRAW) s += "Withdraw ";
        if (p & DEPOSIT)  s += "Deposit ";
        if (p & TRANSFER) s += "Transfer ";
        if (p & VIP)      s += "VIP ";
        if (p & ADMIN)    s += "Admin ";
        if (s.empty()) s = "None";
        return s;
    }
}

// ──────────────────────────────────────────────
//  TRANSACTION COMPRESSION  [4-bit type|28-bit cents]
// ──────────────────────────────────────────────
namespace TxCode {
    constexpr unsigned int DEPOSIT  = 1u;
    constexpr unsigned int WITHDRAW = 2u;
    constexpr unsigned int TRANSFER = 3u;

    inline unsigned int encode(unsigned int type, double amount) {
        unsigned int cents = static_cast<unsigned int>(fabs(amount) * 100.0) & 0x0FFFFFFF;
        return (type << 28) | cents;
    }
    inline void decode(unsigned int code, unsigned int& type, double& amount) {
        type   = (code >> 28) & 0xF;
        amount = (code & 0x0FFFFFFF) / 100.0;
    }
}

// ──────────────────────────────────────────────
//  XOR ENCRYPTION
// ──────────────────────────────────────────────
namespace Crypto {
    constexpr unsigned char KEY = 0xA5;
    inline string xorCrypt(const string& data) {
        string out = data;
        for (char& c : out) c ^= static_cast<char>(KEY);
        return out;
    }
}

// ──────────────────────────────────────────────
//  ABSTRACT BASE CLASS
// ──────────────────────────────────────────────
class Account {
protected:
    int            accountId;
    string         name;
    double         balance;
    unsigned int   permissions;
    double         overdraftLimit;
    vector<double> transactions;
    double         monthlyDeposits[12];
    double         monthlyWithdrawals[12];

    int currentMonth() const {
        time_t t = time(nullptr);
        tm lt{}; localtime_s(&lt, &t);
        return lt.tm_mon;
    }
    void recordMonthly(double amount) {
        int m = currentMonth();
        if (amount > 0) monthlyDeposits[m]    += amount;
        else            monthlyWithdrawals[m] += fabs(amount);
    }

public:
    Account(int id, const string& n, double bal, unsigned int perm, double overdraft = 0.0)
        : accountId(id), name(n), balance(bal), permissions(perm), overdraftLimit(overdraft)
    {
        for (int i = 0; i < 12; ++i) { monthlyDeposits[i] = 0; monthlyWithdrawals[i] = 0; }
    }
    virtual ~Account() {}

    virtual void   deposit(double amount)  = 0;
    virtual void   withdraw(double amount) = 0;
    virtual string accountType()     const = 0;
    virtual void   saveToStream(ostringstream& oss) const = 0;

    int          getId()          const { return accountId; }
    string       getName()        const { return name; }
    double       getBalance()     const { return balance; }
    unsigned int getPermissions() const { return permissions; }
    double       getOverdraft()   const { return overdraftLimit; }

    const vector<double>& getTransactions() const { return transactions; }

    bool hasPermission(unsigned int flag) const { return (permissions & flag) != 0u; }
    void grantPermission(unsigned int flag)     { permissions |=  flag; }
    void revokePermission(unsigned int flag)    { permissions &= ~flag; }

    void pushTransaction(double tx) {
        transactions.push_back(tx);
        recordMonthly(tx);
    }

    // Transfer (bonus)
    string transferTo(Account* target, double amount) {
        if (!hasPermission(Perm::TRANSFER))
            return "TRANSFER permission denied on source account.";
        if (!target->hasPermission(Perm::DEPOSIT))
            return "Target account does not have DEPOSIT permission.";
        if (amount <= 0)
            return "Amount must be positive.";
        if (balance - amount < -overdraftLimit)
            return "Insufficient funds (including overdraft limit).";
        balance -= amount;
        pushTransaction(-amount);
        target->balance += amount;
        target->pushTransaction(amount);
        return "OK";
    }

    // Monthly summary data
    double getMonthDeposit(int m)    const { return monthlyDeposits[m];    }
    double getMonthWithdraw(int m)   const { return monthlyWithdrawals[m]; }

    // (transactions written by BankEngine::saveAll as CSV TX rows)
};

// ──────────────────────────────────────────────
//  SAVINGS ACCOUNT
// ──────────────────────────────────────────────
class SavingsAccount : public Account {
    double interestRate;
public:
    SavingsAccount(int id, const string& n, double bal,
                   unsigned int perm = Perm::STANDARD,
                   double rate       = 7.5,
                   double overdraft  = 0.0)
        : Account(id, n, bal, perm, overdraft), interestRate(rate) {}

    string accountType() const override { return "Savings"; }
    double getInterestRate()        const { return interestRate; }

    void deposit(double amount) override {
        if (!hasPermission(Perm::DEPOSIT))   throw runtime_error("DEPOSIT permission denied.");
        if (amount <= 0)                      throw runtime_error("Amount must be positive.");
        balance += amount;
        pushTransaction(amount);
    }
    void withdraw(double amount) override {
        if (!hasPermission(Perm::WITHDRAW))  throw runtime_error("WITHDRAW permission denied.");
        if (amount <= 0)                      throw runtime_error("Amount must be positive.");
        if (balance - amount < -overdraftLimit)
            throw runtime_error("Insufficient funds.");
        balance -= amount;
        pushTransaction(-amount);
    }
    string applyInterest() {
        double interest = balance * (interestRate / 100.0 / 12.0);
        balance += interest;
        pushTransaction(interest);
        ostringstream oss;
        oss << fixed << setprecision(2) << interest;
        return oss.str();
    }
    void saveToStream(ostringstream& oss) const override {
        // CSV: ACCOUNT,Savings,ID,Name,Balance,Permissions,InterestRate,Overdraft
        oss << "ACCOUNT,Savings,"
            << accountId << "," << name << ","
            << fixed << setprecision(2) << balance << ","
            << permissions << ","
            << fixed << setprecision(4) << interestRate << ","
            << fixed << setprecision(2) << overdraftLimit << "\n";
    }
};

// ──────────────────────────────────────────────
//  CURRENT ACCOUNT
// ──────────────────────────────────────────────
class CurrentAccount : public Account {
    double transactionFee;
public:
    CurrentAccount(int id, const string& n, double bal,
                   unsigned int perm  = Perm::STANDARD,
                   double fee         = 25.0,
                   double overdraft   = 5000.0)
        : Account(id, n, bal, perm, overdraft), transactionFee(fee) {}

    string accountType()   const override { return "Current"; }
    double getTransactionFee()    const { return transactionFee; }

    void deposit(double amount) override {
        if (!hasPermission(Perm::DEPOSIT))   throw runtime_error("DEPOSIT permission denied.");
        if (amount <= 0)                      throw runtime_error("Amount must be positive.");
        balance += amount;
        pushTransaction(amount);
    }
    void withdraw(double amount) override {
        if (!hasPermission(Perm::WITHDRAW))  throw runtime_error("WITHDRAW permission denied.");
        if (amount <= 0)                      throw runtime_error("Amount must be positive.");
        double total = amount + transactionFee;
        if (balance - total < -overdraftLimit)
            throw runtime_error("Insufficient funds (inc. fee of " +
                                to_string((int)transactionFee) + " PKR).");
        balance -= total;
        pushTransaction(-amount);
        pushTransaction(-transactionFee);
    }
    void saveToStream(ostringstream& oss) const override {
        // CSV: ACCOUNT,Current,ID,Name,Balance,Permissions,TxFee,Overdraft
        oss << "ACCOUNT,Current,"
            << accountId << "," << name << ","
            << fixed << setprecision(2) << balance << ","
            << permissions << ","
            << fixed << setprecision(2) << transactionFee << ","
            << fixed << setprecision(2) << overdraftLimit << "\n";
    }
};

// ──────────────────────────────────────────────
//  BANK ENGINE
// ──────────────────────────────────────────────
class BankEngine {
    vector<Account*> accounts;
    int  nextId;
    bool encryptFiles;
    const string DATA_FILE = "bank_accounts.csv";

public:
    BankEngine() : nextId(1001), encryptFiles(false) {}
    ~BankEngine() { for (Account* a : accounts) delete a; }

    BankEngine(const BankEngine&)            = delete;
    BankEngine& operator=(const BankEngine&) = delete;

    bool getEncrypt() const { return encryptFiles; }
    void setEncrypt(bool v) { encryptFiles = v; }

    const vector<Account*>& getAccounts() const { return accounts; }

    Account* findById(int id) const {
        for (Account* a : accounts)
            if (a->getId() == id) return a;
        return nullptr;
    }

    // Create and return new account id
    int createSavings(const string& name, double balance, unsigned int perm,
                      double rate, double overdraft) {
        Account* a = new SavingsAccount(nextId, name, balance, perm, rate, overdraft);
        accounts.push_back(a);
        return nextId++;
    }
    int createCurrent(const string& name, double balance, unsigned int perm,
                      double fee, double overdraft) {
        Account* a = new CurrentAccount(nextId, name, balance, perm, fee, overdraft);
        accounts.push_back(a);
        return nextId++;
    }

    // Returns "" on success, error message on failure
    string deposit(int id, double amount) {
        Account* a = findById(id);
        if (!a) return "Account not found.";
        try { a->deposit(amount); return ""; }
        catch (const exception& e) { return e.what(); }
    }
    string withdraw(int id, double amount) {
        Account* a = findById(id);
        if (!a) return "Account not found.";
        try { a->withdraw(amount); return ""; }
        catch (const exception& e) { return e.what(); }
    }
    string transfer(int fromId, int toId, double amount) {
        Account* from = findById(fromId);
        Account* to   = findById(toId);
        if (!from) return "Source account not found.";
        if (!to)   return "Destination account not found.";
        if (from == to) return "Source and destination cannot be the same.";
        return from->transferTo(to, amount);
    }
    string applyInterest() {
        string result;
        for (Account* a : accounts) {
            SavingsAccount* sa = dynamic_cast<SavingsAccount*>(a);
            if (sa) {
                string amt = sa->applyInterest();
                result += "Account " + to_string(sa->getId()) +
                          " (" + sa->getName() + "): +" + amt + " PKR\n";
            }
        }
        if (result.empty()) result = "No Savings accounts found.";
        return result;
    }

    string deleteAccount(int id) {
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                accounts.erase(it);
                return "";
            }
        }
        return "Account not found.";
    }

    void grantPermission(int id, unsigned int flag) {
        Account* a = findById(id);
        if (a) a->grantPermission(flag);
    }
    void revokePermission(int id, unsigned int flag) {
        Account* a = findById(id);
        if (a) a->revokePermission(flag);
    }

    // ── CSV File I/O ──────────────────────────────────────────────────────
    // Format:
    //   BANK_DATA_V1,<nextId>
    //   ACCOUNT,<Type>,<ID>,<Name>,<Balance>,<Permissions>,<Extra1>,<Extra2>
    //   TX,<AccountID>,<Amount>
    // Optionally XOR-encrypted as a whole before writing.
    // ----------------------------------------------------------------------
    string saveAll() {
        ostringstream oss;
        // Row 1: metadata
        oss << "BANK_DATA_V1," << nextId << "\n";
        // Account section header
        oss << "# --- ACCOUNTS ---\n";
        oss << "# RowType,AccountType,AccountNo,AccountName,Balance,Permissions,InterestRate_or_TxFee,OverdraftLimit\n";
        for (const Account* a : accounts)
            a->saveToStream(oss);
        // Transaction section header
        oss << "# --- TRANSACTIONS ---\n";
        oss << "# RowType,AccountNo,Amount\n";
        for (const Account* a : accounts)
            for (double tx : a->getTransactions())
                oss << "TX," << a->getId() << ","
                    << fixed << setprecision(2) << tx << "\n";

        string content = oss.str();
        if (encryptFiles) content = Crypto::xorCrypt(content);

        ofstream ofs(DATA_FILE, ios::binary);
        if (!ofs) return "Cannot open file for writing.";
        ofs << content;
        return "";
    }

    string loadAll() {
        ifstream ifs(DATA_FILE, ios::binary);
        if (!ifs) return "File not found: " + DATA_FILE;

        string content((istreambuf_iterator<char>(ifs)),
                        istreambuf_iterator<char>());
        ifs.close();

        // Auto-detect XOR encryption (non-ASCII bytes in first 16 chars)
        size_t checkLen = min(content.size(), size_t(16));
        for (size_t i = 0; i < checkLen; ++i)
            if ((unsigned char)content[i] > 127) {
                content = Crypto::xorCrypt(content); break;
            }

        for (Account* a : accounts) delete a;
        accounts.clear();

        istringstream ss(content);
        string line;
        while (getline(ss, line)) {
            // Strip trailing CR (Windows line endings)
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty() || line[0] == '#') continue; // skip blanks and comment/header lines

            // Split CSV row into fields
            vector<string> f;
            istringstream ls(line);
            string field;
            while (getline(ls, field, ',')) f.push_back(field);
            if (f.empty()) continue;

            if (f[0] == "BANK_DATA_V1" && f.size() >= 2) {
                try { nextId = stoi(f[1]); } catch (...) {}
            }
            else if (f[0] == "ACCOUNT" && f.size() >= 8) {
                // ACCOUNT,TYPE,ID,Name,Balance,Permissions,Extra1,Extra2
                try {
                    string type     = f[1];
                    int    id       = stoi(f[2]);
                    string nm       = f[3];
                    double bal      = stod(f[4]);
                    unsigned int pm = (unsigned int)stoul(f[5]);
                    double ex1      = stod(f[6]);
                    double ex2      = stod(f[7]);
                    Account* acc    = (type == "Savings")
                        ? (Account*)new SavingsAccount(id, nm, bal, pm, ex1, ex2)
                        : (Account*)new CurrentAccount(id, nm, bal, pm, ex1, ex2);
                    accounts.push_back(acc);
                } catch (...) {}
            }
            else if (f[0] == "TX" && f.size() >= 3) {
                // TX,AccountID,Amount
                try {
                    int    id = stoi(f[1]);
                    double tx = stod(f[2]);
                    Account* a = findById(id);
                    if (a) a->pushTransaction(tx);
                } catch (...) {}
            }
        }
        return "";
    }

    int accountCount() const { return (int)accounts.size(); }
    string getDataFile() const { return DATA_FILE; }
};
