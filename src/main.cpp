#define NOMINMAX
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <limits>
#include <stdexcept>
#include <ctime>

#include "HangHoa.h"
#include "DanhSachHangHoa.h"
#include "HangHoaConsole.h"
#include "HangHoaRepository.h"
#include "MaGiamGia.h"
#include "DanhSachMaGiamGia.h"
#include "OrderManager.h"

using namespace std;

// ==================== DINH NGHIA HANG SO HE THONG ====================
const int COLOR_DEFAULT = 7;
const int COLOR_GREEN = 10;
const int COLOR_CYAN = 11;
const int COLOR_RED = 12;
const int COLOR_MAGENTA = 13;
const int COLOR_YELLOW = 14;

const int KEY_BACKSPACE = 8;
const int KEY_ENTER = 13;
const int KEY_ESC = 27;
const int KEY_SPACE = 32;
const int KEY_TILDE = 126;
const int KEY_ARROW_PREFIX = 224;

const int MAX_LOGIN_ATTEMPTS = 3;
const int ROLE_EXIT = 0;
const int ROLE_CUSTOMER = 1;
const int ROLE_ADMIN = 2;

const char* const FILE_ADMIN = "data/Admin.txt";
const char* const FILE_ACCOUNT = "data/Account.txt";

typedef struct _TAIKHOAN
{
    string User;
    string Password;
} TAIKHOAN, *PTAIKHOAN;

/******************************************************************************
 * @Description Lay chuoi thoi gian thuc te cua he thong (dd/MM/yyyy HH:mm:ss)
 *****************************************************************************/
string layThoiGianThucTe()
{
    time_t now = time(nullptr);
    tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &localTime);
    return string(buffer);
}

/******************************************************************************
 * @Description Doi mau chu tren man hinh console Windows
 *****************************************************************************/
void setColor(const int iColor)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, iColor);
}

/******************************************************************************
 * @Description Lay toa do con tro console hien tai
 *****************************************************************************/
COORD layToaDoConTro()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition;
}

/******************************************************************************
 * @Description Di chuyen con tro console den toa do chi dinh
 *****************************************************************************/
void diChuyenConTro(COORD coord)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/******************************************************************************
 * @Description Doc lua chon so nguyen tu ban phim dong thoi cap nhat
 *              dong ho thoi gian thuc moi giay ma khong nhap nhay man hinh
 *****************************************************************************/
bool nhapLuaChonVoiDongHo(int& iChon, COORD posDongHo)
{
    string strNhap = "";
    string strThoiGianCu = "";
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD wThuocTinhMacDinh = csbi.wAttributes;

    while (true)
    {
        // 1. Cap nhat dong ho neu da sang giay moi
        string strThoiGianMoi = layThoiGianThucTe();
        if (strThoiGianMoi != strThoiGianCu)
        {
            strThoiGianCu = strThoiGianMoi;
            COORD posNhapHienTai = layToaDoConTro();

            diChuyenConTro(posDongHo);
            SetConsoleTextAttribute(hConsole, COLOR_CYAN);
            cout << strThoiGianMoi;
            SetConsoleTextAttribute(hConsole, wThuocTinhMacDinh);

            diChuyenConTro(posNhapHienTai);
            cout.flush();
        }

        // 2. Kiem tra nguoi dung co dang go phim hay khong
        if (_kbhit())
        {
            int iChar = _getch();
            if (iChar == KEY_ENTER)
            {
                cout << '\n';
                if (strNhap.empty())
                {
                    return false;
                }
                try
                {
                    size_t idx = 0;
                    int iKetQua = stoi(strNhap, &idx);
                    if (idx != strNhap.length())
                    {
                        return false;
                    }
                    iChon = iKetQua;
                    return true;
                }
                catch (...)
                {
                    return false;
                }
            }
            else if (iChar == KEY_BACKSPACE)
            {
                if (!strNhap.empty())
                {
                    strNhap.pop_back();
                    cout << "\b \b";
                    cout.flush();
                }
            }
            else if (iChar == 0 || iChar == KEY_ARROW_PREFIX)
            {
                _getch(); // Bo qua phim mui ten / phim chuc nang
            }
            else if (iChar >= KEY_SPACE && iChar <= KEY_TILDE)
            {
                strNhap.push_back(static_cast<char>(iChar));
                cout << static_cast<char>(iChar);
                cout.flush();
            }
        }

        // Giam tai CPU nhung van phan hoi phim bam tuc thi
        Sleep(50);
    }
}

/******************************************************************************
 * @Description Xoa bo dem ban phim khi nhap sai kieu du lieu
 *****************************************************************************/
void xoaBoDem()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

/******************************************************************************
 * @Description Nhap mat khau tu ban phim va ma hoa hien thi thanh dau '*'
 *****************************************************************************/
bool nhapMatKhau(string& strMatKhau)
{
    strMatKhau = "";
    int iChar = 0;

    while (true)
    {
        iChar = _getch();
        if (iChar == KEY_ESC)
        {
            return false;
        }
        else if (iChar == KEY_ENTER)
        {
            cout << endl;
            return true;
        }
        else if (iChar == KEY_BACKSPACE)
        {
            if (!strMatKhau.empty())
            {
                strMatKhau.pop_back();
                cout << "\b \b";
            }
        }
        else if (iChar == 0 || iChar == KEY_ARROW_PREFIX)
        {
            _getch();
        }
        else if (iChar >= KEY_SPACE && iChar <= KEY_TILDE)
        {
            strMatKhau.push_back(static_cast<char>(iChar));
            cout << '*';
        }
    }
}

bool nhapTenTaiKhoanDangNhap(string& strTenTaiKhoan)
{
    strTenTaiKhoan.clear();
    while (true)
    {
        const int iChar = _getch();
        if (iChar == KEY_ESC)
        {
            cout << '\n';
            return false;
        }
        if (iChar == KEY_ENTER)
        {
            cout << '\n';
            return !strTenTaiKhoan.empty();
        }
        if (iChar == KEY_BACKSPACE)
        {
            if (!strTenTaiKhoan.empty())
            {
                strTenTaiKhoan.pop_back();
                cout << "\b \b";
            }
        }
        else if (iChar == 0 || iChar == KEY_ARROW_PREFIX)
        {
            _getch();
        }
        else if (iChar >= KEY_SPACE && iChar <= KEY_TILDE)
        {
            strTenTaiKhoan.push_back(static_cast<char>(iChar));
            cout << static_cast<char>(iChar);
        }
    }
}

// ==================== LOP ADMIN ====================
class Admin
{
private:
    string _strUser;
    string _strPassword;

public:
    Admin()
    {
        this->_strUser.clear();
        this->_strPassword.clear();
    }

    Admin(const string& strUser, const string& strPassword)
    {
        this->_strUser = strUser;
        this->_strPassword = strPassword;
    }

    bool docFileAdmin(const string& strFilename = FILE_ADMIN)
    {
        ifstream fileIn(strFilename);

        if (!fileIn.is_open())
        {
            return false;
        }

        string strUser;
        string strPassword;
        string strExtra;
        if (!(fileIn >> strUser >> strPassword) || (fileIn >> strExtra) ||
            strUser.empty() || strPassword.empty())
        {
            fileIn.close();
            return false;
        }

        fileIn.close();
        this->_strUser = strUser;
        this->_strPassword = strPassword;
        return true;
    }

    bool kiemTraDangNhap(const string& strUser, const string& strPassword) const
    {
        return this->_strUser == strUser && this->_strPassword == strPassword;
    }

    string getUser() const
    {
        return this->_strUser;
    }
};

// ==================== LOP QUAN LY KHACH HANG ====================
class QuanLyKhachHang
{
private:
    vector<TAIKHOAN> _vecDsTaiKhoan;
    string _strFilename;

    static bool tenTaiKhoanHopLe(const string& strUser)
    {
        if (strUser.empty() || strUser.length() > 32)
        {
            return false;
        }
        for (string::size_type i = 0; i < strUser.length(); ++i)
        {
            const char ch = strUser[i];
            if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
                  (ch >= '0' && ch <= '9') || ch == '_' || ch == '-'))
            {
                return false;
            }
        }
        return true;
    }

    static bool matKhauHopLe(const string& strPassword)
    {
        return !strPassword.empty() && strPassword.length() <= 128 &&
               strPassword.find_first_of(" \t\r\n|") == string::npos;
    }

public:
    QuanLyKhachHang()
    {
        this->_strFilename = FILE_ACCOUNT;
    }

    void docFile()
    {
        this->_vecDsTaiKhoan.clear();
        ifstream fileIn(this->_strFilename);
        if (!fileIn.is_open())
        {
            return;
        }

        string strDong;
        size_t iDong = 0;
        while (getline(fileIn, strDong))
        {
            ++iDong;
            istringstream streamDong(strDong);
            TAIKHOAN taiKhoan;
            string strExtra;
            if (!(streamDong >> taiKhoan.User >> taiKhoan.Password) ||
                (streamDong >> strExtra) ||
                !QuanLyKhachHang::tenTaiKhoanHopLe(taiKhoan.User) ||
                !QuanLyKhachHang::matKhauHopLe(taiKhoan.Password) ||
                this->kiemTraTonTai(taiKhoan.User))
            {
                cerr << "[Canh bao] Bo qua tai khoan khong hop le tai dong "
                     << iDong << " trong " << this->_strFilename << ".\n";
                continue;
            }
            this->_vecDsTaiKhoan.push_back(taiKhoan);
        }
        if (fileIn.bad())
        {
            cerr << "[Loi] Xay ra loi khi doc tep " << this->_strFilename << ".\n";
        }
        fileIn.close();
    }

    bool ghiFile(string& strLoi) const
    {
        const string strTepTam = this->_strFilename + ".tmp";
        ofstream fileOut(strTepTam.c_str(), ios::out | ios::trunc | ios::binary);
        if (!fileOut.is_open())
        {
            strLoi = "Khong tao duoc tep tam de luu tai khoan.";
            return false;
        }

        for (const TAIKHOAN& taiKhoan : this->_vecDsTaiKhoan)
        {
            fileOut << taiKhoan.User << " " << taiKhoan.Password << "\n";
        }
        fileOut.flush();
        if (!fileOut)
        {
            fileOut.close();
            DeleteFileA(strTepTam.c_str());
            strLoi = "Loi khi ghi tep tam tai khoan.";
            return false;
        }
        fileOut.close();
        if (!fileOut)
        {
            DeleteFileA(strTepTam.c_str());
            strLoi = "Khong dong duoc tep tam tai khoan.";
            return false;
        }
        if (!MoveFileExA(strTepTam.c_str(), this->_strFilename.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        {
            const DWORD dwLoi = GetLastError();
            DeleteFileA(strTepTam.c_str());
            strLoi = "Khong the thay tep tai khoan. Ma loi Windows: " +
                     to_string(dwLoi);
            return false;
        }
        strLoi.clear();
        return true;
    }

    bool kiemTraTonTai(const string& strUser) const
    {
        for (const TAIKHOAN& taiKhoan : this->_vecDsTaiKhoan)
        {
            if (taiKhoan.User == strUser)
            {
                return true;
            }
        }
        return false;
    }

    void dangKyTaiKhoan(const string& strAdminUser)
    {
        system("cls");
        setColor(COLOR_YELLOW);
        cout << "========================================\n";
        cout << "           DANG KY TAI KHOAN            \n";
        cout << "========================================\n";
        setColor(COLOR_DEFAULT);

        string strUser = "";
        string strPassword = "";
        string strPasswordConfirm = "";

        cout << "Nhap ten tai khoan moi (Viet lien khong dau): ";
        if (!(cin >> strUser))
        {
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (!QuanLyKhachHang::tenTaiKhoanHopLe(strUser) ||
            strUser == strAdminUser || this->kiemTraTonTai(strUser))
        {
            setColor(COLOR_RED);
            cout << "\n[-] Ten tai khoan khong hop le hoac da ton tai. "
                 << "Chi dung chu cai, chu so, '_' va '-', toi da 32 ky tu.\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            return;
        }

        cout << "Nhap mat khau (Nhan ESC de huy): ";
        if (!nhapMatKhau(strPassword))
        {
            setColor(COLOR_YELLOW);
            cout << "\n[!] Huy thao tac dang ky.\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            return;
        }

        cout << "Xac nhan mat khau (Nhan ESC de huy): ";
        if (!nhapMatKhau(strPasswordConfirm))
        {
            setColor(COLOR_YELLOW);
            cout << "\n[!] Huy thao tac dang ky.\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            return;
        }

        if (!QuanLyKhachHang::matKhauHopLe(strPassword))
        {
            setColor(COLOR_RED);
            cout << "\n[-] Mat khau phai co 1-128 ky tu va khong chua khoang trang hoac '|'.\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            return;
        }

        if (strPassword != strPasswordConfirm)
        {
            setColor(COLOR_RED);
            cout << "\n[-] Mat khau xac nhan khong khop!\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            return;
        }

        TAIKHOAN taiKhoanMoi = {strUser, strPassword};
        this->_vecDsTaiKhoan.push_back(taiKhoanMoi);
        string strLoi;
        if (!this->ghiFile(strLoi))
        {
            this->_vecDsTaiKhoan.pop_back();
            setColor(COLOR_RED);
            cout << "\n[-] Khong luu duoc tai khoan: " << strLoi << "\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            return;
        }

        setColor(COLOR_GREEN);
        cout << "\n[+] Dang ky thanh cong! Ban da co the dang nhap ngay.\n";
        setColor(COLOR_DEFAULT);
        system("pause");
    }

    bool kiemTraDangNhap(const string& strUser, const string& strPassword) const
    {
        for (const TAIKHOAN& taiKhoan : this->_vecDsTaiKhoan)
        {
            if (taiKhoan.User == strUser && taiKhoan.Password == strPassword)
            {
                return true;
            }
        }
        return false;
    }

    void hienThiDanhSach() const
    {
        setColor(COLOR_YELLOW);
        cout << "\n=== DANH SACH TAI KHOAN KHACH HANG (Account.txt) ===\n";
        setColor(COLOR_DEFAULT);
        if (this->_vecDsTaiKhoan.empty())
        {
            cout << "(Chua co tai khoan khach hang nao)\n";
            return;
        }
        cout << "STT\tTai khoan\n";
        cout << "-----------------------------------\n";
        for (size_t i = 0; i < this->_vecDsTaiKhoan.size(); ++i)
        {
            cout << (i + 1) << "\t" << this->_vecDsTaiKhoan[i].User << "\n";
        }
        cout << "-----------------------------------\n";
    }
};

int dangNhapHeThong(const Admin& adminAcc, const QuanLyKhachHang& qlKhachHang, string& strCurrentUsername)
{
    int iLanThu = 0;
    while (iLanThu < MAX_LOGIN_ATTEMPTS)
    {
        system("cls");

        setColor(COLOR_CYAN);
        cout << "****************************************\n";
        cout << "*          DANG NHAP HE THONG          *\n";
        cout << "****************************************\n";
        setColor(COLOR_DEFAULT);

        cout << "(Nhan ESC khi nhap mat khau de quay lai man hinh truoc)\n\n";
        string strUser = "";
        string strPassword = "";

        cout << "(Nhan ESC tai User hoac Password de quay lai)\n";
        cout << "User: ";
        if (!nhapTenTaiKhoanDangNhap(strUser))
        {
            cout << "[!] Da huy dang nhap.\n";
            return ROLE_EXIT;
        }

        cout << "Password: ";
        if (!nhapMatKhau(strPassword))
        {
            setColor(COLOR_YELLOW);
            cout << "\n[!] Ban da nhan ESC. Huy thao tac dang nhap.\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            return ROLE_EXIT;
        }

        if (adminAcc.kiemTraDangNhap(strUser, strPassword))
        {
            strCurrentUsername = strUser;
            setColor(COLOR_GREEN);
            cout << "\n[+] Dang nhap thanh cong voi TOAN QUYEN ADMIN!\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            return ROLE_ADMIN;
        }
        else if (qlKhachHang.kiemTraDangNhap(strUser, strPassword))
        {
            strCurrentUsername = strUser;
            setColor(COLOR_GREEN);
            cout << "\n[+] Dang nhap thanh cong! Chao mung KHACH HANG.\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            return ROLE_CUSTOMER;
        }
        else
        {
            ++iLanThu;
            setColor(COLOR_RED);
            cout << "\n[-] Sai tai khoan hoac mat khau! (Lan " << iLanThu << "/" << MAX_LOGIN_ATTEMPTS << ")\n";
            setColor(COLOR_DEFAULT);
            system("pause");
        }
    }

    setColor(COLOR_RED);
    cout << "\n[!] Ban da nhap sai qua " << MAX_LOGIN_ATTEMPTS << " lan. Quay lai man hinh khoi dong.\n";
    setColor(COLOR_DEFAULT);
    system("pause");
    return ROLE_EXIT;
}

int manHinhKhoiDong(const Admin& adminAcc, QuanLyKhachHang& qlKhachHang, string& strCurrentUsername)
{
    int iChon = -1;
    while (true)
    {
        system("cls");
        setColor(COLOR_CYAN);
        cout << "========================================\n";
        cout << "      HE THONG QUAN LY - KHOI DONG      \n";
        cout << "========================================\n";
        setColor(COLOR_DEFAULT);
        cout << "1. Dang nhap\n";
        cout << "2. Dang ky tai khoan khach hang moi\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "----------------------------------------\n";
        cout << "Chon chuc nang: ";

        if (!(cin >> iChon))
        {
            if (cin.eof())
            {
                return ROLE_EXIT;
            }
            xoaBoDem();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (iChon)
        {
            case 1:
            {
                int iRole = dangNhapHeThong(adminAcc, qlKhachHang, strCurrentUsername);
                if (iRole != ROLE_EXIT)
                {
                    return iRole;
                }
                break;
            }
            case 2:
                qlKhachHang.dangKyTaiKhoan(adminAcc.getUser());
                break;
            case 0:
                return ROLE_EXIT;
            default:
                setColor(COLOR_RED);
                cout << "\n[Loi] Lua chon khong hop le!\n";
                setColor(COLOR_DEFAULT);
                system("pause");
                break;
        }
    }
}

bool luuHangHoaHoacKhoiPhuc(
    DanhSachHangHoa& dsHangHoa,
    DanhSachHangHoa& dsHangHoaTruoc,
    const HangHoaRepository& repositoryHangHoa
)
{
    string strLoiLuu = "";
    if (!repositoryHangHoa.ghiFile(dsHangHoa, strLoiLuu))
    {
        dsHangHoa.swap(dsHangHoaTruoc);
        cout << "[Loi] Khong luu duoc hang hoa; du lieu trong bo nho da duoc khoi phuc. "
             << strLoiLuu << endl;
        return false;
    }

    if (!strLoiLuu.empty())
    {
        cout << "[Canh bao] Hang hoa da duoc luu, nhung backup can duoc kiem tra: "
             << strLoiLuu << endl;
    }

    return true;
}

bool luuMaGiamGiaHoacKhoiPhuc(
    DanhSachMaGiamGia& dsMaGiamGia,
    const DanhSachMaGiamGia& dsMaGiamGiaTruoc
)
{
    if (!dsMaGiamGia.ghiFile())
    {
        dsMaGiamGia = dsMaGiamGiaTruoc;
        cout << "[Loi] Khong luu duoc ma giam gia; thay doi trong bo nho da duoc khoi phuc.\n";
        return false;
    }
    return true;
}

void menuChinh(const int iRole, const string& strCurrentUsername, DanhSachHangHoa& dsHangHoa, HangHoaConsole& giaoDienHangHoa, const HangHoaRepository& repositoryHangHoa, DanhSachMaGiamGia& dsMaGiamGia, OrderManager& orderManager, QuanLyKhachHang& qlKhachHang)
{
    int iChon = -1;
    do
    {
        system("cls");
        setColor(COLOR_YELLOW);
        cout << "========================================================\n";
        cout << "                 MENU CHINH HE THONG                    \n";
        setColor(COLOR_CYAN);
        cout << "  Xin chao: " << strCurrentUsername << "  |  ";
        COORD posDongHo = layToaDoConTro(); // Luu lai toa do ngay vi tri bat dau in dong ho
        cout << layThoiGianThucTe() << "\n";

        if (iRole == ROLE_ADMIN)
        {
            setColor(COLOR_GREEN);
            cout << "  [ Vai tro: ADMIN - TOAN QUYEN HE THONG ]              \n";
        }
        else
        {
            setColor(COLOR_CYAN);
            cout << "  [ Vai tro: KHACH HANG ]                               \n";
        }
        setColor(COLOR_YELLOW);
        cout << "========================================================\n";
        setColor(COLOR_DEFAULT);
        cout << "1. Hien thi danh sach hang hoa\n";
        cout << "2. Tim kiem hang hoa\n";
        cout << "3. Dat hang\n";
        cout << "4. Ap ma giam gia\n";
        cout << "5. Xem don hang cua toi\n";
        cout << "6. Huy don hang da dat\n";

        if (iRole == ROLE_ADMIN)
        {
            setColor(COLOR_MAGENTA);
            cout << "--------------------------------------------------------\n";
            cout << "  [ CHUC NANG TOAN QUYEN ADMIN ]\n";
            setColor(COLOR_DEFAULT);
            cout << "7. Them hang hoa moi\n";
            cout << "8. Cap nhat so luong hang hoa\n";
            cout << "9. Xoa hang hoa\n";
            cout << "10. Quan ly don hang\n";
            cout << "11. Xem danh sach ma giam gia\n";
            cout << "12. Them ma giam gia moi\n";
            cout << "13. Cap nhat so luong ma giam gia\n";
            cout << "14. Xoa ma giam gia\n";
            cout << "15. Xem danh sach tai khoan Khach hang\n";
        }

        cout << "--------------------------------------------------------\n";
        cout << "0. Dang xuat\n";
        cout << "--------------------------------------------------------\n";
        cout << "Nhap lua chon cua ban: ";

        // Vua cho nhap lua chon, vua cap nhat dong ho moi giay tai posDongHo
        if (!nhapLuaChonVoiDongHo(iChon, posDongHo))
        {
            setColor(COLOR_RED);
            cout << "\n[Loi] Kieu du lieu khong hop le! Vui long nhap lai.\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            continue;
        }

        if (iRole != ROLE_ADMIN && iChon >= 7 && iChon <= 15)
        {
            setColor(COLOR_RED);
            cout << "\n[Loi] Lua chon khong hop le!\n";
            setColor(COLOR_DEFAULT);
            system("pause");
            continue;
        }

        try
        {
            switch (iChon)
            {
                case 1:
                {
                    int iChonSapXep = -1;
                    cout << "\n--- HIEN THI DANH SACH HANG HOA ---\n";
                    cout << "1. Sap xep theo gia tu cao den thap\n";
                    cout << "2. Sap xep theo gia tu thap den cao\n";
                    cout << "3. Hien thi theo danh sach hang hoa goc\n";
                    cout << "0. Quay lai menu chinh\n";
                    cout << "Nhap lua chon: ";

                    if (!(cin >> iChonSapXep))
                    {
                        if (cin.eof())
                        {
                            return;
                        }
                        xoaBoDem();
                        setColor(COLOR_RED);
                        cout << "\n[Loi] Lua chon khong hop le!\n";
                        setColor(COLOR_DEFAULT);
                        system("pause");
                        break;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (iChonSapXep == 1)
                    {
                        cout << "\n--- DANH SACH HANG HOA: GIA CAO DEN THAP ---\n";
                        giaoDienHangHoa.hienThiSapXep(
                            dsHangHoa, false, iRole != ROLE_ADMIN);
                        system("pause");
                    }
                    else if (iChonSapXep == 2)
                    {
                        cout << "\n--- DANH SACH HANG HOA: GIA THAP DEN CAO ---\n";
                        giaoDienHangHoa.hienThiSapXep(
                            dsHangHoa, true, iRole != ROLE_ADMIN);
                        system("pause");
                    }
                    else if (iChonSapXep == 3)
                    {
                        cout << "\n--- DANH SACH HANG HOA GOC ---\n";
                        giaoDienHangHoa.hienThi(dsHangHoa, iRole != ROLE_ADMIN);
                        system("pause");
                    }
                    else if (iChonSapXep != 0)
                    {
                        setColor(COLOR_RED);
                        cout << "\n[Loi] Lua chon khong hop le!\n";
                        setColor(COLOR_DEFAULT);
                        system("pause");
                    }
                    break;
                }
                case 2:
                    cout << "\n--- TIM KIEM HANG HOA ---\n";
                    // Khong can xoaBoDem() vi nhapLuaChonVoiDongHo da doc sach phim Enter
                    giaoDienHangHoa.timKiem(dsHangHoa);
                    system("pause");
                    break;
                case 3:
                    cout << "\n--- DAT HANG ---\n";
                    orderManager.datHang(strCurrentUsername);
                    system("pause");
                    break;
                case 4:
                    cout << "\n--- AP MA GIAM GIA ---\n";
                    orderManager.menuApMaGiamGia();
                    system("pause");
                    break;
                case 5:
                    cout << "\n--- LICH SU DON HANG CUA TOI ---\n";
                    orderManager.hienThiDonHangCuaTaiKhoan(strCurrentUsername, false);
                    system("pause");
                    break;
                case 6:
                    cout << "\n--- HUY DON HANG DA DAT ---\n";
                    orderManager.huyDonHangKhachHang(strCurrentUsername);
                    system("pause");
                    break;
                case 7:
                    cout << "\n--- THEM HANG HOA MOI ---\n";
                    {
                        DanhSachHangHoa dsHangHoaTruoc = dsHangHoa;
                        if (giaoDienHangHoa.themHangHoa(dsHangHoa))
                        {
                            luuHangHoaHoacKhoiPhuc(dsHangHoa, dsHangHoaTruoc, repositoryHangHoa);
                        }
                    }
                    system("pause");
                    break;
                case 8:
                    cout << "\n--- CAP NHAT SO LUONG HANG HOA ---\n";
                    {
                        DanhSachHangHoa dsHangHoaTruoc = dsHangHoa;
                        if (giaoDienHangHoa.capNhatSoLuong(dsHangHoa))
                        {
                            luuHangHoaHoacKhoiPhuc(dsHangHoa, dsHangHoaTruoc, repositoryHangHoa);
                        }
                    }
                    system("pause");
                    break;
                case 9:
                    cout << "\n--- XOA HANG HOA ---\n";
                    {
                        DanhSachHangHoa dsHangHoaTruoc = dsHangHoa;
                        if (giaoDienHangHoa.xoaHangHoa(
                                dsHangHoa,
                                [&orderManager](const string& strMaHang)
                                {
                                    return orderManager.coDonChoTheoMaHang(strMaHang);
                                }))
                        {
                            luuHangHoaHoacKhoiPhuc(dsHangHoa, dsHangHoaTruoc, repositoryHangHoa);
                        }
                    }
                    system("pause");
                    break;
                case 10:
                    cout << "\n--- QUAN LY DON HANG ---\n";
                    orderManager.menuQuanLyDonHang();
                    break;
                case 11:
                    cout << "\n--- DANH SACH MA GIAM GIA ---\n";
                    dsMaGiamGia.hienThi();
                    system("pause");
                    break;
                case 12:
                    cout << "\n--- THEM MA GIAM GIA MOI ---\n";
                    {
                        const DanhSachMaGiamGia dsMaGiamGiaTruoc = dsMaGiamGia;
                        if (dsMaGiamGia.themMaGiamGia(dsHangHoa))
                        {
                            luuMaGiamGiaHoacKhoiPhuc(dsMaGiamGia, dsMaGiamGiaTruoc);
                        }
                    }
                    system("pause");
                    break;
                case 13:
                    cout << "\n--- CAP NHAT SO LUONG MA GIAM GIA ---\n";
                    {
                        const DanhSachMaGiamGia dsMaGiamGiaTruoc = dsMaGiamGia;
                        if (dsMaGiamGia.capNhatSoLuong())
                        {
                            luuMaGiamGiaHoacKhoiPhuc(dsMaGiamGia, dsMaGiamGiaTruoc);
                        }
                    }
                    system("pause");
                    break;
                case 14:
                    cout << "\n--- XOA MA GIAM GIA ---\n";
                    {
                        const DanhSachMaGiamGia dsMaGiamGiaTruoc = dsMaGiamGia;
                        if (dsMaGiamGia.xoaMaGiamGia())
                        {
                            luuMaGiamGiaHoacKhoiPhuc(dsMaGiamGia, dsMaGiamGiaTruoc);
                        }
                    }
                    system("pause");
                    break;
                case 15:
                    qlKhachHang.hienThiDanhSach();
                    system("pause");
                    break;
                case 0:
                    setColor(COLOR_CYAN);
                    cout << "\n[+] Da dang xuat thanh cong!\n";
                    setColor(COLOR_DEFAULT);
                    system("pause");
                    break;
                default:
                    setColor(COLOR_RED);
                    cout << "\n[Loi] Lua chon khong hop le!\n";
                    setColor(COLOR_DEFAULT);
                    system("pause");
                    break;
            }
        }
        catch (const exception& eError)
        {
            setColor(COLOR_RED);
            cout << "\n[Loi He Thong]: " << eError.what() << ". Da tro ve Menu.\n";
            setColor(COLOR_DEFAULT);
            system("pause");
        }
    } while (iChon != 0);
}

int main()
{
    Admin adminAcc;
    QuanLyKhachHang qlKhachHang;
    DanhSachHangHoa dsHangHoa;
    DanhSachMaGiamGia dsMaGiamGia;
    HangHoaRepository repositoryHangHoa;
    HangHoaConsole giaoDienHangHoa;

    string strLoiDocHangHoa = "";
    if (!repositoryHangHoa.docFile(dsHangHoa, strLoiDocHangHoa))
    {
        setColor(COLOR_RED);
        cout << "[!] " << strLoiDocHangHoa << "\n";
        setColor(COLOR_DEFAULT);
        system("pause");
        return 1;
    }
    if (!strLoiDocHangHoa.empty())
    {
        setColor(COLOR_YELLOW);
        cout << "[Canh bao] " << strLoiDocHangHoa << "\n";
        setColor(COLOR_DEFAULT);
    }

    dsMaGiamGia.docFile(dsHangHoa);
    OrderManager orderManager(dsHangHoa, dsMaGiamGia, repositoryHangHoa);
    orderManager.docFile();
    qlKhachHang.docFile();

    if (!adminAcc.docFileAdmin(FILE_ADMIN))
    {
        setColor(COLOR_RED);
        cout << "[!] Khong doc duoc thong tin quan tri hop le trong Admin.txt. "
             << "Tu choi khoi dong voi mat khau mac dinh.\n";
        setColor(COLOR_DEFAULT);
        system("pause");
        return 1;
    }

    while (true)
    {
        string strCurrentUsername = "";
        int iRole = manHinhKhoiDong(adminAcc, qlKhachHang, strCurrentUsername);
        if (iRole == ROLE_EXIT)
        {
            break;
        }

        menuChinh(iRole, strCurrentUsername, dsHangHoa, giaoDienHangHoa, repositoryHangHoa, dsMaGiamGia, orderManager, qlKhachHang);

        orderManager.ghiFile();
    }

    orderManager.ghiFile();

    return 0;
}