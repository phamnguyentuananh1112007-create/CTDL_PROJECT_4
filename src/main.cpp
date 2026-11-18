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
#include <filesystem>
#include <thread>
#include <atomic>
#include "..\include\Console.h"
#include "..\include\Application.h"

#include "..\include\HangHoa.h"
#include "..\include\DanhSachHangHoa.h"
#include "..\include\HangHoaConsole.h"
#include "..\include\HangHoaRepository.h"
#include "..\include\MaGiamGia.h"
#include "..\include\DanhSachMaGiamGia.h"
#include "..\include\OrderManager.h"

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
const int ROLE_GUEST = -1;
const int ROLE_EXIT = 0;
const int ROLE_CUSTOMER = 1;
const int ROLE_ADMIN = 2;

const char* const FILE_ADMIN = "data/Admin.txt";
const char* const FILE_ACCOUNT = "data/Account.txt";

bool datThuMucLamViecVeThuMucDuAn()
{
#ifdef _WIN32
    char szDuongDanExe[MAX_PATH] = {};
    const DWORD iSoKyTu = GetModuleFileNameA(NULL, szDuongDanExe, MAX_PATH);
    if (iSoKyTu == 0 || iSoKyTu == MAX_PATH)
    {
        return false;
    }

    const std::filesystem::path duongDanExe(szDuongDanExe);
    const std::filesystem::path thuMucDuAn = duongDanExe.parent_path().parent_path();
    return SetCurrentDirectoryA(thuMucDuAn.string().c_str()) != 0;
#else
    return false;
#endif
}

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
 * @Description Quan ly luong cap nhat dong ho thoi gian thuc an toan (RAII)
 *****************************************************************************/
class DongHoThoiGianThuc
{
private:
    atomic<bool> _dangChay;
    thread _luongCapNhat;

public:
    explicit DongHoThoiGianThuc(COORD toaDoDongHo) : _dangChay(true)
    {
        _luongCapNhat = thread([this, toaDoDongHo]()
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            while (_dangChay.load())
            {
                const string strThoiGian = layThoiGianThucTe();
                DWORD soKyTuDaGhi = 0;
                WriteConsoleOutputCharacterA(
                    hConsole,
                    strThoiGian.c_str(),
                    static_cast<DWORD>(strThoiGian.length()),
                    toaDoDongHo,
                    &soKyTuDaGhi
                );

                for (int i = 0; i < 10 && _dangChay.load(); ++i)
                {
                    Sleep(100);
                }
            }
        });
    }

    void dung()
    {
        if (_dangChay.exchange(false))
        {
            if (_luongCapNhat.joinable())
            {
                _luongCapNhat.join();
            }
        }
    }

    ~DongHoThoiGianThuc()
    {
        dung();
    }
};

/******************************************************************************
 * @Description Hien thi Header dong bo gom Tieu de, Xin chao, Vai tro & Dong ho
 *****************************************************************************/
COORD hienThiHeaderHeThong(
    const string& strTieuDe,
    const string& strUsername,
    const int iRole,
    const Console::Color mauTieuDe = Console::Color::Yellow
)
{
    Console::clear();
    Console::title(strTieuDe, mauTieuDe);

    setColor(COLOR_CYAN);
    cout << "Xin chao: " << (strUsername.empty() ? "Khach truy cap" : strUsername) << "  |  ";
    COORD toaDoDongHo = layToaDoConTro();
    cout << layThoiGianThucTe() << "\n";

    if (iRole == ROLE_ADMIN)
    {
        setColor(COLOR_GREEN);
        cout << "  [ Vai tro: ADMIN - TOAN QUYEN HE THONG ]\n";
    }
    else if (iRole == ROLE_CUSTOMER)
    {
        setColor(COLOR_CYAN);
        cout << "  [ Vai tro: KHACH HANG ]\n";
    }
    else
    {
        setColor(COLOR_YELLOW);
        cout << "  [ Trang thai: CHUA DANG NHAP ]\n";
    }

    setColor(COLOR_YELLOW);
    cout << "========================================================\n";
    setColor(COLOR_DEFAULT);
    return toaDoDongHo;
}

/******************************************************************************
 * @Description Hien thi khung bang menu tuy chon dep va dong bo
 *****************************************************************************/
void hienThiBangMenu(
    const vector<string>& dsMucChon,
    const string& strMucQuayLai = "Quay lai"
)
{
    setColor(COLOR_DEFAULT);
    for (size_t i = 0; i < dsMucChon.size(); ++i)
    {
        cout << " " << (i + 1) << ". " << dsMucChon[i] << "\n";
    }
    cout << "--------------------------------------------------------\n";
    cout << " 0. " << strMucQuayLai << "\n";
    cout << "--------------------------------------------------------\n";
    cout << "Nhap lua chon cua ban: ";
}

/******************************************************************************
 * @Description Doc lua chon so nguyen tu ban phim
 *****************************************************************************/
bool nhapLuaChon(int& iChon)
{
    if (!(cin >> iChon))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
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
                cerr << "[!] Bo qua tai khoan khong hop le tai dong "
                     << iDong << " trong " << this->_strFilename << ".\n";
                continue;
            }
            this->_vecDsTaiKhoan.push_back(taiKhoan);
        }
        if (fileIn.bad())
        {
            cerr << "[-] Xay ra loi khi doc tep " << this->_strFilename << ".\n";
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
        hienThiHeaderHeThong("DANG KY TAI KHOAN", "", ROLE_GUEST, Console::Color::Yellow);

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
            cout << "\n";
            Console::error("Ten tai khoan khong hop le hoac da ton tai. Chi dung chu cai, chu so, '_' va '-', toi da 32 ky tu.");
            Console::pause();
            return;
        }

        cout << "Nhap mat khau (Nhan ESC de huy): ";
        if (!nhapMatKhau(strPassword))
        {
            cout << "\n";
            Console::warning("Huy thao tac dang ky.");
            Console::pause();
            return;
        }

        cout << "Xac nhan mat khau (Nhan ESC de huy): ";
        if (!nhapMatKhau(strPasswordConfirm))
        {
            cout << "\n";
            Console::warning("Huy thao tac dang ky.");
            Console::pause();
            return;
        }

        if (!QuanLyKhachHang::matKhauHopLe(strPassword))
        {
            cout << "\n";
            Console::error("Mat khau phai co 1-128 ky tu va khong chua khoang trang hoac '|'.");
            Console::pause();
            return;
        }

        if (strPassword != strPasswordConfirm)
        {
            cout << "\n";
            Console::error("Mat khau xac nhan khong khop!");
            Console::pause();
            return;
        }

        TAIKHOAN taiKhoanMoi = {strUser, strPassword};
        this->_vecDsTaiKhoan.push_back(taiKhoanMoi);
        string strLoi;
        if (!this->ghiFile(strLoi))
        {
            this->_vecDsTaiKhoan.pop_back();
            cout << "\n";
            Console::error("Khong luu duoc tai khoan: " + strLoi);
            Console::pause();
            return;
        }

        cout << "\n";
        Console::success("Dang ky thanh cong! Ban da co the dang nhap ngay.");
        Console::pause();
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
        if (this->_vecDsTaiKhoan.empty())
        {
            Console::info("(Chua co tai khoan khach hang nao trong Account.txt)");
            return;
        }
        Console::setColor(Console::Color::Yellow);
        cout << " STT\tTen tai khoan khach hang\n";
        Console::setColor(Console::Color::Default);
        cout << "--------------------------------------------------------\n";
        for (size_t i = 0; i < this->_vecDsTaiKhoan.size(); ++i)
        {
            cout << " " << (i + 1) << "\t" << this->_vecDsTaiKhoan[i].User << "\n";
        }
        cout << "--------------------------------------------------------\n";
    }
};

int dangNhapHeThong(const Admin& adminAcc, const QuanLyKhachHang& qlKhachHang, string& strCurrentUsername)
{
    int iLanThu = 0;
    while (iLanThu < MAX_LOGIN_ATTEMPTS)
    {
        hienThiHeaderHeThong("DANG NHAP HE THONG", "", ROLE_GUEST, Console::Color::Cyan);

        cout << "(Nhan ESC tai User hoac Password de quay lai)\n\n";
        string strUser = "";
        string strPassword = "";

        cout << "User: ";
        if (!nhapTenTaiKhoanDangNhap(strUser))
        {
            Console::warning("Da huy dang nhap.");
            return ROLE_EXIT;
        }

        cout << "Password: ";
        if (!nhapMatKhau(strPassword))
        {
            cout << "\n";
            Console::warning("Ban da nhan ESC. Huy thao tac dang nhap.");
            Console::pause();
            return ROLE_EXIT;
        }

        if (adminAcc.kiemTraDangNhap(strUser, strPassword))
        {
            strCurrentUsername = strUser;
            cout << "\n";
            Console::success("Dang nhap thanh cong voi TOAN QUYEN ADMIN!");
            Console::pause();
            return ROLE_ADMIN;
        }
        else if (qlKhachHang.kiemTraDangNhap(strUser, strPassword))
        {
            strCurrentUsername = strUser;
            cout << "\n";
            Console::success("Dang nhap thanh cong! Chao mung KHACH HANG.");
            Console::pause();
            return ROLE_CUSTOMER;
        }
        else
        {
            ++iLanThu;
            cout << "\n";
            Console::error("Sai tai khoan hoac mat khau! (Lan " + to_string(iLanThu) + "/" + to_string(MAX_LOGIN_ATTEMPTS) + ")");
            Console::pause();
        }
    }

    cout << "\n";
    Console::error("Ban da nhap sai qua " + to_string(MAX_LOGIN_ATTEMPTS) + " lan. Quay lai man hinh khoi dong.");
    Console::pause();
    return ROLE_EXIT;
}

int manHinhKhoiDong(const Admin& adminAcc, QuanLyKhachHang& qlKhachHang, string& strCurrentUsername)
{
    int iChon = -1;
    while (true)
    {
        COORD toaDoDongHo = hienThiHeaderHeThong(
            "HE THONG QUAN LY - KHOI DONG",
            "",
            ROLE_GUEST,
            Console::Color::Cyan
        );

        hienThiBangMenu({
            "Dang nhap",
            "Dang ky tai khoan khach hang moi"
        }, "Thoat chuong trinh");

        bool nhapThanhCong = false;
        {
            DongHoThoiGianThuc dongHo(toaDoDongHo);
            nhapThanhCong = nhapLuaChon(iChon);
        }

        if (!nhapThanhCong)
        {
            if (cin.eof())
            {
                return ROLE_EXIT;
            }
            cout << "\n";
            Console::error("Lua chon khong hop le!");
            Console::pause();
            continue;
        }

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
                cout << "\n";
                Console::error("Lua chon khong hop le!");
                Console::pause();
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
        Console::error("Khong luu duoc hang hoa; du lieu trong bo nho da duoc khoi phuc. " + strLoiLuu);
        return false;
    }

    if (!strLoiLuu.empty())
    {
        Console::warning("Hang hoa da duoc luu, nhung backup can duoc kiem tra: " + strLoiLuu);
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
        Console::error("Khong luu duoc ma giam gia; thay doi trong bo nho da duoc khoi phuc.");
        return false;
    }
    return true;
}

void menuChinh(
    const int iRole,
    const string& strCurrentUsername,
    DanhSachHangHoa& dsHangHoa,
    HangHoaConsole& giaoDienHangHoa,
    const HangHoaRepository& repositoryHangHoa,
    DanhSachMaGiamGia& dsMaGiamGia,
    OrderManager& orderManager,
    QuanLyKhachHang& qlKhachHang
)
{
    int iChon = -1;
    do
    {
        COORD toaDoDongHo = hienThiHeaderHeThong(
            "MENU CHINH HE THONG",
            strCurrentUsername,
            iRole,
            Console::Color::Yellow
        );

        cout << " 1. Hien thi danh sach hang hoa\n";
        cout << " 2. Tim kiem hang hoa\n";
        cout << " 3. Dat hang\n";
        cout << " 4. Ap ma giam gia\n";
        cout << " 5. Xem don hang cua toi\n";
        cout << " 6. Huy don hang da dat\n";

        if (iRole == ROLE_ADMIN)
        {
            setColor(COLOR_MAGENTA);
            cout << "--------------------------------------------------------\n";
            cout << "  [ CHUC NANG TOAN QUYEN ADMIN ]\n";
            setColor(COLOR_DEFAULT);
            cout << " 7. Them hang hoa moi\n";
            cout << " 8. Cap nhat so luong hang hoa\n";
            cout << " 9. Xoa hang hoa\n";
            cout << " 10. Quan ly don hang\n";
            cout << " 11. Xem danh sach ma giam gia\n";
            cout << " 12. Them ma giam gia moi\n";
            cout << " 13. Cap nhat so luong ma giam gia\n";
            cout << " 14. Xoa ma giam gia\n";
            cout << " 15. Xem danh sach tai khoan Khach hang\n";
        }

        cout << "--------------------------------------------------------\n";
        cout << " 0. Dang xuat\n";
        cout << "--------------------------------------------------------\n";
        cout << "Nhap lua chon cua ban: ";

        bool nhapThanhCong = false;
        {
            DongHoThoiGianThuc dongHo(toaDoDongHo);
            nhapThanhCong = nhapLuaChon(iChon);
        }

        if (!nhapThanhCong)
        {
            if (cin.eof())
            {
                return;
            }
            cout << "\n";
            Console::error("Kieu du lieu khong hop le! Vui long nhap lai.");
            Console::pause();
            continue;
        }

        if (iRole != ROLE_ADMIN && iChon >= 7 && iChon <= 15)
        {
            cout << "\n";
            Console::error("Lua chon khong hop le!");
            Console::pause();
            continue;
        }

        try
        {
            switch (iChon)
            {
                case 1:
                {
                    int iChonSapXep = -1;
                    while (true)
                    {
                        COORD toaDoDongHoCon = hienThiHeaderHeThong(
                            "HIEN THI DANH SACH HANG HOA",
                            strCurrentUsername,
                            iRole,
                            Console::Color::Cyan
                        );

                        hienThiBangMenu({
                            "Sap xep theo gia tu cao den thap",
                            "Sap xep theo gia tu thap den cao",
                            "Hien thi theo danh sach hang hoa goc"
                        }, "Quay lai menu chinh");

                        bool nhapConThanhCong = false;
                        {
                            DongHoThoiGianThuc dongHoCon(toaDoDongHoCon);
                            nhapConThanhCong = nhapLuaChon(iChonSapXep);
                        }

                        if (!nhapConThanhCong)
                        {
                            if (cin.eof())
                            {
                                return;
                            }
                            cout << "\n";
                            Console::error("Lua chon khong hop le!");
                            Console::pause();
                            continue;
                        }

                        if (iChonSapXep == 0)
                        {
                            break;
                        }
                        else if (iChonSapXep == 1)
                        {
                            hienThiHeaderHeThong(
                                "DANH SACH HANG HOA: GIA CAO DEN THAP",
                                strCurrentUsername,
                                iRole,
                                Console::Color::Cyan
                            );
                            giaoDienHangHoa.hienThiSapXep(dsHangHoa, false, iRole != ROLE_ADMIN);
                            Console::pause();
                        }
                        else if (iChonSapXep == 2)
                        {
                            hienThiHeaderHeThong(
                                "DANH SACH HANG HOA: GIA THAP DEN CAO",
                                strCurrentUsername,
                                iRole,
                                Console::Color::Cyan
                            );
                            giaoDienHangHoa.hienThiSapXep(dsHangHoa, true, iRole != ROLE_ADMIN);
                            Console::pause();
                        }
                        else if (iChonSapXep == 3)
                        {
                            hienThiHeaderHeThong(
                                "DANH SACH HANG HOA GOC",
                                strCurrentUsername,
                                iRole,
                                Console::Color::Cyan
                            );
                            giaoDienHangHoa.hienThi(dsHangHoa, iRole != ROLE_ADMIN);
                            Console::pause();
                        }
                        else
                        {
                            cout << "\n";
                            Console::error("Lua chon khong hop le!");
                            Console::pause();
                        }
                    }
                    break;
                }
                case 2:
                    hienThiHeaderHeThong("TIM KIEM HANG HOA", strCurrentUsername, iRole, Console::Color::Cyan);
                    giaoDienHangHoa.timKiem(dsHangHoa);
                    Console::pause();
                    break;
                case 3:
                    hienThiHeaderHeThong("DAT HANG", strCurrentUsername, iRole, Console::Color::Green);
                    orderManager.datHang(strCurrentUsername);
                    Console::pause();
                    break;
                case 4:
                    hienThiHeaderHeThong("AP MA GIAM GIA", strCurrentUsername, iRole, Console::Color::Yellow);
                    orderManager.menuApMaGiamGia();
                    Console::pause();
                    break;
                case 5:
                    hienThiHeaderHeThong("LICH SU DON HANG CUA TOI", strCurrentUsername, iRole, Console::Color::Cyan);
                    orderManager.hienThiDonHangCuaTaiKhoan(strCurrentUsername, false);
                    Console::pause();
                    break;
                case 6:
                    hienThiHeaderHeThong("HUY DON HANG DA DAT", strCurrentUsername, iRole, Console::Color::Red);
                    orderManager.huyDonHangKhachHang(strCurrentUsername);
                    Console::pause();
                    break;
                case 7:
                    hienThiHeaderHeThong("THEM HANG HOA MOI", strCurrentUsername, iRole, Console::Color::Green);
                    {
                        DanhSachHangHoa dsHangHoaTruoc = dsHangHoa;
                        if (giaoDienHangHoa.themHangHoa(dsHangHoa))
                        {
                            luuHangHoaHoacKhoiPhuc(dsHangHoa, dsHangHoaTruoc, repositoryHangHoa);
                        }
                    }
                    Console::pause();
                    break;
                case 8:
                    hienThiHeaderHeThong("CAP NHAT SO LUONG HANG HOA", strCurrentUsername, iRole, Console::Color::Yellow);
                    {
                        DanhSachHangHoa dsHangHoaTruoc = dsHangHoa;
                        if (giaoDienHangHoa.capNhatSoLuong(dsHangHoa))
                        {
                            luuHangHoaHoacKhoiPhuc(dsHangHoa, dsHangHoaTruoc, repositoryHangHoa);
                        }
                    }
                    Console::pause();
                    break;
                case 9:
                    hienThiHeaderHeThong("XOA HANG HOA", strCurrentUsername, iRole, Console::Color::Red);
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
                    Console::pause();
                    break;
                case 10:
                    hienThiHeaderHeThong("QUAN LY DON HANG", strCurrentUsername, iRole, Console::Color::Magenta);
                    orderManager.menuQuanLyDonHang();
                    break;
                case 11:
                    hienThiHeaderHeThong("DANH SACH MA GIAM GIA", strCurrentUsername, iRole, Console::Color::Cyan);
                    dsMaGiamGia.hienThi();
                    Console::pause();
                    break;
                case 12:
                    hienThiHeaderHeThong("THEM MA GIAM GIA MOI", strCurrentUsername, iRole, Console::Color::Green);
                    {
                        const DanhSachMaGiamGia dsMaGiamGiaTruoc = dsMaGiamGia;
                        if (dsMaGiamGia.themMaGiamGia(dsHangHoa))
                        {
                            luuMaGiamGiaHoacKhoiPhuc(dsMaGiamGia, dsMaGiamGiaTruoc);
                        }
                    }
                    Console::pause();
                    break;
                case 13:
                    hienThiHeaderHeThong("CAP NHAT SO LUONG MA GIAM GIA", strCurrentUsername, iRole, Console::Color::Yellow);
                    {
                        const DanhSachMaGiamGia dsMaGiamGiaTruoc = dsMaGiamGia;
                        if (dsMaGiamGia.capNhatSoLuong())
                        {
                            luuMaGiamGiaHoacKhoiPhuc(dsMaGiamGia, dsMaGiamGiaTruoc);
                        }
                    }
                    Console::pause();
                    break;
                case 14:
                    hienThiHeaderHeThong("XOA MA GIAM GIA", strCurrentUsername, iRole, Console::Color::Red);
                    {
                        const DanhSachMaGiamGia dsMaGiamGiaTruoc = dsMaGiamGia;
                        if (dsMaGiamGia.xoaMaGiamGia())
                        {
                            luuMaGiamGiaHoacKhoiPhuc(dsMaGiamGia, dsMaGiamGiaTruoc);
                        }
                    }
                    Console::pause();
                    break;
                case 15:
                    hienThiHeaderHeThong("DANH SACH TAI KHOAN KHACH HANG", strCurrentUsername, iRole, Console::Color::Cyan);
                    qlKhachHang.hienThiDanhSach();
                    Console::pause();
                    break;
                case 0:
                    cout << "\n";
                    Console::success("Da dang xuat thanh cong!");
                    Console::pause();
                    break;
                default:
                    cout << "\n";
                    Console::error("Lua chon khong hop le!");
                    Console::pause();
                    break;
            }
        }
        catch (const exception& eError)
        {
            cout << "\n";
            Console::error(string("Loi he thong: ") + eError.what() + ". Da tro ve Menu.");
            Console::pause();
        }
    } while (iChon != 0);
}

int main()
{
    if (!Application::prepareWorkingDirectory())
    {
        Console::error("Khong the xac dinh thu muc du an tu vi tri main.exe.");
        Console::pause();
        return 1;
    }

    Admin adminAcc;
    QuanLyKhachHang qlKhachHang;
    DanhSachHangHoa dsHangHoa;
    DanhSachMaGiamGia dsMaGiamGia;
    HangHoaRepository repositoryHangHoa;
    HangHoaConsole giaoDienHangHoa;

    string strLoiDocHangHoa = "";
    if (!repositoryHangHoa.docFile(dsHangHoa, strLoiDocHangHoa))
    {
        Console::error(strLoiDocHangHoa);
        Console::pause();
        return 1;
    }
    if (!strLoiDocHangHoa.empty())
    {
        Console::warning(strLoiDocHangHoa);
    }

    dsMaGiamGia.docFile(dsHangHoa);
    OrderManager orderManager(dsHangHoa, dsMaGiamGia, repositoryHangHoa);
    orderManager.docFile();
    qlKhachHang.docFile();

    if (!adminAcc.docFileAdmin(FILE_ADMIN))
    {
        Console::error("Khong doc duoc thong tin quan tri hop le trong Admin.txt. Tu choi khoi dong voi mat khau mac dinh.");
        Console::pause();
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